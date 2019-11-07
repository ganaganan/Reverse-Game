#include "FilePath.h"

char* GetModelPath(ModelAttribute fileAttribute)
{
	switch (fileAttribute)
	{
	case ModelAttribute::Stage:
		return "Data/Models/sitsunai.fbx";	// break;
	default:
		return "NoData";					// break;
	}
}