#include "static_mesh.h"
#include "shader.h"
#include "misc.h"
#include <fstream>
#include <vector>
#include "texture.h"
#include<cassert>
#include"framework.h"

StaticMesh::StaticMesh(ID3D11Device* _device, const wchar_t* _objFileName)
{
	std::vector<vertex> vertices;
	std::vector<u_int> indices;


	u_int currentIndex = 0;
	static std::wstring wst;


	int indexCount = 0;
	int indexStart = 0;

	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<DirectX::XMFLOAT2> texcoords;

	std::wifstream fin(_objFileName);
	_ASSERT_EXPR(fin, L"'OBJ file not fuund.");


	wchar_t command[256];
	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"v"))
		{
			// v x y z w
			// 
			// Specifies a geometric vertex and its x y z coordinates.Rational
			// curves and surfaces require a fourth homogeneous coordinate, also
			// called the weight.
			// 
			// x y z are the x, y, and z coordinates for the vertex.These are
			// floating point numbers that define the position of the vertex in
			// three dimensions.
			// 
			// w is the weight required for rational curves and surfaces.It is
			// not required for non - rational curves and surfaces.If you do not
			// specify a value for w, the default is 1.0.
			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back(DirectX::XMFLOAT3(x, y, z));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vn"))
		{
			// vn i j k
			// 
			// Specifies a normal vector with components i, j, and k.
			// 
			// Vertex normals affect the smooth - shading and rendering of geometry.0
			// For polygons, vertex normals are used in place of the actual facet
			// normals.For surfaces, vertex normals are interpolated over the
			// entire surface and replace the actual analytic surface normal.
			// 
			// When vertex normals are present, they supersede smoothing groups.
			// 
			// i j k are the i, j, and k coordinates for the vertex normal.They
			// are floating point numbers.
			FLOAT i, j, k;
			fin >> i >> j >> k;
			normals.push_back(DirectX::XMFLOAT3(i, j, k));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt"))
		{
			float u,v;
			fin >> u >> v;

			v = 1 - v;

			texcoords.push_back(DirectX::XMFLOAT2(u,v));
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"usemtl"))
		{
			if (indexCount != 0)
			{
				subset s;
				s.index_count = indexCount;
				s.index_start = indexStart;
				s.usemtl = wst;
				subsets.push_back(s);
			}
			indexStart = indexCount;
			indexCount = 0;

			fin >> wst;
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"f"))
		{
			//f  v1 / vt1 / vn1   v2 / vt2 / vn2   v3 / vt3 / vn3 . . .
			//
			// optionally include the texture vertex and vertex normal reference
			// numbers.
			// 
			// The reference numbers for the vertices, texture vertices, and
			// vertex normals must be separated by slashes(/ ).There is no space
			// between the number and the slash.
			// 
			// v is the reference number for a vertex in the face element.A
			// minimum of three vertices are required.
			// 
			// vt is an optional argument.
			// 
			// vt is the reference number for a texture vertex in the face
			// element.It always follows the first slash.
			// 
			// vn is an optional argument.
			// 
			// vn is the reference number for a vertex normal in the face element.
			// It must always follow the second slash.
			// 
			// Face elements use surface normals to indicate their orientation.If
			// vertices are ordered counterclockwise around the face, both the
			// face and the normal will point toward the viewer.If the vertex
			// ordering is clockwise, both will point away from the viewer.If
			// vertex normals are assigned, they should point in the general
			// direction of the surface normal, otherwise unpredictable results
			// may occur.
			//
			// If a face has a texture map assigned to it and no texture vertices
			// are assigned in the f statement, the texture map is ignored when
			// the element is rendered.
			for (u_int i = 0; i < 3; i++)
			{
				vertex vertex;
				u_int v, vt, vn;

				fin >> v;
				vertex.position = positions[v - 1];
				if (L'/' == fin.peek())
				{
					fin.ignore();
					if (L'/' != fin.peek())
					{
						fin >> vt;
						vertex.tex = texcoords[vt - 1];
					}
					if (L'/' == fin.peek())
					{
						fin.ignore();
						fin >> vn;
						vertex.normal = normals[vn - 1];
					}
				}
				vertices.push_back(vertex);
				indices.push_back(currentIndex++);
				++indexCount;
			}
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"mtllib"))
		{
			wchar_t mtlName[256];
			fin >> mtlName;
			LoadMtlFile(mtlName);
		}
		else if (!fin)
		{
			subset s;
			s.index_count = indexCount;
			s.index_start = indexStart;
			s.usemtl = wst;
			subsets.push_back(s);
		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}


	fin.close();

	CreateBuffer(_device, vertices.data(), vertices.size(), indices.data(), indices.size());

	HRESULT hr = S_OK;

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	create_vs_from_cso(_device, "static_mesh_vs.cso", &vertexShader, &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(_device, "static_mesh_ps.cso", &pixelShader);

	// create rasterizer state : solid mode
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

StaticMesh::~StaticMesh()
{
	vertexBuffer-> Release();
	indexBuffer-> Release();
	constantBuffer-> Release();
	vertexShader-> Release();
	pixelShader-> Release();
	inputLayout-> Release();
	rasterizerState[0]-> Release();
	rasterizerState[1]->Release();
	depthStencilState-> Release();
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	constantBuffer = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	inputLayout = nullptr;
	rasterizerState[0] = nullptr;
	rasterizerState[1] = nullptr;
	depthStencilState = nullptr;
}

void	StaticMesh::CreateBuffer(ID3D11Device *device, vertex *vertices, int num_vertices, u_int *indices, int num_indices)
{
	HRESULT hr;
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex)*num_vertices;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		//buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
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

		buffer_desc.ByteWidth = sizeof(u_int)*num_indices;
		//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
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

void	StaticMesh::LoadMtlFile(wchar_t* _mtlFileName)
{
	std::wifstream fin(_mtlFileName);
	_ASSERT_EXPR(fin, L"'MTL file not found.");

	material mat;

	wchar_t command[256];
	while (fin)
	{
		fin >> command;
		if (0 == wcscmp(command, L"map_Kd"))
		{
			wchar_t name[256];
			fin >> name;
			LoadTextureFile(name,&mat.shaderResourceView);
			materials.push_back(mat);
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"newmtl"))
		{
			fin >> mat.newmtl;
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"Ka"))
		{
			DirectX::XMFLOAT3 ka;
			fin >> ka.x >> ka.y >> ka.z;
			mat.Ka = ka;
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"Kd"))
		{
			DirectX::XMFLOAT3 kd;
			fin >> kd.x >> kd.y >> kd.z;
			mat.Kd = kd;
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"Ks"))
		{
			DirectX::XMFLOAT3 ks;
			fin >> ks.x >> ks.y >> ks.z;
			mat.Ks = ks;
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"illum"))
		{
			fin >> mat.illum;
			fin.ignore(1024, L'\n');
		}
