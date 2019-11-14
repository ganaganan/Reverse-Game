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