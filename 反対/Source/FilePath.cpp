#include "FilePath.h"

char* GetModelPath(ModelAttribute fileAttribute)
{
	switch (fileAttribute)
	{
	case ModelAttribute::Stage:
		return "Data/Models/sitsunai.fbx";	// break;
	case ModelAttribute::TotemPole:
		return "Data/Models/ghost.fbx";		// break;
	case ModelAttribute::Ghost:
		return "Data/Models/ghost.fbx";		// break;
	default:
		return "NoData";					// break;
	}
}