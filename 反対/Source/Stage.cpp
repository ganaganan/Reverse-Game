#include "Stage.h"
#include "FilePath.h"

void Stage::Init()
{
	stageModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Stage));
}

void Stage::Update()
{

}

void Stage::Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P)
{
	using namespace DirectX;
	auto Matrix = [](const XMFLOAT4X4 & matrix)
	{
		return XMLoadFloat4x4(&matrix);
	};
	auto Float4x4 = [](const XMMATRIX & M)
	{
		XMFLOAT4X4 matrix{};
		XMStoreFloat4x4(&matrix, M);
		return matrix;
	};

	XMMATRIX S, R, T, W, WVP;
	S = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	R = XMMatrixIdentity();
	T = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	W = S * R * T;

	WVP = W * _V * _P;

	XMFLOAT4 color{ 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT4 light(1.0f, -1.0f, 0.0f, 0.0f);

	stageModel->Render(FRAMEWORK->GetDeviceContext(),
		Float4x4(WVP), 
		Float4x4(W), 
		light, 
		color
	);
}

void Stage::Uninit()
{

}