//		else if (!fin)
//		{
//			materials.push_back(mat);
//		}
		else
		{
			fin.ignore(1024, L'\n');
		}
	}
}

void	StaticMesh::LoadTextureFile(wchar_t* _texFileName, ID3D11ShaderResourceView** _shaderResourceView)
{
	HRESULT hr = S_OK;
	ID3D11Device* device = FRAMEWORK->GetDevice();
	hr = load_texture_from_file(device, _texFileName, _shaderResourceView, &texture2d);
	assert(SUCCEEDED(hr) && "static_mesh.cpp Line 24");

	D3D11_SAMPLER_DESC smpDesc;

	ZeroMemory(&smpDesc, sizeof(D3D11_SAMPLER_DESC));
	smpDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	smpDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	smpDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	smpDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = device->CreateSamplerState(&smpDesc, &samplerState);
	assert(SUCCEEDED(hr) && "static_mesh.cpp Line 34");
}

void	StaticMesh::Render(ID3D11DeviceContext* _context, const DirectX::XMFLOAT4X4 &_WVP, const DirectX::XMFLOAT4X4 &_world, const DirectX::XMFLOAT4 &_lightDirection, DirectX::XMFLOAT4 &_materialColor, bool wireframe)
{
#if 1
	for (auto& material : materials)
	{
		cbuffer data;
		data.worldViewProjection = _WVP;
		data.world = _world;
		data.lightDirection = _lightDirection;
		data.materialColor.x = _materialColor.x*material.Kd.x;
		data.materialColor.y = _materialColor.y*material.Kd.y;
		data.materialColor.z = _materialColor.z*material.Kd.z;
		_context->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
		_context->VSSetConstantBuffers(0, 1, &constantBuffer);
		_context->PSSetShaderResources(0, 1, &material.shaderResourceView);
		_context->PSSetSamplers(0, 1, &samplerState);

		u_int stride = sizeof(vertex);
		u_int offset = 0;
		_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		_context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
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

		for (auto& subset : subsets)
		{
			if (material.newmtl == subset.usemtl)
			{
				_context->DrawIndexed(subset.index_count, subset.index_start, 0);
			}

		}
	}
#else
	cbuffer data;
	data.worldViewProjection = _WVP;
	data.world = _world;
	data.lightDirection = _lightDirection;
	data.materialColor = _materialColor;
	_context->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
	_context->VSSetConstantBuffers(0, 1, &constantBuffer);

	u_int stride = sizeof(vertex);
	u_int offset = 0;
	_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	_context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_context->IASetInputLayout(inputLayout);

	_context->PSSetShaderResources(0, 1, &shaderResourceView);
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
	indexBuffer->GetDesc(&buffer_desc);
	_context->DrawIndexed(buffer_desc.ByteWidth / sizeof(u_int), 0, 0);
#endif
}