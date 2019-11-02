#include "FilePath.h"

char* GetModelPath(ModelAttribute fileAttribute)
{
	switch (fileAttribute)
	{
	case ModelAttribute::Stage:
		return "sitsunai.fbx";
	}
}