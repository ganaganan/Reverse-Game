#pragma once

#include <framework.h>

enum class ModelAttribute
{
	Stage,
	TotemPole,
	Ghost,
	Daruma,
	RightDoor,
	LeftDoor,
};


char* GetModelPath(ModelAttribute fileAttribute);
