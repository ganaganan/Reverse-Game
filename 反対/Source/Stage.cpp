#include "Stage.h"
#include "FilePath.h"
#include "BaseScene.h"

float posX;
float posY;
float posZ;
float pos2X;
float pos2Y;
float pos2Z;

void Stage::Init()
{
	if (BaseScene::GetNowDay() == 2)
	{
		stageModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::KowaiStage));
	}
	else
	{
		stageModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Stage));
	}
	if (!rightDoorModel)
		rightDoorModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::RightDoor));
	if (!leftDoorModel)
		leftDoorModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::LeftDoor));

	rightDoorAngle = 0.0f;
	leftDoorAngle = 0.0f;

	posX = 0.0f;
	posY = 0.0f;
	posZ = 0.0f;
	pos2X = 0.0f;
	pos2Y = 0.0f;
	pos2Z = 0.0f;
}

void Stage::Update()
{
	//rightDoorAngle += 1.0f * 0.01745f;
	//leftDoorAngle += 1.0f * 0.01745f;
	//if (rightDoorAngle >= 360)
	//{
	//	rightDoorAngle = 0.0f;
	//}
	//if (leftDoorAngle >= 360)
	//{
	//	leftDoorAngle = 0.0f;
	//}

#ifdef USE_IMGUI
	ImGui::Begin("Stage parameter");
	ImGui::SliderFloat(" rightX ", &posX, -50.0f, 50.0f);
	ImGui::SliderFloat(" rightY ", &posY, -50.0f, 50.0f);
	ImGui::SliderFloat(" rightZ ", &posZ, -50.0f, 50.0f);
	ImGui::SliderFloat(" leftX ", &pos2X, -50.0f, 50.0f);
	ImGui::SliderFloat(" leftY ", &pos2Y, -50.0f, 50.0f);
	ImGui::SliderFloat(" leftZ ", &pos2Z, -50.0f, 50.0f);
	ImGui::End();
#endif
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
	XMFLOAT4 color{ 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT4 light(1.0f, -1.0f, 1.0f, 0.0f);
	// situnai
	{
		S = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		R = XMMatrixIdentity();
		T = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		W = S * R * T;

		WVP = W * _V * _P;


		stageModel->Render(FRAMEWORK->GetDeviceContext(),
			Float4x4(WVP),
			Float4x4(W),
			light,
			color
		);
	}
	// right door
	{
		S = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		R = XMMatrixRotationRollPitchYaw(0.0f, rightDoorAngle + DirectX::XMConvertToRadians(12.0f) , 0.0f);//XMMatrixIdentity();
		T = XMMatrixTranslation(20.92f, 1.395f, 19.850f);	// magic number
		W = S * R * T;

		WVP = W * _V * _P;

		rightDoorModel->Render(FRAMEWORK->GetDeviceContext(),
			Float4x4(WVP),
			Float4x4(W),
			light,
			color
		);
	}
	// left door
	{
		S = XMMatrixScaling(0.1f, 0.1f, 0.1f);
		R = XMMatrixRotationRollPitchYaw(0.0f, leftDoorAngle + DirectX::XMConvertToRadians(-12.0f), 0.0f);//XMMatrixIdentity();
		T = XMMatrixTranslation(-20.88f, 1.395f, 19.850f);
		W = S * R * T;

		WVP = W * _V * _P;

		leftDoorModel->Render(FRAMEWORK->GetDeviceContext(),
			Float4x4(WVP),
			Float4x4(W),
			light,
			color
		);
	}
}

void Stage::Uninit()
{

}