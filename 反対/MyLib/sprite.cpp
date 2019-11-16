#include <stdio.h>
#include <memory>
#include <map>
#include "sprite.h"
#include "shader.h"
#include "texture.h"
#include "WICTextureLoader.h"
#include "framework.h"

/**********************************************************************************************************************

Sprite クラス

**********************************************************************************************************************/
//spriteクラスのコンストラクタの実装
Sprite::Sprite(ID3D11Device* orgDevice, const wchar_t* texture)
{
	//①頂点情報のセット
	vertex virtices[] = {
		{ DirectX::XMFLOAT3(-0.5,+0.5,0),DirectX::XMFLOAT4(1,1,1,1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(+0.5,+0.5,0),DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(-0.5,-0.5,0),DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(+0.5,-0.5,0),DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT2(0, 0) },
	};

	//②頂点バッファオブジェクトの生成
	//TODO : 3-2 spriteクラスのコンストラクタで頂点バッファオブジェクトの生成方法を変更する
	//GPU（読み取りのみ）とCPU（書き込みのみ）によるアクセスを可能にする
	//バッファリソースを記述する
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(virtices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//書き込みのみアクセスを可能にする
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	//複資源を初期設定するためのデータを指定します
	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = virtices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;
	
	//バッファ（頂点バッファ、インデックスバッファ、またはシェーダ定数バッファ）を作成します
	HRESULT hr = S_OK;
	hr = orgDevice->CreateBuffer(&bufferDesc, &subResourceData, &vertexBuffer);
	if (FAILED(hr))
		assert(hr);

	//③頂点シェーダーオブジェクトの生成
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//CSOファイルをロードする

	//⑤バーテックスシェーダオブジェクトの生成
	//sprite_vsをロードする
	create_vs_from_cso(orgDevice, "Data/Shaders/sprite_vs.cso", &vertexShader, &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));
	//⑤ピクセルシェーダオブジェクトの生成
	//sprite_psをロード
	create_ps_from_cso(orgDevice, "Data/Shaders/sprite_ps.cso", &pixelShader);
	//ロード終了

	//⑥ラスタライザーステートの設定
	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;	//レンダリング時に使用する塗りつぶしモードを決定する
	rasterizer_desc.CullMode = D3D11_CULL_NONE;		//指定された方向を向いている三角形が描かれていないことを示します
	rasterizer_desc.FrontCounterClockwise = FALSE;	//三角形が正面を向いているか背面を向いているかを決定します。(TRUEの場合、三角形はその頂点がレンダリングターゲット上で反時計回りにある場合は正面を向いているとみなされる)
	rasterizer_desc.DepthBias = 0;					//特定のピクセルに追加された深度値
	rasterizer_desc.DepthBiasClamp = 0;				//ピクセルの最大深度バイアス
	rasterizer_desc.SlopeScaledDepthBias = 0;		//与えられたピクセルの傾きのスカラー
	rasterizer_desc.DepthClipEnable = FALSE;		//距離に基づいてクリッピングを有効にします
	rasterizer_desc.ScissorEnable = FALSE;			//シザー長方形カリングを有効にします
	rasterizer_desc.MultisampleEnable = FALSE;		//MSAAレンダーターゲットで四辺形またはα先のアンチエイリアシングアルゴリズムを使用するかどうかを指定します
	rasterizer_desc.AntialiasedLineEnable = FALSE;	//ラインアンチエイリアシングを有効にするかどうかを指定します

	//CreateRasterizerState methot
	hr = orgDevice->CreateRasterizerState(&rasterizer_desc, &rasterizerState);
	if (FAILED(hr))
		assert(hr);

	//テクスチャのロード
	//画像の読み込み
	hr = load_texture_from_file(orgDevice, texture, &shaderResourceView, &texture2d_desc);

	//サンプラーステートオブジェクトの生成(ID3D11SamplerState)の生成
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;																//テクスチャをサンプリングするときに使用するフィルタリング方法
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;														//0から1の範囲外のauテクスチャ座標を解決するために使用するメソッド
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;														//0から1の範囲外のテクスチャ座標を解決するために使用するメソッド
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;														//0から1の範囲外のWテクスチャ座標を解決するために使用するメソッド
	sampler_desc.MipLODBias = 0;																				//計算されたミップマップレベルからのオフセット
	sampler_desc.MaxAnisotropy = 16;																			//D3D11_FILTER_ANISOTROPICまたはD3D11_FILTER_COMPARISON_ANISOTROPICがフィルタで指定されている場合に使用されるクランプ値 有効値は1～16
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;														//サンプリングデータを既存のサンプリングデータと比較する関数
	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));	//AddressU,AddressV,AddressWにD3D11_TEXTURE_ADDRESS_BORDERが指定されている場合に使用する枠の色。範囲は0.0以上1.0以下
	sampler_desc.MinLOD = 0;																					//アクセスを制限するミップマップ範囲の下限
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;																	//アクセスを制限するミップマップ範囲の上限
	//CreateSamplerState methot
	hr = orgDevice->CreateSamplerState(&sampler_desc, &samplerState);
	if (FAILED(hr))
		assert(hr);

	//深度ステンシルステートに使う構造体の作成
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//生成！
	hr = orgDevice->CreateDepthStencilState(&depth_stencil_desc, &depthStencilState);
	if (FAILED(hr))
		assert(hr);
}

