#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>

/*****************************************************************

	Sprite クラス

*****************************************************************/
class Sprite
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

public:
	//頂点シェーダステージを制御する実行可能プログラム（頂点シェーダー）を管理する。
	ID3D11VertexShader*			vertexShader;
	//ピクセルシェーダーステージを制御する実行可能プログラム（ピクセルシェーダー）を管理します。
	ID3D11PixelShader*			pixelShader;
	//メモリ内に配置された頂点データをグラフィックスパイプラインの入力アンセブらステージにどのように供給するかの定義を保持します。
	ID3D11InputLayout*			inputLayout;
	//バッファリソースにアクセスします。これは非構造化メモリです。バッファは通常、頂点データまたはインデックスデータを格納します。
	ID3D11Buffer*				vertexBuffer;//頂点バッファ
	//ラスタライザステージにバインドできるライスタライザ状態の説明を保持します。
	ID3D11RasterizerState*		rasterizerState;
	//ID3D11ShaderResourceView
	ID3D11ShaderResourceView*	shaderResourceView;
	//TEXTURE2D_DESC
	D3D11_TEXTURE2D_DESC		texture2d_desc;
	//ID3D11SamlerState
	ID3D11SamplerState*			samplerState;
	//ID3D11DepthStencilState
	ID3D11DepthStencilState*	depthStencilState;
	//ID3D11BlendState
//	ID3D11BlendState*			blendState;

	//リソースを管理するための変数（ファイル名を保存している）
	wchar_t*					resouceName;


public:
	//コンストラクタ
	Sprite(ID3D11Device*,const wchar_t*);
	//デストラクタ
	~Sprite() 
	{
		if (vertexShader)
			vertexShader->Release();
		if (pixelShader)
			pixelShader->Release();
		if (inputLayout)
			inputLayout->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (rasterizerState)
			rasterizerState->Release();
		if (shaderResourceView)
			shaderResourceView->Release();
		if (samplerState)
			samplerState->Release();
		if (depthStencilState)
			depthStencilState->Release();
		vertexShader = nullptr;
		pixelShader = nullptr;
		inputLayout = nullptr;
		vertexBuffer = nullptr;
		rasterizerState = nullptr;
		shaderResourceView = nullptr;
		samplerState = nullptr;
		depthStencilState = nullptr;
//		blendState = nullptr;
	};
	//TODO : 3-1 render関数のインターフェイスを変更する
	//描画関数
	void Render(
		ID3D11DeviceContext*,
		float dx, float dy,				//dx,dy		: Coordinate of sprite's left-top corner in screen space
		float dw, float dh,				//dw,dh		: Size of sprite in screen space
		float sx, float sy,				//sx,sy		: Coordinate of sprite's left-top corner in texture space
		float sw, float sh,				//sw,sh		: Size of sprite in texture space
		float angle = 0,				//angle		: Raotation angle(Rotation centreis sprite's centre) Unit is degree
		float r = 1, float g = 1, float b = 1, float a = 1	//r,g,b,a	: Color of sprite's each vertics
		);

	void textout(ID3D11DeviceContext *_deviceContext, std::string _s, float _x, float _y, float _w, float _h, float _r = 1, float _g = 1, float _b = 1, float _a = 1);

};


/*****************************************************************

	SpriteBatch クラス

*****************************************************************/
class SpriteBatch
{
public:
	/*構造体定義*/
	struct vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT2	texcoord;
	};

	struct instance
	{
		DirectX::XMFLOAT4X4	ndcTransform;
		DirectX::XMFLOAT4	texcoordTransform;
		DirectX::XMFLOAT4	color;
	};
private:
	ID3D11VertexShader*			vertexShader;
	ID3D11PixelShader*			pixelShader;
	ID3D11InputLayout*			inputLayout;
	ID3D11Buffer*				buffer;
	ID3D11RasterizerState*		rasterizerState;

	ID3D11ShaderResourceView*	shaderResourceView;
	D3D11_TEXTURE2D_DESC		tex2dDesc;
	ID3D11SamplerState*			samplerState;
	ID3D11DepthStencilState*	depthStencilState;
	ID3D11Buffer*				instanceBuffer;
	size_t MAX_INSTANCES;

	D3D11_VIEWPORT				viewport;
	UINT						instanceCount = 0;
	instance*					instances = nullptr;

	DirectX::XMFLOAT2			cutOutPos;
public:
	DirectX::XMFLOAT2			cutOutSize;
	std::map<int, DirectX::XMFLOAT2>	adress;

public:
	SpriteBatch(const wchar_t*, size_t, DirectX::XMFLOAT2);
	~SpriteBatch();

	/*-----------------------------------*/
	//	前処理
	/*-----------------------------------*/
	void	Begin();
	/*-----------------------------------*/
	//	描画 ( XMFLOAT版 )
	/*-----------------------------------*/
	void	Draw(
		DirectX::XMFLOAT2& position,	DirectX::XMFLOAT2& scale,
		DirectX::XMFLOAT2& texPos,		DirectX::XMFLOAT2& texSize,
		DirectX::XMFLOAT2& center,		float angle,
		DirectX::XMFLOAT4& color
		);
	/*-----------------------------------*/
	//	描画 ( float版 )
	/*-----------------------------------*/
	void	Draw(
		float x,	float y,	float sx,	float sy,
		float tx,	float ty,	float tw,	float th,
		float cx,	float cy,	float angle/*degree*/,
		float r,	float g,	float b,	float a
		);
	/*-----------------------------------*/
	//	描画 ( アニメーションを考慮する感じのやつ )
	/*-----------------------------------*/
	void	Draw(
		float x, float y, float sx, float sy,
		float angle,/*degree*/
		float r, float g, float b, float a,
		int cutOutAdress
		);

	/*-----------------------------------*/
	//	後処理
	/*-----------------------------------*/
	void	End();
};

/*----------------------------------------

	各種関数

----------------------------------------*/
extern void DrawGraph(float x, float y, float sx, float sy, float angle/*degree*/, int cutOutAdress, SpriteBatch* data, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);