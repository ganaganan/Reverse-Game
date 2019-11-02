#include "SceneGame.h"

#include <DirectXMath.h>
#include <framework.h>

void SceneGame::Init()
{

}

void SceneGame::Update()
{

}

void SceneGame::Render()
{
	/************************/
	//ワールド変換行列取得
	/************************/
	DirectX::XMMATRIX W, S, R, T, V;
	DirectX::XMFLOAT4X4	world_view_projection;
	DirectX::XMFLOAT4X4	world_m;
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 light(1.0f, -1.0f, 0.0f, 0.0f);
	{
		//	初期化
		W = DirectX::XMMatrixIdentity();
		//	拡大・縮小
		S = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		//	回転
		R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		//	平行移動
		T = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		//	ワールド変換行列
		W = S * R * T;

		/***************************/
		//ビュー行列を設定
		/***************************/
		DirectX::XMVECTOR p = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR t = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		V = DirectX::XMMatrixLookAtLH(p, t, up);

		/****************************/
		//	プロジェクション行列を作成
		/****************************/
		float	fov = DirectX::XMConvertToRadians(30.0f);
		float	aspect = static_cast<float>(framework::SCREEN_WIDTH) / static_cast<float>(framework::SCREEN_HEIGHT);
		DirectX::XMMATRIX	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, 0.1f, 1000.0f);
		//Matrix -> Float4x4 変換
		DirectX::XMStoreFloat4x4(&world_view_projection, W * V * projection);
		DirectX::XMStoreFloat4x4(&world_m, W);
	}

}

void SceneGame::Uninit()
{

}