#pragma once

#include <framework.h>

enum class ModelAttribute
{
	Stage,
};

char* GetModelPath(ModelAttribute fileAttribute);