//TODO6 : Spriteクラスの render メンバ関数の実装を変更する
void Sprite::Render(ID3D11DeviceContext* orgDeviceContext, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a)
{
	// dx, dy	: Coordinate of sprite's left-top corner in screen space 
	// dw, dh	: Size of sprite in screen space 
	// sx,sy	: Coordinate of sprite's left-top corner in texture space
	// sw,sh	: Size of sprite in texture space
	// angle	: Raotation angle (Rotation centre is sprite's centre), unit is degree
	// r, g, b,a: Color of sprite's each vertices 

	D3D11_VIEWPORT viewport;
	UINT numViewport = 1;
	orgDeviceContext->RSGetViewports(&numViewport, &viewport);
	
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	float lt_x = dx;		//左上
	float lt_y = dy;

	float rt_x = dx + dw;	//右上
	float rt_y = dy;

	float lb_x = dx;		//左下
	float lb_y = dy + dh;

	float rb_x = dx + dw;	//右下
	float rb_y = dy + dh;


	float mx = dx + dw*0.5f;
	float my = dy + dh*0.5f;
	lt_x -= mx;
	lt_y -= my;
	rt_x -= mx;
	rt_y -= my;
	lb_x -= mx;
	lb_y -= my;
	rb_x -= mx;
	rb_y -= my;

	//回転する
	float sin = sinf(angle*0.01745f);
	float cos = cosf(angle*0.01745f);
	float rx, ry;
	
	rx = lt_x;
	ry = lt_y;
	lt_x = cos*rx + -sin*ry;
	lt_y = sin*rx + cos*ry;
	rx = rt_x;
	ry = rt_y;
	rt_x = cos*rx + -sin*ry;
	rt_y = sin*rx + cos*ry;
	rx = lb_x;
	ry = lb_y;
	lb_x = cos*rx + -sin*ry;
	lb_y = sin*rx + cos*ry;
	rx = rb_x;
	ry = rb_y;
	rb_x = cos*rx + -sin*ry;
	rb_y = sin*rx + cos*ry;

	lt_x += mx;
	lt_y += my;
	rt_x += mx;
	rt_y += my;
	lb_x += mx;
	lb_y += my;
	rb_x += mx;
	rb_y += my;

	//NDC座標変換
	float ndc_lt_x = (lt_x / screen_width) * 2 - 1;
	float ndc_lt_y = 1 - ((lt_y / screen_height) * 2);

	float ndc_rt_x = (rt_x / screen_width) * 2 - 1;
	float ndc_rt_y = 1 - ((rt_y / screen_height) * 2);

	float ndc_lb_x = (lb_x / screen_width) * 2 - 1;
	float ndc_lb_y = 1 - ((lb_y / screen_height) * 2);

	float ndc_rb_x = (rb_x / screen_width) * 2 - 1;
	float ndc_rb_y = 1 - ((rb_y / screen_height) * 2);

	//計算結果で頂点バッファオブジェクトを更新する
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;

	HRESULT hr = S_OK;
	hr = orgDeviceContext->Map(
		vertexBuffer,	//ID3D11Resourceへのポインタ
		0,				//副リソースのインデックス番号
		map,			//リソースに対するCPUの読み取りおよび書き込み権限を指定するD3D11_MAPタイプの値
		0,				//GPUがビジーのときにCPUが何をするかを指定するフラグ
		&mapped_buffer	//マップされたサブリソースのD3D11_MAPPED_SUBRESOURCE構造体へのポインタ
		);
	if (FAILED(hr))
		return;

	//verticesに入れる
	vertex* vertices= static_cast<vertex *>(mapped_buffer.pData);
	vertices[0].position.x = ndc_lt_x;
	vertices[0].position.y = ndc_lt_y;
	vertices[1].position.x = ndc_rt_x;
	vertices[1].position.y = ndc_rt_y;
	vertices[2].position.x = ndc_lb_x;
	vertices[2].position.y = ndc_lb_y;
	vertices[3].position.x = ndc_rb_x;
	vertices[3].position.y = ndc_rb_y;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

	//テクスチャ座標の計算と頂点バッファの更新
	vertices[0].texcoord.x = static_cast<FLOAT>(sx) / texture2d_desc.Width;
	vertices[0].texcoord.y = static_cast<FLOAT>(sy) / texture2d_desc.Height;
	vertices[1].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2d_desc.Width;
	vertices[1].texcoord.y = static_cast<FLOAT>(sy) / texture2d_desc.Height;
	vertices[2].texcoord.x = static_cast<FLOAT>(sx) / texture2d_desc.Width;
	vertices[2].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2d_desc.Height;
	vertices[3].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2d_desc.Width;
	vertices[3].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2d_desc.Height;

	orgDeviceContext->Unmap(vertexBuffer, 0);


	UINT stride = sizeof(vertex);
	UINT offset = 0;
	//①頂点バッファーのバインド
	orgDeviceContext->IASetVertexBuffers(
		0,				//バインディグ用の最初の入力スロット
		1,				//配列内の頂点バッファの数
		&vertexBuffer,	//頂点バッファの配列へのポインタ
		&stride,		//ストライド値の配列のポインタ
		&offset			//オフセット値へのポインタ
		);

	//②プリミティブタイプ及びデータの順序に関する情報のバインド
	orgDeviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	//プリミティブの種類とプリミティブデータの順序
		);

	//③入力レイアウトオブジェクトのバインド
	orgDeviceContext->IASetInputLayout(
		inputLayout		//ID3D11InputLayoutのポインタ
		);

	//④ラスタライザーステートの設定
	orgDeviceContext->RSSetState(
		rasterizerState	//パイプラインにバインドするためのラスタライザ状態インタフェースへのポインタ
		);

	//⑤シェーダの設定
	//VertexShader
	orgDeviceContext->VSSetShader(
		vertexShader,	//頂点シェーダへのポインタ
		nullptr,		//クラスインスタンスインターフェースの配列へのポインタ。シェーダがインターフェースを使用しない場合はNULL
		0				//配列内のクラスインスタンスインターフェースの数
		);
	//PixelShader
	orgDeviceContext->PSSetShader(
		pixelShader,	//ピクセルシェーダへのポインタ
		nullptr,		//クラスインスタンスインターフェースの配列へのポインタ。シェーダがインターフェースを使用しない場合はNULL
		0				//配列内のクラスインスタンスインターフェースの数
		);

	//シェーダーリソースのバインド
	orgDeviceContext->PSSetShaderResources(
		0,					//デバイスのゼロベースの配列にインデックスを付けて、シェーダリソースの設定を開始します（0～D3D11_COMMONSHADER_INPUT_RESOURCE_SHOT_COUNT -1 の範囲）
		1,					//設定するシェーダリソースには最大128スロットまで使用できます(0～D3D11_COMMONSHADER_INPUT_RESOURCE_SHOT_COUNT -StartShot の範囲)
		&shaderResourceView	//デバイスに設定するシェーダリソースビューインターフェイスの配列
		);

	//サンプラーステートの設定
	orgDeviceContext->PSSetSamplers(
		0,					//デバイスのゼロベースの配列にインデックスを付けて、サンプラの設定を開始します（0～D3D11_COMMONSHADER_INPUT_RESOURCE_SHOT_COUNT -1 の範囲）
		1,					//配列内のサンプラ数 各パイプラインステージには、合計16個のサンプラースロットがあります(0～D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT -StartSlotの範囲)
		&samplerState		//サンプラ状態インターフェースの配列へのポインタ
		);

	//深度ステンシルステートの設定
	orgDeviceContext->OMSetDepthStencilState(
		depthStencilState,
		1
		);

	//⑥プリミティブの描画
	orgDeviceContext->Draw(
		4,	//描画する頂点の数
		0	//最初の頂点のインデックス
		);
}


