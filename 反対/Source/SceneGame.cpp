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
	//���[���h�ϊ��s��擾
	/************************/
	DirectX::XMMATRIX W, S, R, T, V;
	DirectX::XMFLOAT4X4	world_view_projection;
	DirectX::XMFLOAT4X4	world_m;
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 light(1.0f, -1.0f, 0.0f, 0.0f);
	{
		//	������
		W = DirectX::XMMatrixIdentity();
		//	�g��E�k��
		S = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		//	��]
		R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		//	���s�ړ�
		T = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		//	���[���h�ϊ��s��
		W = S * R * T;

		/***************************/
		//�r���[�s���ݒ�
		/***************************/
		DirectX::XMVECTOR p = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR t = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		V = DirectX::XMMatrixLookAtLH(p, t, up);

		/****************************/
		//	�v���W�F�N�V�����s����쐬
		/****************************/
		float	fov = DirectX::XMConvertToRadians(30.0f);
		float	aspect = static_cast<float>(framework::SCREEN_WIDTH) / static_cast<float>(framework::SCREEN_HEIGHT);
		DirectX::XMMATRIX	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, 0.1f, 1000.0f);
		//Matrix -> Float4x4 �ϊ�
		DirectX::XMStoreFloat4x4(&world_view_projection, W * V * projection);
		DirectX::XMStoreFloat4x4(&world_m, W);
	}

}

void SceneGame::Uninit()
{

}