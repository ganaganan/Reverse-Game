#ifndef _INCLUDED_LOAD_H_
#define _INCLUDED_LOAD_H_

#include	"sprite.h"

/************************************************************************

	Load ƒNƒ‰ƒX

*************************************************************************/
class Load
{
public:
	Load()
	{

	}

	bool LoadGraph(const wchar_t* fileName, SpriteBatch** sprData);
	bool LoadAnimGraph(const wchar_t* _fileName, SpriteBatch** _sprData, float _cutOutSizeX = 0, float _cutOutSizeY = 0);

public:
	static Load* GetInstance()
	{
		static Load instance;
		return &instance;
	}
};

#define LOAD Load::GetInstance()

#endif //!INCLUDED_LOAD_H_