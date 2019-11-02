#include"GeometricPrimitive.h"
#include"shader.h"

#ifdef USE_OLD_CLASS

geometric_primitive::geometric_primitive(ID3D11Device* _device)
{
	HRESULT hr = S_OK;

	//頂点シェーダーオブジェクトの生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		/*メンバー紹介*/
		//semanticName			: シェーダーのinput-signatureでこの要素に関連付けられているHLSLセマンティクス
		//semanticIndex			: 同じセマンティックを持つ要素が複数ある場合にのみ必要
		//Format				: DXGI_FORMATを参照
		//InputSlot				: 入力アンセブラを識別する整数値です。有効数字は0～15
		//AlignedByteoffset		: オプションです。便宜上、D3D111_APPEND_ALIGNED_ELEMENTを使用する
		//InputSlotClass		: 単一入力スロットの入力データクラスを識別します。
		//InstanceDataStepRate	: バッファ内を１要素進む前に同じインスタンスごとのデータを使用して描画するインスタンスの数
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	create_vs_from_cso(_device, "geometric_primitive_vs.cso", &vertexShader, &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));

	//ピクセルシェーダーオブジェクトの作成
	create_ps_from_cso(_device, "geometric_primitive_ps.cso", &pixelShader);

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
		hr = _device->CreateRasterizerState(&rasterizer_desc, &renderWire);
		assert(SUCCEEDED(hr) && "Wireのラスタライザステート作ってたら止まったで");
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
		hr = _device->CreateRasterizerState(&rasterizer_desc, &renderSolid);
		assert(SUCCEEDED(hr) && "Solidのラスタライザステート作ってたら止まったで");
	}

	//深度ステンシルステートに使う構造体の作成
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = TRUE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;

	//生成！
	hr = _device->CreateDepthStencilState(&depth_stencil_desc, &depthStencilState);
	assert(SUCCEEDED(hr) && "深度ステンシルステート作ってたら止まったで");

	//定数バッファオブジェクトの生成
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.ByteWidth = sizeof(cbuffer);
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;

		hr = _device->CreateBuffer(&buffer_desc, nullptr, &constantBuffer);
		assert(SUCCEEDED(hr) && "定数バッファ作ってたら止まったで");
	}

	vertex vertices[4 * 6] = {};			//一面あたり4頂点 × 立方体は六面　８頂点 × ３
	unsigned int indices[3 * 2 * 6] = {};	//描画する三角形の数 × 3

	int numV = 0, numI = 0;
	//自分で作ったほう
	{
		/*-----------------< 上面 >-----------------*/
		//頂点情報
		vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
		vertices[numV + 2].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
		vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
		//indexBuffer情報
		indices[numI + 0] = numV + 0;
		indices[numI + 1] = numV + 1;
		indices[numI + 2] = numV + 3;
		indices[numI + 3] = numV + 1;
		indices[numI + 4] = numV + 2;
		indices[numI + 5] = numV + 3;
		//法線は上向き
		vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);	

		numV += 4; numI += 6;


		/*-----------------< 下面 >-----------------*/
		//頂点情報
		vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
		vertices[numV + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
		vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
		//indexBuffer情報
		indices[numI + 0] = numV + 0;
		indices[numI + 1] = numV + 1;
		indices[numI + 2] = numV + 3;
		indices[numI + 3] = numV + 1;
		indices[numI + 4] = numV + 2;
		indices[numI + 5] = numV + 3;
		//法線は下向き
		vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

		numV += 4; numI += 6;

		//TODO : こいつから
		/*-----------------< 左面 >-----------------*/
		//頂点情報
		vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
		vertices[numV + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
		vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
		vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
		//indexBuffer情報
		indices[numI + 0] = numV + 0;
		indices[numI + 1] = numV + 1;
		indices[numI + 2] = numV + 3;
		indices[numI + 3] = numV + 1;
		indices[numI + 4] = numV + 2;
		indices[numI + 5] = numV + 3;
		//法線は左向き
		vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f);

		numV += 4; numI += 6;


		/*-----------------< 右面 >-----------------*/
		//頂点情報
		vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
		vertices[numV + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
		vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
		//indexBuffer情報
		indices[numI + 0] = numV + 0;
		indices[numI + 1] = numV + 1;
		indices[numI + 2] = numV + 3;
		indices[numI + 3] = numV + 1;
		indices[numI + 4] = numV + 2;
		indices[numI + 5] = numV + 3;
		//法線は右向き
		vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);

		numV += 4; numI += 6;


		/*-----------------< 前面 >-----------------*/
		//頂点情報
		vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
		vertices[numV + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
		vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
		//indexBuffer情報
		indices[numI + 0] = numV + 0;
		indices[numI + 1] = numV + 1;
		indices[numI + 2] = numV + 3;
		indices[numI + 3] = numV + 1;
		indices[numI + 4] = numV + 2;
		indices[numI + 5] = numV + 3;
		//法線は前向き
		vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);

		numV += 4; numI += 6;


		/*-----------------< 後面 >-----------------*/
		//頂点情報
		vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
		vertices[numV + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
		vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
		vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
		//indexBuffer情報
		indices[numI + 0] = numV + 0;
		indices[numI + 1] = numV + 1;
		indices[numI + 2] = numV + 3;
		indices[numI + 3] = numV + 1;
		indices[numI + 4] = numV + 2;
		indices[numI + 5] = numV + 3;
		//法線は後向き
		vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);

		numV += 4; numI += 6;
	}
	//先生のお手本
	{
		//	上面
//		vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
//		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
//		vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
//		vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
//		vertices[numV + 0].normal = vertices[numV + 1].normal =
//			vertices[numV + 2].normal =
//			vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +1.0f, +0.0f);
//		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
//		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
//		numV += 4;	numI += 6;
//
//		//	下面
//		vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
//		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
//		vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
//		vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
//		vertices[numV + 0].normal = vertices[numV + 1].normal =
//			vertices[numV + 2].normal =
//			vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, -1.0f, +0.0f);
//		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
//		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
//		numV += 4;	numI += 6;
//
//		//	右面
//		vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
//		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
//		vertices[numV + 2].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
//		vertices[numV + 3].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
//		vertices[numV + 0].normal = vertices[numV + 1].normal =
//			vertices[numV + 2].normal =
//			vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, +0.0f, +0.0f);
//		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
//		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
//		numV += 4;	numI += 6;
//
//		//	左面
//		vertices[numV + 0].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
//		vertices[numV + 1].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
//		vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
//		vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
//		vertices[numV + 0].normal = vertices[numV + 1].normal =
//			vertices[numV + 2].normal =
//			vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, +0.0f, +0.0f);
//		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
//		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
//		numV += 4;	numI += 6;
//
//		//	後面
//		vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
//		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
//		vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
//		vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
//		vertices[numV + 0].normal = vertices[numV + 1].normal =
//			vertices[numV + 2].normal =
//			vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, +1.0f);
//		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 1;	indices[numI + 2] = numV + 2;
//		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 3;	indices[numI + 5] = numV + 2;
//		numV += 4;	numI += 6;
//
//		//	前面
//		vertices[numV + 0].position = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
//		vertices[numV + 1].position = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
//		vertices[numV + 2].position = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
//		vertices[numV + 3].position = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
//		vertices[numV + 0].normal = vertices[numV + 1].normal =
//			vertices[numV + 2].normal =
//			vertices[numV + 3].normal = DirectX::XMFLOAT3(+0.0f, +0.0f, -1.0f);
//		indices[numI + 0] = numV + 0;	indices[numI + 1] = numV + 2;	indices[numI + 2] = numV + 1;
//		indices[numI + 3] = numV + 1;	indices[numI + 4] = numV + 2;	indices[numI + 5] = numV + 3;
//		numV += 4;	numI += 6;
	}
	/***************************************************************************************************/
	CreateBuffers(_device, vertices, numV, indices, numI);
}

