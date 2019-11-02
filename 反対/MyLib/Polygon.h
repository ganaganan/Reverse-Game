#pragma once

#include<d3d11.h>
#include<DirectXMath.h>

//Boxクラス
class Box
{
public:
	//頂点シェーダステージを制御する実行可能プログラム（頂点シェーダー）を管理する。
	ID3D11VertexShader*		vertexShader;
	//ピクセルシェーダーステージを制御する実行可能プログラム（ピクセルシェーダー）を管理します。
	ID3D11PixelShader*		pixelShader;
	//メモリ内に配置された頂点データをグラフィックスパイプラインの入力アンセブらステージにどのように供給するかの定義を保持します。
	ID3D11InputLayout*		inputLayout;
	//バッファリソースにアクセスします。これは非構造化メモリです。バッファは通常、頂点データまたはインデックスデータを格納します。
	ID3D11Buffer*			vertexBuffer;//頂点バッファ
										 //ラスタライザステージにバインドできるライスタライザ状態の説明を保持します。
	ID3D11RasterizerState*	rasterizerState;


public:
	//コンストラクタ
	Box(ID3D11Device*);
	//デストラクタ
	~Box() {};
	//TODO : 3-1 render関数のインターフェイスを変更する
	//描画関数
	void Render(
		ID3D11DeviceContext*,
		float dx, float dy,				//dx,dy		: Coordinate of sprite's left-top corner in screen space
		float dw, float dh,				//dw,dh		: Size of sprite in screen space
		float angle = 0,				//angle		: Raotation angle(Rotation centreis sprite's centre) Unit is degree
		float r = 1, float g = 1, float b = 1, float a = 1	//r,g,b,a	: Color of sprite's each vertics
		);
};

struct vertex
{
	DirectX::XMFLOAT3	position;
	DirectX::XMFLOAT4	color;
};