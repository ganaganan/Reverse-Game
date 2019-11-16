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

enum class TextureAttribute
{
	Number,
	Battery,
	Black,
	Tutorial,
	White,
	TimeWhite,
	Title,
	GameOver,
};


char* GetModelPath(ModelAttribute fileAttribute);
wchar_t* GetTexturePath(TextureAttribute fileAttribute);
