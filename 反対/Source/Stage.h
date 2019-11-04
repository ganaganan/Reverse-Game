#pragma once

#include <memory>

#include <Skinned_mesh.h>

class Stage
{
private:
	std::unique_ptr<SkinnedMesh> stageModel;

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