void	geometric_primitive::CreateBuffers(ID3D11Device* _device, vertex* _vertices, int _numVertices, u_int* _indices, int _numIndices)
{
	HRESULT hr;

	//バーテックスバッファの生成
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(vertex)*_numVertices;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		//buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = _vertices;
		subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
		subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

		hr = _device->CreateBuffer(&buffer_desc, &subresource_data, &vertexBuffer);
		assert(SUCCEEDED(hr) && "vertexバッファ作ってたら止まったで");
	}

	//インデックスバッファの生成
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(u_int)*_numIndices;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		//buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = _indices;
		subresource_data.SysMemPitch = 0; //Not use for index buffers.
		subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
		hr = _device->CreateBuffer(&buffer_desc, &subresource_data, &indexBuffer);
		assert(SUCCEEDED(hr) && "indexバッファ作ってたら止まったで");
	}
}

void	geometric_primitive::Render(ID3D11DeviceContext* _deviceContext, const DirectX::XMFLOAT4X4 & _wvp, const DirectX::XMFLOAT4X4 & _world, const DirectX::XMFLOAT4 & _color, const DirectX::XMFLOAT4 & _light,bool bSolid)
{
	cbuffer cb;
	cb.world_view_projection = _wvp;
	cb.world = _world;
	cb.light_direction = _light;
	cb.material_color = _color;
	//定数バッファのバインド
	_deviceContext->UpdateSubresource(constantBuffer, 0, 0, &cb, 0, 0);
	_deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer);

	UINT stride = sizeof(vertex);
	UINT offset = 0;

	//頂点バッファのバインド
	_deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//インデックスバッファのバインド
	_deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	//プリミティブトポロジーの設定
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//入力レイアウトオブジェクトのバインド
	_deviceContext->IASetInputLayout(
		inputLayout		//ID3D11InputLayoutのポインタ
		);

	//シェーダの設定
	//VertexShader
	_deviceContext->VSSetShader(
		vertexShader,	//頂点シェーダへのポインタ
		nullptr,		//クラスインスタンスインターフェースの配列へのポインタ。シェーダがインターフェースを使用しない場合はNULL
		0				//配列内のクラスインスタンスインターフェースの数
		);
	//PixelShader
	_deviceContext->PSSetShader(
		pixelShader,	//ピクセルシェーダへのポインタ
		nullptr,		//クラスインスタンスインターフェースの配列へのポインタ。シェーダがインターフェースを使用しない場合はNULL
		0				//配列内のクラスインスタンスインターフェースの数
		);

	//深度ステンシルステートの設定
	_deviceContext->OMSetDepthStencilState(
		depthStencilState,
		1
		);

	//ラスタライザーステートの設定
	if (bSolid)
	{
		_deviceContext->RSSetState(
			renderSolid		//パイプラインにバインドするためのラスタライザ状態インタフェースへのポインタ
			);
	}
	else
	{
		_deviceContext->RSSetState(
			renderWire		//パイプラインにバインドするためのラスタライザ状態インタフェースへのポインタ
			);
	}

	//インデックス付けされているプリミティブの描画
