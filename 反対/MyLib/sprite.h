#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>

/*****************************************************************

	Sprite �N���X

*****************************************************************/
class Sprite
{
public:
	struct vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT2	texcoord;
	};

public:
	//���_�V�F�[�_�X�e�[�W�𐧌䂷����s�\�v���O�����i���_�V�F�[�_�[�j���Ǘ�����B
	ID3D11VertexShader*			vertexShader;
	//�s�N�Z���V�F�[�_�[�X�e�[�W�𐧌䂷����s�\�v���O�����i�s�N�Z���V�F�[�_�[�j���Ǘ����܂��B
	ID3D11PixelShader*			pixelShader;
	//���������ɔz�u���ꂽ���_�f�[�^���O���t�B�b�N�X�p�C�v���C���̓��̓A���Z�u��X�e�[�W�ɂǂ̂悤�ɋ������邩�̒�`��ێ����܂��B
	ID3D11InputLayout*			inputLayout;
	//�o�b�t�@���\�[�X�ɃA�N�Z�X���܂��B����͔�\�����������ł��B�o�b�t�@�͒ʏ�A���_�f�[�^�܂��̓C���f�b�N�X�f�[�^���i�[���܂��B
	ID3D11Buffer*				vertexBuffer;//���_�o�b�t�@
	//���X�^���C�U�X�e�[�W�Ƀo�C���h�ł��郉�C�X�^���C�U��Ԃ̐�����ێ����܂��B
	ID3D11RasterizerState*		rasterizerState;
	//ID3D11ShaderResourceView
	ID3D11ShaderResourceView*	shaderResourceView;
	//TEXTURE2D_DESC
	D3D11_TEXTURE2D_DESC		texture2d_desc;
	//ID3D11SamlerState
	ID3D11SamplerState*			samplerState;
	//ID3D11DepthStencilState
	ID3D11DepthStencilState*	depthStencilState;
	//ID3D11BlendState
//	ID3D11BlendState*			blendState;

	//���\�[�X���Ǘ����邽�߂̕ϐ��i�t�@�C������ۑ����Ă���j
	wchar_t*					resouceName;


public:
	//�R���X�g���N�^
	Sprite(ID3D11Device*,const wchar_t*);
	//�f�X�g���N�^
	~Sprite() 
	{
		if (vertexShader)
			vertexShader->Release();
		if (pixelShader)
			pixelShader->Release();
		if (inputLayout)
			inputLayout->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (rasterizerState)
			rasterizerState->Release();
		if (shaderResourceView)
			shaderResourceView->Release();
		if (samplerState)
			samplerState->Release();
		if (depthStencilState)
			depthStencilState->Release();
		vertexShader = nullptr;
		pixelShader = nullptr;
		inputLayout = nullptr;
		vertexBuffer = nullptr;
		rasterizerState = nullptr;
		shaderResourceView = nullptr;
		samplerState = nullptr;
		depthStencilState = nullptr;
//		blendState = nullptr;
	};
	//TODO : 3-1 render�֐��̃C���^�[�t�F�C�X��ύX����
	//�`��֐�
	void Render(
		ID3D11DeviceContext*,
		float dx, float dy,				//dx,dy		: Coordinate of sprite's left-top corner in screen space
		float dw, float dh,				//dw,dh		: Size of sprite in screen space
		float sx, float sy,				//sx,sy		: Coordinate of sprite's left-top corner in texture space
		float sw, float sh,				//sw,sh		: Size of sprite in texture space
		float angle = 0,				//angle		: Raotation angle(Rotation centreis sprite's centre) Unit is degree
		float r = 1, float g = 1, float b = 1, float a = 1	//r,g,b,a	: Color of sprite's each vertics
		);

	void textout(ID3D11DeviceContext *_deviceContext, std::string _s, float _x, float _y, float _w, float _h, float _r = 1, float _g = 1, float _b = 1, float _a = 1);

};


/*****************************************************************

	SpriteBatch �N���X

*****************************************************************/
class SpriteBatch
{
public:
	/*�\���̒�`*/
	struct vertex
	{
		DirectX::XMFLOAT3	position;
		DirectX::XMFLOAT2	texcoord;
	};

	struct instance
	{
		DirectX::XMFLOAT4X4	ndcTransform;
		DirectX::XMFLOAT4	texcoordTransform;
		DirectX::XMFLOAT4	color;
	};
private:
	ID3D11VertexShader*			vertexShader;
	ID3D11PixelShader*			pixelShader;
	ID3D11InputLayout*			inputLayout;
	ID3D11Buffer*				buffer;
	ID3D11RasterizerState*		rasterizerState;

	ID3D11ShaderResourceView*	shaderResourceView;
	D3D11_TEXTURE2D_DESC		tex2dDesc;
	ID3D11SamplerState*			samplerState;
	ID3D11DepthStencilState*	depthStencilState;
	ID3D11Buffer*				instanceBuffer;
	size_t MAX_INSTANCES;

	D3D11_VIEWPORT				viewport;
	UINT						instanceCount = 0;
	instance*					instances = nullptr;

	DirectX::XMFLOAT2			cutOutPos;
public:
	DirectX::XMFLOAT2			cutOutSize;
	std::map<int, DirectX::XMFLOAT2>	adress;

public:
	SpriteBatch(const wchar_t*, size_t, DirectX::XMFLOAT2);
	~SpriteBatch();

	/*-----------------------------------*/
	//	�O����
	/*-----------------------------------*/
	void	Begin();
	/*-----------------------------------*/
	//	�`�� ( XMFLOAT�� )
	/*-----------------------------------*/
	void	Draw(
		DirectX::XMFLOAT2& position,	DirectX::XMFLOAT2& scale,
		DirectX::XMFLOAT2& texPos,		DirectX::XMFLOAT2& texSize,
		DirectX::XMFLOAT2& center,		float angle,
		DirectX::XMFLOAT4& color
		);
	/*-----------------------------------*/
	//	�`�� ( float�� )
	/*-----------------------------------*/
	void	Draw(
		float x,	float y,	float sx,	float sy,
		float tx,	float ty,	float tw,	float th,
		float cx,	float cy,	float angle/*degree*/,
		float r,	float g,	float b,	float a
		);
	/*-----------------------------------*/
	//	�`�� ( �A�j���[�V�������l�����銴���̂�� )
	/*-----------------------------------*/
	void	Draw(
		float x, float y, float sx, float sy,
		float angle,/*degree*/
		float r, float g, float b, float a,
		int cutOutAdress
		);

	/*-----------------------------------*/
	//	�㏈��
	/*-----------------------------------*/
	void	End();
};

/*----------------------------------------

	�e��֐�

----------------------------------------*/
extern void DrawGraph(float x, float y, float sx, float sy, float angle/*degree*/, int cutOutAdress, SpriteBatch* data, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);