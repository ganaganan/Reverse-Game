#pragma once

#include<d3d11.h>
#include<DirectXMath.h>

//Box�N���X
class Box
{
public:
	//���_�V�F�[�_�X�e�[�W�𐧌䂷����s�\�v���O�����i���_�V�F�[�_�[�j���Ǘ�����B
	ID3D11VertexShader*		vertexShader;
	//�s�N�Z���V�F�[�_�[�X�e�[�W�𐧌䂷����s�\�v���O�����i�s�N�Z���V�F�[�_�[�j���Ǘ����܂��B
	ID3D11PixelShader*		pixelShader;
	//���������ɔz�u���ꂽ���_�f�[�^���O���t�B�b�N�X�p�C�v���C���̓��̓A���Z�u��X�e�[�W�ɂǂ̂悤�ɋ������邩�̒�`��ێ����܂��B
	ID3D11InputLayout*		inputLayout;
	//�o�b�t�@���\�[�X�ɃA�N�Z�X���܂��B����͔�\�����������ł��B�o�b�t�@�͒ʏ�A���_�f�[�^�܂��̓C���f�b�N�X�f�[�^���i�[���܂��B
	ID3D11Buffer*			vertexBuffer;//���_�o�b�t�@
										 //���X�^���C�U�X�e�[�W�Ƀo�C���h�ł��郉�C�X�^���C�U��Ԃ̐�����ێ����܂��B
	ID3D11RasterizerState*	rasterizerState;


public:
	//�R���X�g���N�^
	Box(ID3D11Device*);
	//�f�X�g���N�^
	~Box() {};
	//TODO : 3-1 render�֐��̃C���^�[�t�F�C�X��ύX����
	//�`��֐�
	void Render(
		ID3D11DeviceContext*,
		float dx, float dy,				//dx,dy		: Coordinate of sprite's left-top corner in screen space
		float dw, float dh,				//dw,dh		: Size of sprite in screen space
		float angle = 0,				//angle		: Raotation angle(Rotation centreis sprite's centre) Unit is degree
		float r = 1, float g = 1, float b = 1, float a = 1	//r,g,b,a	: Color of sprite's each vertics
		);
};

struct vertex
{
	DirectX::XMFLOAT3	position;
	DirectX::XMFLOAT4	color;
};