//	_deviceContext->DrawIndexed(6, 0, 0);
	D3D11_BUFFER_DESC buffer_desc;
	indexBuffer->GetDesc(&buffer_desc);
	_deviceContext->DrawIndexed(buffer_desc.ByteWidth / sizeof(int), 0, 0);


}

#else

void GeometricPrimitive::Init(ID3D11Device* _device, 
	const char* _vsname, D3D11_INPUT_ELEMENT_DESC* _inputElements, int _numElement, 
	const char* psName)
{
	HRESULT hr;

	// 頂点シェーダー
	create_vs_from_cso(_device, _vsname, &vertexShader, &inputLayout, _inputElements, _numElement);

	// ピクセルシェーダー
	create_ps_from_cso(_device, psName, &pixelShader);

	// ラスタライザーステート（ワイヤーフレーム）の作成
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.AntialiasedLineEnable = TRUE;
	hr = _device->CreateRasterizerState(&rsDesc, &wireframeRasterizerState);
	assert(SUCCEEDED(hr) && "GeometricPrimitive.cpp Line:417");

	// ラスタライザーステート（ソリッド）の作成
	//D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(rsDesc));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	hr = _device->CreateRasterizerState(&rsDesc, &solidRasterizerState);
	assert(SUCCEEDED(hr) && "GeometricPrimitive.cpp Line:427");

	// 深度ステンシルステートの作成
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(dssDesc));
	dssDesc.DepthEnable = TRUE;						//	深度テスト有効
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dssDesc.StencilEnable = FALSE;					//	ステンシルは無効
	hr = _device->CreateDepthStencilState(&dssDesc, &depthStencilState);
	assert(SUCCEEDED(hr) && "GeometricPrimitive.cpp Line:437");
}