void Sprite::textout(ID3D11DeviceContext *_deviceContext, std::string _s, float _x, float _y, float _w, float _h, float _r, float _g, float _b, float _a)
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float cursor = 0;
	for (auto c : _s)
	{
		LONG sx = c % 0x0F;
		Render(_deviceContext, _x + cursor, _y, _w, _h, sw*(c & 0x0F), sh*(c >> 4), sw, sh, 0, _r, _g, _b, _a);
		cursor += _w;
	}
}



/**********************************************************************************************************************

	SpriteBatch クラス

**********************************************************************************************************************/

/*--------------------------------*/
//	コンストラクタ
/*--------------------------------*/
SpriteBatch::SpriteBatch(const wchar_t* _fileName, size_t _maxInstance, DirectX::XMFLOAT2 _cutOutSize)
{
	HRESULT hr = S_OK;

	MAX_INSTANCES = _maxInstance;

	ID3D11Device* device = FRAMEWORK->GetDevice();

	//vertexBufferの作成
	vertex vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
		{ DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
		{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	device->CreateBuffer(&bufferDesc, &subresourceData, &buffer);
	assert(SUCCEEDED(hr) && "VertexBuffer作ってたら止まったで by SpriteBatch");


	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElements = ARRAYSIZE(layout);

	//シェーダーのロード
	hr = create_vs_from_cso(device, "Data/Shaders/sprite_batch_vs.cso", &vertexShader, &inputLayout, layout, ARRAYSIZE(layout));
	assert(SUCCEEDED(hr) && "Data/Shaders/vertexShagerの読み込み中にこけました　by SpriteBatch");
	hr = create_ps_from_cso(device, "Data/Shaders/sprite_batch_ps.cso", &pixelShader);
	assert(SUCCEEDED(hr) && "Data/Shaders/pixelShaderの読み込み中にこけました　by SpriteBatch");

	instance* inst = new instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		bd.ByteWidth = sizeof(instance) * MAX_INSTANCES;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		sd.pSysMem = inst;
		sd.SysMemPitch = 0;
		sd.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bd, &sd, &instanceBuffer);
		assert(SUCCEEDED(hr) && "バッファの生成に失敗した by SpriteBatch");
	}
	delete[] inst;

	//ラスタライザステートの生成
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	assert(SUCCEEDED(hr) && "ラスタライザステートの生成でこけた by SpriteBatch");

	//テクスチャ読み込み
	hr = load_texture_from_file(device, _fileName, &shaderResourceView, &tex2dDesc);
	assert(SUCCEEDED(hr) && "テクスチャ読み込みに失敗しやした by SpriteBatch");

	//サンプラーステートの生成
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	for (int i = 0; i < 4; i++) samplerDesc.BorderColor[i] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, &samplerState);
	assert(SUCCEEDED(hr) && "サンプラーステート作成でこけた by SpriteBatch");

	//深度ステンシルステートの生成
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = FALSE;;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&dsDesc, &depthStencilState);
	assert(SUCCEEDED(hr) && "深度ステンシルステートの作成でこけた by SpriteBatch");

	//Assign a orgments to a variable
	//However,	if「_cutOutSizeX <= 0」or「_cutOutSizeY <= 0」,cutOutSize is natual texture size.
	if (_cutOutSize.x <= 0 || _cutOutSize.y <= 0)
	{
		_cutOutSize.x = static_cast<float>(tex2dDesc.Width);
		_cutOutSize.y = static_cast<float>(tex2dDesc.Height);
	}
	cutOutSize.x = _cutOutSize.x;
	cutOutSize.y = _cutOutSize.y;
	;
	float tentativeTexSizeX = static_cast<float>(tex2dDesc.Width);
	float tentativeTexSizeY = static_cast<float>(tex2dDesc.Height);
	int cutNumX = static_cast<int>(tentativeTexSizeX / cutOutSize.x);
	int cutNumY = static_cast<int>(tentativeTexSizeY / cutOutSize.y);

	int count = 0;
	for (int i = 0; i<cutNumY; i++)
	{
		for (int j = 0; j<cutNumX; j++)
		{
			adress.insert(std::make_pair(count++, DirectX::XMFLOAT2(j*cutOutSize.x, i*cutOutSize.y)));
		}
	}
}

