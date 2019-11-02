#pragma once

#include <memory>

#include <Skinned_mesh.h>

class Stage
{
	std::unique_ptr<SkinnedMesh> stageModel;

public:
	void Init();
	void Uninit();
	void Update();
	void Render();
};