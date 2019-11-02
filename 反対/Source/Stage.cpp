#include "Stage.h"
#include "FilePath.h"

void Stage::Init()
{
	stageModel = std::make_unique<SkinnedMesh>(FRAMEWORK->GetDevice(), GetModelPath(ModelAttribute::Stage));
}

void Stage::Update()
{

}

void Stage::Render()
{

}

void Stage::Uninit()
{

}