/*--------------------------------*/
//	デストラクタ
/*--------------------------------*/
SpriteBatch::~SpriteBatch()
{
	if (vertexShader)
		vertexShader->Release();
	if (pixelShader)
		pixelShader->Release();
	if (inputLayout)
		inputLayout->Release();
	if (rasterizerState)
		rasterizerState->Release();
	if (shaderResourceView)
		shaderResourceView->Release();
	if (samplerState)
		samplerState->Release();
	if (depthStencilState)
		depthStencilState->Release();
	if (buffer)
		buffer->Release();
	if (instanceBuffer)
		instanceBuffer->Release();
	vertexShader = nullptr;
	pixelShader = nullptr;
	inputLayout = nullptr;
	rasterizerState = nullptr;
	shaderResourceView = nullptr;
	samplerState = nullptr;
	depthStencilState = nullptr;
	buffer = nullptr;
	instanceBuffer = nullptr;
}

/*--------------------------------*/
//	描画前処理
/*--------------------------------*/
void SpriteBatch::Begin()
{
	HRESULT hr = S_OK;
	ID3D11DeviceContext* deviceContext = FRAMEWORK->GetDeviceContext();

	UINT strides[2] = { sizeof(vertex),sizeof(instance) };
	UINT offsets[2] = { 0,0 };
	ID3D11Buffer* vbs[2] = { buffer,instanceBuffer };
	deviceContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);
	deviceContext->RSSetState(rasterizerState);
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);
	deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	UINT numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;

	hr = deviceContext->Map(instanceBuffer, 0, map, 0, &mappedBuffer);
	assert(SUCCEEDED(hr) && "描画前のMapで引っかかったで by SpriteBatch");

	instances = static_cast<instance *>(mappedBuffer.pData);

	instanceCount = 0;
}


