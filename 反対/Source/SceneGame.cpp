#include "SceneGame.h"

#include <DirectXMath.h>
#include <framework.h>

#include "Camera.h"
#include "Stage.h"

void SceneGame::Init()
{
	Stage::Get().Init();
}

void SceneGame::Update()
{
	Camera::Get().Update();
	Stage::Get().Update();
}

void SceneGame::Render()
{
	using namespace DirectX;

	XMMATRIX V, P;
	V =	Camera::Get().GetViewMatrix();
	P =	Camera::Get().GetProjectionMatrix();


	Stage::Get().Render(V, P);
}

void SceneGame::Uninit()
{

}