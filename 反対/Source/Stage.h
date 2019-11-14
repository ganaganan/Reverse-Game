#pragma once

#include <memory>

#include <Skinned_mesh.h>

class Stage
{
private:
	std::unique_ptr<SkinnedMesh> stageModel;
	std::unique_ptr<SkinnedMesh> rightDoorModel;
	std::unique_ptr<SkinnedMesh> leftDoorModel;

public:
	float rightDoorAngle;
	float leftDoorAngle;

private:
	Stage(){}
public:
	~Stage(){}
	void Init();
	void Uninit();
	void Update();
	void Render(DirectX::XMMATRIX& _V, DirectX::XMMATRIX& _P);

	static Stage& Get()
	{
		static Stage instance;
		return instance;
	}
};