/*--------------------------------*/
//	描画処理( XMFLOAT版 )
/*--------------------------------*/
void	SpriteBatch::Draw(
	DirectX::XMFLOAT2& position, DirectX::XMFLOAT2& scale,
	DirectX::XMFLOAT2& texPos, DirectX::XMFLOAT2& texSize,
	DirectX::XMFLOAT2& center, float angle,
	DirectX::XMFLOAT4& color
	)
{
	//Begin
//	if (instanceCount == 0)
//	{
//		Begin();
//	}

	if (instanceCount >= MAX_INSTANCES)
	{
		assert("instanceがMAX_INSTANCESを超えてまっせ");
		return;
	}
	if (scale.x*scale.y == 0.0f)return;

	float tw = texSize.x;
	float th = texSize.y;
	if (texSize.x <= 0.0f || texSize.y <= 0.0f)
	{
		tw = static_cast<float>(tex2dDesc.Width);
		th = static_cast<float>(tex2dDesc.Height);
	}

	float cx = center.x;
	float cy = center.y;
	cx *= scale.x;
	cy *= scale.y;

	FLOAT c = cosf(angle*0.01745f);
	FLOAT s = sinf(angle*0.01745f);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;

	instances[instanceCount].ndcTransform._11 = w*scale.x*c;
	instances[instanceCount].ndcTransform._21 = h*scale.x*s;
	instances[instanceCount].ndcTransform._31 = 0.0f;
	instances[instanceCount].ndcTransform._41 = 0.0f;
	instances[instanceCount].ndcTransform._12 = w*scale.y*-s;
	instances[instanceCount].ndcTransform._22 = h*scale.y*c;
	instances[instanceCount].ndcTransform._32 = 0.0f;
	instances[instanceCount].ndcTransform._42 = 0.0f;
	instances[instanceCount].ndcTransform._13 = 0.0f;
	instances[instanceCount].ndcTransform._23 = 0.0f;
	instances[instanceCount].ndcTransform._33 = 1.0f;
	instances[instanceCount].ndcTransform._43 = 0.0f;
	instances[instanceCount].ndcTransform._14 = w*(-cx*c + -cy*-s + cx + position.x) - 1.0f;
	instances[instanceCount].ndcTransform._24 = h*(-cx*s + -cy*c + cy + position.y) + 1.0f;
	instances[instanceCount].ndcTransform._34 = 0.0f;
	instances[instanceCount].ndcTransform._44 = 1.0f;

	float tex_width = static_cast<float>(tex2dDesc.Width);
	float tex_height = static_cast<float>(tex2dDesc.Height);
	instances[instanceCount].texcoordTransform = DirectX::XMFLOAT4(texPos.x / tex_width, texPos.y / tex_height, tw / tex_width, th / tex_height);
	instances[instanceCount].color = color;

	++instanceCount;

	//End
//	if (instanceCount >= MAX_INSTANCES)
//	{
//		End();
//	}
}