bool	GeometricPrimitive::CreateBuffers(ID3D11Device* _device, Vertex* _vertices, int _numV, unsigned int* _indices, int _numI)
{
	HRESULT hr;
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA srd;

	// Vertex Buffer
	{
		ZeroMemory(&bd, sizeof(bd));
		ZeroMemory(&srd, sizeof(srd));

		bd.ByteWidth = _numV * sizeof(Vertex);
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		srd.pSysMem = _vertices;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;

		hr = _device->CreateBuffer(&bd, &srd, &vertexBuffer);
		if (FAILED(hr)) return false;
	}
	// Index Buffer
	{
		ZeroMemory(&bd, sizeof(bd));
		ZeroMemory(&srd, sizeof(srd));

		bd.ByteWidth = _numI * sizeof(unsigned int);
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		srd.pSysMem = _indices;
		srd.SysMemPitch = 0;
		srd.SysMemSlicePitch = 0;

		numIndices = _numI;
		hr = _device->CreateBuffer(&bd, &srd, &indexBuffer);
		if (FAILED(hr))	return false;
	}
	// Constant Buffer
	{
		ZeroMemory(&bd, sizeof(bd));

		bd.ByteWidth = sizeof(Cbuffer);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		hr = _device->CreateBuffer(&bd, nullptr, &constantBuffer);
		if (FAILED(hr)) return false;

	}

	isMakeBuffer = true;
	return true;
}

GeometricPrimitive::~GeometricPrimitive()
{
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = nullptr;
	}
	if (constantBuffer)
	{
		constantBuffer->Release();
		constantBuffer = nullptr;
	}
	if (depthStencilState)
	{
		depthStencilState->Release();
		depthStencilState = nullptr;
	}
	if (solidRasterizerState)
	{
		solidRasterizerState->Release();
		solidRasterizerState = nullptr;
	}
	if (wireframeRasterizerState)
	{
		wireframeRasterizerState->Release();
		wireframeRasterizerState = nullptr;
	}
	if (vertexShader)
	{
		vertexShader->Release();
		vertexShader = nullptr;
	}
	if (pixelShader)
	{
		pixelShader->Release();
		pixelShader = nullptr;
	}
	if (inputLayout)
	{
		inputLayout->Release();
		inputLayout = nullptr;
	}
}


void GeometricPrimitive::Render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& wvp,
	const DirectX::XMFLOAT4X4& world,
	const DirectX::XMFLOAT4& lightDirection,
	const DirectX::XMFLOAT4& materialColor,
	bool bSolid
)
{
	// バッファが作られていなかったら止める
	assert(isMakeBuffer);

	// 定数バッファの作成
	Cbuffer cb;
	cb.wvp = wvp;
	cb.world = world;
	cb.lightDirection = lightDirection;
	cb.materialColor = materialColor;
	context->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);
	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	// 頂点バッファのバインド
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// インデックスバッファのバインド
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// プリミティブモードの設定
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 入力レイアウトのバインド
	context->IASetInputLayout( inputLayout );

	// ラスタライザの設定
	if (bSolid)	context->RSSetState(solidRasterizerState);
	else		context->RSSetState(wireframeRasterizerState);

	// シェーダーの設定
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	// 深度テストの設定
	context->OMSetDepthStencilState(depthStencilState, 0);

	// 描画
	context->DrawIndexed(numIndices, 0, 0);
}


