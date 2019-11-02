#include"Polygon.h"
#include<stdio.h>
#include<memory>
#include"shader.h"


Box::Box(ID3D11Device* _device)
{
	//①頂点情報のセット
	vertex virtices[] = {
		{ DirectX::XMFLOAT3(-0.5,+0.5,0),DirectX::XMFLOAT4(1,1,1,1) },
		{ DirectX::XMFLOAT3(+0.5,+0.5,0),DirectX::XMFLOAT4(1,0,0,1) },
		{ DirectX::XMFLOAT3(-0.5,-0.5,0),DirectX::XMFLOAT4(0,1,0,1) },
		{ DirectX::XMFLOAT3(+0.5,-0.5,0),DirectX::XMFLOAT4(0,0,1,1) },
	};

	//②頂点バッファオブジェクトの生成

	//TODO : 3-2 spriteクラスのコンストラクタで頂点バッファオブジェクトの生成方法を変更する
	//GPU（読み取りのみ）とCPU（書き込みのみ）によるアクセスを可能にする
	//バッファリソースを記述する
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(virtices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//書き込みのみアクセスを可能にする
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	//複資源を初期設定するためのデータを指定します
	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = virtices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	//バッファ（頂点バッファ、インデックスバッファ、またはシェーダ定数バッファ）を作成します
	HRESULT hr = S_OK;
	hr = _device->CreateBuffer(&bufferDesc, &subResourceData, &vertexBuffer);
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
	};

	//CSOファイルをロードする
#if 0 
	FILE* fp = nullptr;
	fopen_s(&fp, "sprite_vs.cso", "rb");
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	//CreateVertexShader methot
	hr = _device->CreateVertexShader(
		cso_data.get(),		//コンパイルされたシェーダへのポインタ
		cso_sz,				//コンパイルされた頂点シェーダのサイズ
		nullptr,			//クラスリンケージインターフェースへのポインタ（nullでかまわない）
		&vertexShader		//ID3I11VertexShaderへのポインタ
		);
	if (FAILED(hr))
		assert(hr);
	//④入力レイアウトオブジェクトの生成
	//CreateInputLayout
	hr = _device->CreateInputLayout(
		input_element_desc,				//入力アセンブラステージの入力データ型の配列
		ARRAYSIZE(input_element_desc),	//入力用紙の配列内の入力データ型の数
		cso_data.get(),					//コンパイルされたシェーダへのポインタ
		cso_sz,							//コンパイルされたシェーダのサイズ
		&inputLayout					//ID3D11InputLayoutのポインタ
		);
	if (FAILED(hr))
		assert(hr);

	//⑤ピクセルシェーダオブジェクトの生成
	//sprite_psをロード
	FILE* fp2 = nullptr;
	fopen_s(&fp2, "sprite_ps.cso", "rb");

	fseek(fp2, 0, SEEK_END);
	long cso_sz2 = ftell(fp2);
	fseek(fp2, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data2 = std::make_unique<unsigned char[]>(cso_sz2);
	fread(cso_data2.get(), cso_sz2, 1, fp2);
	fclose(fp2);

	//CreatePixelShader methot
	hr = _device->CreatePixelShader(
		cso_data2.get(),	//コンパイルされたシェーダへのポインタ
		cso_sz2,			//コンパイルされたピクセルシェーダのサイズ
		nullptr,			//クラスリンケージインタフェースへのポインタ（nullでかまわない）
		&pixelShader		//ID3D11PixelShagerへのポインタ
		);
	if (FAILED(hr))
		assert(hr);
#else
	create_vs_from_cso(_device, "Polygon_vs.cso", &vertexShader, &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(_device, "Polygon_ps.cso", &pixelShader);
#endif
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
	hr = _device->CreateRasterizerState(&rasterizer_desc, &rasterizerState);
	if (FAILED(hr))
		assert(hr);
}

void Box::Render(ID3D11DeviceContext* _deviceContext, float dx, float dy, float dw, float dh, float angle, float r, float g, float b, float a)
{
	// dx, dy : Coordinate of sprite's left-top corner in screen space 
	// dw, dh : Size of sprite in screen space 
	// angle : Raotation angle (Rotation centre is sprite's centre), unit is degree
	// r, g, b : Color of sprite's each vertices 

	D3D11_VIEWPORT viewport;
	UINT numViewport = 1;
	_deviceContext->RSGetViewports(&numViewport, &viewport);

	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	float lt_x = dx - dw;	//左上
	float lt_y = dy - dh;

	float rt_x = dx + dw;	//右上
	float rt_y = dy - dh;

	float lb_x = dx - dw;	//左下
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
	hr = _deviceContext->Map(
		vertexBuffer,	//ID3D11Resourceへのポインタ
		0,				//副リソースのインデックス番号
		map,			//リソースに対するCPUの読み取りおよび書き込み権限を指定するD3D11_MAPタイプの値
		0,				//GPUがビジーのときにCPUが何をするかを指定するフラグ
		&mapped_buffer	//マップされたサブリソースのD3D11_MAPPED_SUBRESOURCE構造体へのポインタ
		);
	if (FAILED(hr))
		return;

	//verticesに入れる
	vertex* vertices = static_cast<vertex *>(mapped_buffer.pData);
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

	_deviceContext->Unmap(vertexBuffer, 0);


	UINT stride = sizeof(vertex);
	UINT offset = 0;
	//①頂点バッファーのバインド
	_deviceContext->IASetVertexBuffers(
		0,				//バインディグ用の最初の入力スロット
		1,				//配列内の頂点バッファの数
		&vertexBuffer,	//頂点バッファの配列へのポインタ
		&stride,		//ストライド値の配列のポインタ
		&offset			//オフセット値へのポインタ
		);

	//②プリミティブタイプ及びデータの順序に関する情報のバインド
	_deviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	//プリミティブの種類とプリミティブデータの順序
		);

	//③入力レイアウトオブジェクトのバインド
	_deviceContext->IASetInputLayout(
		inputLayout		//ID3D11InputLayoutのポインタ
		);

	//④ラスタライザーステートの設定
	_deviceContext->RSSetState(
		rasterizerState	//パイプラインにバインドするためのラスタライザ状態インタフェースへのポインタ
		);

	//⑤シェーダの設定
	_deviceContext->VSSetShader(
		vertexShader,	//頂点シェーダへのポインタ
		nullptr,		//クラスインスタンスインターフェースの配列へのポインタ。シェーダがインターフェースを使用しない場合はNULL
		0				//配列内のクラスインスタンスインターフェースの数
		);
	_deviceContext->PSSetShader(
		pixelShader,	//ピクセルシェーダへのポインタ
		nullptr,		//クラスインスタンスインターフェースの配列へのポインタ。シェーダがインターフェースを使用しない場合はNULL
		0				//配列内のクラスインスタンスインターフェースの数
		);

	//⑥プリミティブの描画
	_deviceContext->Draw(
		4,	//描画する頂点の数
		0	//最初の頂点のインデックス
		);
}