/*--------------------------------*/
//	描画処理( float版 )
/*--------------------------------*/
void	SpriteBatch::Draw(
	float x,	float y,	float sx,	float sy,
	float tx,	float ty,	float tw,	float th,
	float cx,	float cy,	float angle/*radian*/,
	float r,	float g,	float b,	float a
	)
{
	Draw(
		DirectX::XMFLOAT2(x, y),	DirectX::XMFLOAT2(sx, sy),
		DirectX::XMFLOAT2(tx, ty),	DirectX::XMFLOAT2(tw, th),
		DirectX::XMFLOAT2(cx, cy),	angle, 
		DirectX::XMFLOAT4(r, g, b, a)
		);
}

/*--------------------------------*/
//	描画処理( アニメーションを考慮する感じのやつ版 )
/*--------------------------------*/
void	SpriteBatch::Draw(
	float x, float y, float sx, float sy,
	float angle,/*degree*/
	float r, float g, float b, float a,
	int cutOutAdress
	)
{
	DirectX::XMFLOAT2 texPos = adress.at(cutOutAdress);
	
	Draw(
		DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(sx, sy),
		texPos, cutOutSize,
		DirectX::XMFLOAT2((texPos.x + cutOutSize.x) / 2, (texPos.y + cutOutSize.y) / 2),
		angle,
		DirectX::XMFLOAT4(r, g, b, a)
		);
}

/*--------------------------------*/
//	描画後処理
/*--------------------------------*/
void SpriteBatch::End()
{
	ID3D11DeviceContext* deviceContext = FRAMEWORK->GetDeviceContext();
	deviceContext->Unmap(instanceBuffer, 0);
	deviceContext->DrawInstanced(4, instanceCount, 0, 0);
//	instanceCount = 0;
}


/**********************************************************************************************************************

	各種関数

**********************************************************************************************************************/
/*************************************************************************
	画像を描画する関数（アニメーション対応Ver.）
-------------------------------------------------------------------------
引数 :
float			x,	y		:	描画位置の左上
float			sx,	sy		:	描画する画像の大きさ
float			angle		:	画像の回転角度（degree）
float			r, g, b, a	:	画像の色情報の設定
int				cutOutAdress:	スプライト内のどこを描画するか
SpriteBatch**	sprData		:	画像データを入れるポインタ

戻り値 :
void						:	なし
*************************************************************************/
void DrawGraph(float x, float y, float sx, float sy, float angle, int cutOutAdress, SpriteBatch* data, float r, float g, float b, float a)
{
	DirectX::XMFLOAT2 texPos = data->adress.at(cutOutAdress);

	data->Draw(
		DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(sx, sy),
		texPos, data->cutOutSize,
		DirectX::XMFLOAT2((texPos.x + data->cutOutSize.x) / 2, (texPos.y + data->cutOutSize.y) / 2),
		angle,
		DirectX::XMFLOAT4(r, g, b, a)
		);
}