GeometricRect::GeometricRect(ID3D11Device* _device, bool _isUseTexture)
{
	Vertex vertices[4 * 6] = {};			//一面あたり4頂点 × 立方体は六面　８頂点 × ３
	unsigned int indices[3 * 2 * 6] = {};	//描画する三角形の数 × 3

	int numV = 0, numI = 0;
	//自分で作ったほう
	{
		/*-----------------< 上面 >-----------------*/
		{
			//頂点情報
			vertices[numV + 0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
			vertices[numV + 1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
			vertices[numV + 2].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
			vertices[numV + 3].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
			//indexBuffer情報
			indices[numI + 0] = numV + 0;
			indices[numI + 1] = numV + 1;
			indices[numI + 2] = numV + 3;
			indices[numI + 3] = numV + 1;
			indices[numI + 4] = numV + 2;
			indices[numI + 5] = numV + 3;
			//法線は上向き
			vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, +1.0f, 0.0f);

			numV += 4; numI += 6;

		}
		/*-----------------< 下面 >-----------------*/
		{
			//頂点情報
			vertices[numV + 0].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
			vertices[numV + 1].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
			vertices[numV + 2].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
			vertices[numV + 3].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
			//indexBuffer情報
			indices[numI + 0] = numV + 0;
			indices[numI + 1] = numV + 1;
			indices[numI + 2] = numV + 3;
			indices[numI + 3] = numV + 1;
			indices[numI + 4] = numV + 2;
			indices[numI + 5] = numV + 3;
			//法線は下向き
			vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);

			numV += 4; numI += 6;
		}
		/*-----------------< 左面 >-----------------*/
		{
			//頂点情報
			vertices[numV + 0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
			vertices[numV + 1].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
			vertices[numV + 2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
			vertices[numV + 3].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
			//indexBuffer情報
			indices[numI + 0] = numV + 0;
			indices[numI + 1] = numV + 1;
			indices[numI + 2] = numV + 3;
			indices[numI + 3] = numV + 1;
			indices[numI + 4] = numV + 2;
			indices[numI + 5] = numV + 3;
			//法線は左向き
			vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f);

			numV += 4; numI += 6;

		}
		/*-----------------< 右面 >-----------------*/
		{
			//頂点情報
			vertices[numV + 0].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
			vertices[numV + 1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
			vertices[numV + 2].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
			vertices[numV + 3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
			//indexBuffer情報
			indices[numI + 0] = numV + 0;
			indices[numI + 1] = numV + 1;
			indices[numI + 2] = numV + 3;
			indices[numI + 3] = numV + 1;
			indices[numI + 4] = numV + 2;
			indices[numI + 5] = numV + 3;
			//法線は右向き
			vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(+1.0f, 0.0f, 0.0f);

			numV += 4; numI += 6;

		}
		/*-----------------< 前面 >-----------------*/
		{
			//頂点情報
			vertices[numV + 0].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, -0.5f);
			vertices[numV + 1].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, -0.5f);
			vertices[numV + 2].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, -0.5f);
			vertices[numV + 3].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f);
			//indexBuffer情報
			indices[numI + 0] = numV + 0;
			indices[numI + 1] = numV + 1;
			indices[numI + 2] = numV + 3;
			indices[numI + 3] = numV + 1;
			indices[numI + 4] = numV + 2;
			indices[numI + 5] = numV + 3;
			//法線は前向き
			vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, +1.0f);

			numV += 4; numI += 6;

		}
		/*-----------------< 後面 >-----------------*/
		{
			//頂点情報
			vertices[numV + 0].pos = DirectX::XMFLOAT3(+0.5f, +0.5f, +0.5f);
			vertices[numV + 1].pos = DirectX::XMFLOAT3(-0.5f, +0.5f, +0.5f);
			vertices[numV + 2].pos = DirectX::XMFLOAT3(-0.5f, -0.5f, +0.5f);
			vertices[numV + 3].pos = DirectX::XMFLOAT3(+0.5f, -0.5f, +0.5f);
			//indexBuffer情報
			indices[numI + 0] = numV + 0;
			indices[numI + 1] = numV + 1;
			indices[numI + 2] = numV + 3;
			indices[numI + 3] = numV + 1;
			indices[numI + 4] = numV + 2;
			indices[numI + 5] = numV + 3;
			//法線は後向き
			vertices[numV + 0].normal = vertices[numV + 1].normal = vertices[numV + 2].normal = vertices[numV + 3].normal = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);

			numV += 4; numI += 6;
		}
	}

	CreateBuffers(_device, vertices, numV, indices, numI);
}

#endif