#include"Load.h"
#include"framework.h"

/*************************************************************************
	�摜�f�[�^�����[�h����֐�
-------------------------------------------------------------------------
���� : 
	wchar_t*		fileName	:	�摜�f�[�^�̃A�h���X
	SpriteBatch**	sprData		:	�摜�f�[�^������|�C���^

�߂�l : 
	bool						:	����������true��Ԃ�
*************************************************************************/
bool	Load::LoadGraph(const wchar_t* fileName, SpriteBatch** sprData)
{
	*sprData = new SpriteBatch(fileName, 1000, DirectX::XMFLOAT2(0,0));
	if (sprData != nullptr)return true;
	return false;
}

/*************************************************************************
�A�j���[�V����������摜�f�[�^�����[�h����֐�
-------------------------------------------------------------------------
���� :
wchar_t*		fileName	:	�摜�f�[�^�̃A�h���X
SpriteBatch**	sprData		:	�摜�f�[�^������|�C���^
float			cutOutSizeX	:	��R�}�̃T�C�Y���w�肷��ix�j
float			cutOutSizeY	:	��R�}�̃T�C�Y���w�肷��iy�j

�߂�l :
bool						:	����������true��Ԃ�
*************************************************************************/
bool	Load::LoadAnimGraph(const wchar_t* _fileName, SpriteBatch** _sprData, float _cutOutSizeX, float _cutOutSizeY)
{
	*_sprData = new SpriteBatch(_fileName, 1, DirectX::XMFLOAT2A(_cutOutSizeX, _cutOutSizeY));
	if (_sprData != nullptr)return true;
	return false;
}
