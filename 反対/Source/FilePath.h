#pragma once

#include <framework.h>

enum class ModelAttribute
{
	Stage,
	TotemPole,
	Ghost,
};

char* GetModelPath(ModelAttribute fileAttribute);
