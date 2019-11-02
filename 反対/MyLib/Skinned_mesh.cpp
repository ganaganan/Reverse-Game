#define _CRT_SECURE_NO_WARNINGS
#include "Skinned_mesh.h"
#include "misc.h"
#include <fbxsdk.h>
#include <vector>
#include <functional>
#include "shader.h"
#include "texture.h"
#include "framework.h"

using namespace fbxsdk;

void FbxamatrixToXmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

SkinnedMesh::SkinnedMesh(ID3D11Device* _device, const char* _fbx_filename)
{
	//Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	//Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	//Create an inporter
	FbxImporter* importer = FbxImporter::Create(manager, "");

	//Initialize the importer
	bool import_status = false;
	import_status = importer->Initialize(_fbx_filename, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	//Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	//Import the contents of the file into the scene.
	import_status = importer->Import(scene);

	_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());

	//Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometry_converter(manager);
	geometry_converter.Triangulate(scene, /*repllace*/true);

	//Fetch node attributes and materials under this node recursively. Currently only mesh.
	std::vector<FbxNode*>fetched_meshes;
	std::function<void(FbxNode*)>traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
			if (fbx_node_attribute)
			{
				switch (fbx_node_attribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetched_meshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
				traverse(node->GetChild(i));
		}
	};
	traverse(scene->GetRootNode());

	// Loop of meshes
	meshes.resize(fetched_meshes.size());

	for (size_t i = 0; i < fetched_meshes.size(); i++)
	{
		fbxsdk::FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
		mesh& mesh = meshes.at(i);
		//TODO : ぐろーばるとらんすふぉーむをもらう
		FbxAMatrix globalTransform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
		FbxamatrixToXmfloat4x4(globalTransform, mesh.globalTransform);

		//Fetch material properties
		const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
		mesh.subsets.resize(number_of_materials > 0 ? number_of_materials : 1);	// UNIT.18
		for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
		{
			subset& subset = mesh.subsets.at(index_of_material);	// UNIT.18
			const FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);

			const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
			if (property.IsValid() && factor.IsValid())
			{
				FbxDouble3 color = property.Get<FbxDouble3>();
				double f = factor.Get<FbxDouble>();

				subset.diffuse.color.x = static_cast<float>(color[0] * f);
				subset.diffuse.color.y = static_cast<float>(color[1] * f);
				subset.diffuse.color.z = static_cast<float>(color[2] * f);
				subset.diffuse.color.w = 1.0f;
			}
			if (property.IsValid())
			{
				const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
				if (number_of_textures > 0)
				{
					const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
					const char* filename = file_texture->GetRelativeFileName();
					if (filename) // Textureがある場合
					{
						//const char* filename = file_texture->GetRelativeFileName();

						//Create "diffuse.shader_resource_view" from "filename".
						LoadTextureFile(filename, &subset.diffuse.shaderResourceView);
					}
				}
				else	// Textureがない場合
				{
					std::function<void(const DirectX::XMFLOAT4&, DWORD&)> convert = [&](const DirectX::XMFLOAT4 & colour, DWORD & RGBA)
					{
						DWORD R = static_cast<BYTE>(colour.x * 255);
						DWORD G = static_cast<BYTE>(colour.y * 255);
						DWORD B = static_cast<BYTE>(colour.z * 255);
						DWORD A = static_cast<BYTE>(colour.w * 255);
						RGBA = R | (G << 8) | (B << 16) | (A << 24);
					};

					DWORD RGBA = 0;
					convert(subset.diffuse.color, RGBA);
					MakeDummyTexture(_device, &subset.diffuse.shaderResourceView, RGBA, 1, true, true);
				}
			}
		}

		// Count the polygon count of each materal
		if (number_of_materials > 0)
		{
			// Count the faces of each material
			const int number_of_polygons = fbx_mesh->GetPolygonCount();
			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
			{
				const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
				mesh.subsets.at(material_index).index_count += 3;
			}

			// Record the offset (how many vertex)
			int offset = 0;
			for (subset& subset : mesh.subsets)
			{
				subset.index_start = offset;
				offset += subset.index_count;
				//This will be used as counter in the following procedures, reset to zero
				subset.index_count = 0;
			}
		}

		//Fetch mesh data
		std::vector<vertex>vertices;	//Vertex buffer
		std::vector<u_int>indices;		//Index buffer
		u_int vertex_count = 0;

		const FbxVector4* array_of_control_points = fbx_mesh->GetControlPoints();
		const int number_of_polygons = fbx_mesh->GetPolygonCount();
		indices.resize(number_of_polygons * 3);
		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
//		if (uv_names.GetCount() != 0)
//		{
//			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
//			{
//				// UNIT.18   // The material for current face.   
//				int index_of_material = 0;
//				if (number_of_materials > 0)
//				{
//					index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
//				}
//				// Where should I save the vertex attribute index, according to the material  
//				subset& subset = mesh.subsets.at(index_of_material);
//				const int index_offset = subset.index_start + subset.index_count;
//
//				for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
//				{
//					vertex vertex;
//					const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
//					vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
//					vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
//					vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
//
//					FbxVector4 normal;
//					fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
//					vertex.normal.x = static_cast<float>(normal[0]);
//					vertex.normal.y = static_cast<float>(normal[1]);
//					vertex.normal.z = static_cast<float>(normal[2]);
//
//					FbxVector2 uv;
//					bool unmapped_uv;
//					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
//					vertex.tex.x = static_cast<float>(uv[0]);
//					vertex.tex.y = 1.0f - static_cast<float>(uv[1]);
//
//					vertices.push_back(vertex);
////					indices.push_back(vertex_count);
//					indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
//					vertex_count += 1;
//				}
//				subset.index_count += 3;
//			}
//		}
//		else
//		{
//			for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
//			{
//				for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
//				{
//					vertex vertex;
//					const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
//					vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
//					vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
//					vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
//
//					FbxVector4 normal;
//					fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
//					vertex.normal.x = static_cast<float>(normal[0]);
//					vertex.normal.y = static_cast<float>(normal[1]);
//					vertex.normal.z = static_cast<float>(normal[2]);
//
//					vertices.push_back(vertex);
//					indices.push_back(vertex_count);
//					vertex_count++;
//				}
//			}
//		}
		for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
		{
			// UNIT.18
			// The material for current face.
			int index_of_material = 0;
			if (number_of_materials > 0)
			{
				index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
			}

			// Where should I save the vertex attribute index, according to the material
			// UNIT.19
			//subset &subset = subsets.at(index_of_material);
			subset& subset = mesh.subsets.at(index_of_material);
			const int index_offset = subset.index_start + subset.index_count;

			for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
			{
				vertex vertex;
				const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
				vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
				vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
				vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);

				// UNIT.17
				if (fbx_mesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				// UNIT.17
				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
					vertex.tex.x = static_cast<float>(uv[0]);
					vertex.tex.y = 1.0f - static_cast<float>(uv[1]);
				}

				vertices.push_back(vertex);

				// UNIT.18
				//indices.push_back(vertex_count);
				indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
				vertex_count += 1;
			}
			subset.index_count += 3;
		}
		mesh.CreateBuffer(_device, vertices.data(), vertices.size(), indices.data(), indices.size());
	}
	manager->Destroy();

	//Initialize Direct3D COM objects

	// TODO : ブレークポイントで止めて、先生のサンプルとsubsetsやmeshesの値が変わらないか見る

	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	create_vs_from_cso(_device, "Data/Shaders/skinned_mesh_vs.cso", &vertexShader, &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(_device, "Data/Shaders/skinned_mesh_ps.cso", &pixelShader);

	//create rasterizer state : solid mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = _device->CreateRasterizerState(&rasterizer_desc, &rasterizerState[0]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create rasterizer state : wireframe mode
	{
		D3D11_RASTERIZER_DESC rasterizer_desc = {};
		rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		rasterizer_desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = _device->CreateRasterizerState(&rasterizer_desc, &rasterizerState[1]);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create depth stencil state
	{
		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = FALSE;
		depth_stencil_desc.StencilReadMask = 0xFF;
		depth_stencil_desc.StencilWriteMask = 0xFF;
		depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = _device->CreateDepthStencilState(&depth_stencil_desc, &depthStencilState);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// create sampler state
	{
		D3D11_SAMPLER_DESC smpDesc;
		ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
		smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		hr = FRAMEWORK->GetDevice()->CreateSamplerState(&smpDesc, &samplerState);
		assert(SUCCEEDED(hr) && "Skinned_mesh.cpp Line: 387");
	}
	// create constant buffer
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(cbuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		hr = _device->CreateBuffer(&buffer_desc, nullptr, &constantBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

SkinnedMesh::~SkinnedMesh()
{
	constantBuffer->Release();
	vertexShader->Release();
	pixelShader->Release();
	inputLayout->Release();
	rasterizerState[0]->Release();
	rasterizerState[1]->Release();
	depthStencilState->Release();
	constantBuffer = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	inputLayout = nullptr;
	rasterizerState[0] = nullptr;
	rasterizerState[1] = nullptr;
	depthStencilState = nullptr;
}

void	SkinnedMesh::mesh::CreateBuffer(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
{
	HRESULT hr;
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex) * num_vertices;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertexBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(u_int) * num_indices;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, &indexBuffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

#if 0
void	SkinnedMesh::Render(ID3D11DeviceContext* _context, const DirectX::XMFLOAT4X4 &_WVP, const DirectX::XMFLOAT4X4 &_world, const DirectX::XMFLOAT4 &_lightDirection, DirectX::XMFLOAT4 &_materialColor, bool wireframe)
{
	//サブセット単位で描画できるようにしようず
	for (size_t i = 0; i < meshes.size(); i++)
	{
		mesh& mesh = meshes.at(i);
		if (mesh.subsets.size() != 0)
		{
			for (auto& subset : mesh.subsets)
			{
				cbuffer data;
				//data.worldViewProjection = _WVP;
				//data.world = _world;
				DirectX::XMStoreFloat4x4(&data.worldViewProjection, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&_WVP));	// WVP * globalTransform
				DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&_world));				// world * globalTransform
				data.lightDirection = _lightDirection;
				data.materialColor.x = _materialColor.x * subset.diffuse.color.x;
				data.materialColor.y = _materialColor.y * subset.diffuse.color.y;
				data.materialColor.z = _materialColor.z * subset.diffuse.color.z;
				_context->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
				_context->VSSetConstantBuffers(0, 1, &constantBuffer);
				_context->PSSetShaderResources(0, 1, &subset.diffuse.shaderResourceView);

				_context->PSSetSamplers(0, 1, &samplerState);

				u_int stride = sizeof(vertex);
				u_int offset = 0;
				_context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
				_context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
				_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				_context->IASetInputLayout(inputLayout);

				_context->VSSetShader(vertexShader, nullptr, 0);
				_context->PSSetShader(pixelShader, nullptr, 0);

				_context->OMSetDepthStencilState(depthStencilState, 1);
				if (wireframe)
				{
					_context->RSSetState(rasterizerState[1]);
				}
				else
				{
					_context->RSSetState(rasterizerState[0]);
				}

				_context->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
		else
		{
			cbuffer data;
			data.worldViewProjection = _WVP;
			data.world = _world;
			data.lightDirection = _lightDirection;
			data.materialColor = _materialColor;
			_context->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
			_context->VSSetConstantBuffers(0, 1, &constantBuffer);

			u_int stride = sizeof(vertex);
			u_int offset = 0;
			_context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
			_context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_context->IASetInputLayout(inputLayout);

			//		_context->PSSetShaderResources(0, 1, &diffuse.shaderResourceView);
			_context->PSSetSamplers(0, 1, &samplerState);

			_context->VSSetShader(vertexShader, nullptr, 0);
			_context->PSSetShader(pixelShader, nullptr, 0);

			_context->OMSetDepthStencilState(depthStencilState, 1);
			if (wireframe)
			{
				_context->RSSetState(rasterizerState[1]);
			}
			else
			{
				_context->RSSetState(rasterizerState[0]);
			}

			D3D11_BUFFER_DESC buffer_desc;
			mesh.indexBuffer->GetDesc(&buffer_desc);
			_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(u_int), 0, 0);
		}
	}
}
#else
void	SkinnedMesh::Render(ID3D11DeviceContext* _context, const DirectX::XMFLOAT4X4& _WVP, const DirectX::XMFLOAT4X4& _world, const DirectX::XMFLOAT4& _lightDirection, DirectX::XMFLOAT4& _materialColor, bool wireframe)
{
	for (mesh& mesh : meshes)
	{
		u_int stride = sizeof(vertex);
		u_int offset = 0;

		_context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
		_context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_context->IASetInputLayout(inputLayout);

		_context->VSSetShader(vertexShader, nullptr, 0);
		_context->PSSetShader(pixelShader, nullptr, 0);

		_context->OMSetDepthStencilState(depthStencilState, 1);
		if (wireframe)
		{
			_context->RSSetState(rasterizerState[1]);
		}
		else
		{
			_context->RSSetState(rasterizerState[0]);
		}

		cbuffer data;
		// UNIT.19
		DirectX::XMStoreFloat4x4(&data.worldViewProjection, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&_WVP));
		DirectX::XMStoreFloat4x4(&data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&_world));

		data.lightDirection = _lightDirection;

		data.lightPosRight = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		data.lightColorRight = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		data.attenuation = DirectX::XMFLOAT4(1.0f, 0.0f, 0.2f, 1.0f);

		for (subset& subset : mesh.subsets)
		{
			data.materialColor.x = subset.diffuse.color.x * _materialColor.x;
			data.materialColor.y = subset.diffuse.color.y * _materialColor.y;
			data.materialColor.z = subset.diffuse.color.z * _materialColor.z;
			data.materialColor.w = _materialColor.w;
			_context->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
			_context->VSSetConstantBuffers(0, 1, &constantBuffer);
//			_context->PSSetConstantBuffers(0, 1, &constantBuffer);

			_context->PSSetShaderResources(0, 1, &subset.diffuse.shaderResourceView);
			_context->PSSetSamplers(0, 1, &samplerState);
			_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}
}
#endif

void SkinnedMesh::LoadTextureFile(const char* _fileName, ID3D11ShaderResourceView** _shaderResourceView)
{
	HRESULT hr = S_OK;
	wchar_t name[100];

	D3D11_TEXTURE2D_DESC		texture2d;	//Dummy

	setlocale(LC_CTYPE, "jpn");
	mbstowcs(name, _fileName, 100);
	hr = load_texture_from_file(FRAMEWORK->GetDevice(), name, _shaderResourceView, &texture2d);
	assert(SUCCEEDED(hr) && "Skinned_mesh.cpp Line: 378");
}