#include"Load.h"
#include"framework.h"

/*************************************************************************
	画像データをロードする関数
-------------------------------------------------------------------------
引数 : 
	wchar_t*		fileName	:	画像データのアドレス
	SpriteBatch**	sprData		:	画像データを入れるポインタ

戻り値 : 
	bool						:	成功したらtrueを返す
*************************************************************************/
bool	Load::LoadGraph(const wchar_t* fileName, SpriteBatch** sprData)
{
	*sprData = new SpriteBatch(fileName, 1000, DirectX::XMFLOAT2(0,0));
	if (sprData != nullptr)return true;
	return false;
}

/*************************************************************************
アニメーションさせる画像データをロードする関数
-------------------------------------------------------------------------
引数 :
wchar_t*		fileName	:	画像データのアドレス
SpriteBatch**	sprData		:	画像データを入れるポインタ
float			cutOutSizeX	:	一コマのサイズを指定する（x）
float			cutOutSizeY	:	一コマのサイズを指定する（y）

戻り値 :
bool						:	成功したらtrueを返す
*************************************************************************/
bool	Load::LoadAnimGraph(const wchar_t* _fileName, SpriteBatch** _sprData, float _cutOutSizeX, float _cutOutSizeY)
{
	*_sprData = new SpriteBatch(_fileName, 1, DirectX::XMFLOAT2A(_cutOutSizeX, _cutOutSizeY));
	if (_sprData != nullptr)return true;
	return false;
}
