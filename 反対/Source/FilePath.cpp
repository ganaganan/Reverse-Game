#include "FilePath.h"

char* GetModelPath(ModelAttribute fileAttribute)
{
	switch (fileAttribute)
	{
	case ModelAttribute::Stage:
		return "Data/Models/sitsunai.fbx";		// break;
	case ModelAttribute::TotemPole:
		return "Data/Models/paul.fbx";			// break;
	case ModelAttribute::Ghost:
		return "Data/Models/ghost.fbx";			// break;
	case ModelAttribute::Daruma:
		return "Data/Models/daruma.fbx";		// break;
	case ModelAttribute::RightDoor:
		return "Data/Models/door_Right.fbx";	// break;
	case ModelAttribute::LeftDoor:
		return "Data/Models/door_Left.fbx";		// break;
	default:
		return "NoData";						// break;
	}
}

wchar_t* GetTexturePath(TextureAttribute fileAttribute)
{
	switch (fileAttribute)
	{
	case TextureAttribute::Number:
		return L"NoData";						// break;
	case TextureAttribute::Battery:
		return L"Data/Sprite/battery.png";		// break;
	case TextureAttribute::Black:
		return L"Data/Sprite/BlackBack.png";	// break;
	case TextureAttribute::Tutorial:
		return L"Data/Sprite/tutorial.png";		// break;
	case TextureAttribute::White:
		return L"Data/Sprite/WhiteBack.png";	// break;
	case TextureAttribute::TimeWhite:
		return L"Data/Sprite/timeWhite.png";	// break;
	case TextureAttribute::Title:
		return L"Data/Sprite/title.png";		// break;
	case TextureAttribute::GameOver:
		return L"Data/Sprite/gameoverclear.png";// break;
	default:
		break;
	}
}