#include <stdio.h>
#include <memory>
#include <map>
#include "sprite.h"
#include "shader.h"
#include "texture.h"
#include "WICTextureLoader.h"
#include "framework.h"

/**********************************************************************************************************************

Sprite �N���X

**********************************************************************************************************************/
//sprite�N���X�̃R���X�g���N�^�̎���
Sprite::Sprite(ID3D11Device* orgDevice, const wchar_t* texture)
{
	//�@���_���̃Z�b�g
	vertex virtices[] = {
		{ DirectX::XMFLOAT3(-0.5,+0.5,0),DirectX::XMFLOAT4(1,1,1,1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(+0.5,+0.5,0),DirectX::XMFLOAT4(1,0,0,1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(-0.5,-0.5,0),DirectX::XMFLOAT4(0,1,0,1), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(+0.5,-0.5,0),DirectX::XMFLOAT4(0,0,1,1), DirectX::XMFLOAT2(0, 0) },
	};

	//�A���_�o�b�t�@�I�u�W�F�N�g�̐���
	//TODO : 3-2 sprite�N���X�̃R���X�g���N�^�Œ��_�o�b�t�@�I�u�W�F�N�g�̐������@��ύX����
	//GPU�i�ǂݎ��̂݁j��CPU�i�������݂̂݁j�ɂ��A�N�Z�X���\�ɂ���
	//�o�b�t�@���\�[�X���L�q����
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(virtices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//�������݂̂݃A�N�Z�X���\�ɂ���
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	//�������������ݒ肷�邽�߂̃f�[�^���w�肵�܂�
	D3D11_SUBRESOURCE_DATA subResourceData = {};
	subResourceData.pSysMem = virtices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;
	
	//�o�b�t�@�i���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�܂��̓V�F�[�_�萔�o�b�t�@�j���쐬���܂�
	HRESULT hr = S_OK;
	hr = orgDevice->CreateBuffer(&bufferDesc, &subResourceData, &vertexBuffer);
	if (FAILED(hr))
		assert(hr);

	//�B���_�V�F�[�_�[�I�u�W�F�N�g�̐���
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		/*�����o�[�Љ�*/
		//semanticName			: �V�F�[�_�[��input-signature�ł��̗v�f�Ɋ֘A�t�����Ă���HLSL�Z�}���e�B�N�X
		//semanticIndex			: �����Z�}���e�B�b�N�����v�f����������ꍇ�ɂ̂ݕK�v
		//Format				: DXGI_FORMAT���Q��
		//InputSlot				: ���̓A���Z�u�������ʂ��鐮���l�ł��B�L��������0�`15
		//AlignedByteoffset		: �I�v�V�����ł��B�֋X��AD3D111_APPEND_ALIGNED_ELEMENT���g�p����
		//InputSlotClass		: �P����̓X���b�g�̓��̓f�[�^�N���X�����ʂ��܂��B
		//InstanceDataStepRate	: �o�b�t�@�����P�v�f�i�ޑO�ɓ����C���X�^���X���Ƃ̃f�[�^���g�p���ĕ`�悷��C���X�^���X�̐�
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//CSO�t�@�C�������[�h����

	//�D�o�[�e�b�N�X�V�F�[�_�I�u�W�F�N�g�̐���
	//sprite_vs�����[�h����
	create_vs_from_cso(orgDevice, "Data/Shaders/sprite_vs.cso", &vertexShader, &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));
	//�D�s�N�Z���V�F�[�_�I�u�W�F�N�g�̐���
	//sprite_ps�����[�h
	create_ps_from_cso(orgDevice, "Data/Shaders/sprite_ps.cso", &pixelShader);
	//���[�h�I��

	//�E���X�^���C�U�[�X�e�[�g�̐ݒ�
	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;	//�����_�����O���Ɏg�p����h��Ԃ����[�h�����肷��
	rasterizer_desc.CullMode = D3D11_CULL_NONE;		//�w�肳�ꂽ�����������Ă���O�p�`���`����Ă��Ȃ����Ƃ������܂�
	rasterizer_desc.FrontCounterClockwise = FALSE;	//�O�p�`�����ʂ������Ă��邩�w�ʂ������Ă��邩�����肵�܂��B(TRUE�̏ꍇ�A�O�p�`�͂��̒��_�������_�����O�^�[�Q�b�g��Ŕ����v���ɂ���ꍇ�͐��ʂ������Ă���Ƃ݂Ȃ����)
	rasterizer_desc.DepthBias = 0;					//����̃s�N�Z���ɒǉ����ꂽ�[�x�l
	rasterizer_desc.DepthBiasClamp = 0;				//�s�N�Z���̍ő�[�x�o�C�A�X
	rasterizer_desc.SlopeScaledDepthBias = 0;		//�^����ꂽ�s�N�Z���̌X���̃X�J���[
	rasterizer_desc.DepthClipEnable = FALSE;		//�����Ɋ�Â��ăN���b�s���O��L���ɂ��܂�
	rasterizer_desc.ScissorEnable = FALSE;			//�V�U�[�����`�J�����O��L���ɂ��܂�
	rasterizer_desc.MultisampleEnable = FALSE;		//MSAA�����_�[�^�[�Q�b�g�Ŏl�ӌ`�܂��̓���̃A���`�G�C���A�V���O�A���S���Y�����g�p���邩�ǂ������w�肵�܂�
	rasterizer_desc.AntialiasedLineEnable = FALSE;	//���C���A���`�G�C���A�V���O��L���ɂ��邩�ǂ������w�肵�܂�

	//CreateRasterizerState methot
	hr = orgDevice->CreateRasterizerState(&rasterizer_desc, &rasterizerState);
	if (FAILED(hr))
		assert(hr);

	//�e�N�X�`���̃��[�h
	//�摜�̓ǂݍ���
	hr = load_texture_from_file(orgDevice, texture, &shaderResourceView, &texture2d_desc);

	//�T���v���[�X�e�[�g�I�u�W�F�N�g�̐���(ID3D11SamplerState)�̐���
	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;																//�e�N�X�`�����T���v�����O����Ƃ��Ɏg�p����t�B���^�����O���@
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;														//0����1�͈̔͊O��au�e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;														//0����1�͈̔͊O�̃e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;														//0����1�͈̔͊O��W�e�N�X�`�����W���������邽�߂Ɏg�p���郁�\�b�h
	sampler_desc.MipLODBias = 0;																				//�v�Z���ꂽ�~�b�v�}�b�v���x������̃I�t�Z�b�g
	sampler_desc.MaxAnisotropy = 16;																			//D3D11_FILTER_ANISOTROPIC�܂���D3D11_FILTER_COMPARISON_ANISOTROPIC���t�B���^�Ŏw�肳��Ă���ꍇ�Ɏg�p�����N�����v�l �L���l��1�`16
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;														//�T���v�����O�f�[�^�������̃T���v�����O�f�[�^�Ɣ�r����֐�
	memcpy(sampler_desc.BorderColor, &DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(DirectX::XMFLOAT4));	//AddressU,AddressV,AddressW��D3D11_TEXTURE_ADDRESS_BORDER���w�肳��Ă���ꍇ�Ɏg�p����g�̐F�B�͈͂�0.0�ȏ�1.0�ȉ�
	sampler_desc.MinLOD = 0;																					//�A�N�Z�X�𐧌�����~�b�v�}�b�v�͈͂̉���
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;																	//�A�N�Z�X�𐧌�����~�b�v�}�b�v�͈͂̏��
	//CreateSamplerState methot
	hr = orgDevice->CreateSamplerState(&sampler_desc, &samplerState);
	if (FAILED(hr))
		assert(hr);

	//�[�x�X�e���V���X�e�[�g�Ɏg���\���̂̍쐬
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�����I
	hr = orgDevice->CreateDepthStencilState(&depth_stencil_desc, &depthStencilState);
	if (FAILED(hr))
		assert(hr);
}

//TODO6 : Sprite�N���X�� render �����o�֐��̎�����ύX����
void Sprite::Render(ID3D11DeviceContext* orgDeviceContext, float dx, float dy, float dw, float dh, float sx, float sy, float sw, float sh, float angle, float r, float g, float b, float a)
{
	// dx, dy	: Coordinate of sprite's left-top corner in screen space 
	// dw, dh	: Size of sprite in screen space 
	// sx,sy	: Coordinate of sprite's left-top corner in texture space
	// sw,sh	: Size of sprite in texture space
	// angle	: Raotation angle (Rotation centre is sprite's centre), unit is degree
	// r, g, b,a: Color of sprite's each vertices 

	D3D11_VIEWPORT viewport;
	UINT numViewport = 1;
	orgDeviceContext->RSGetViewports(&numViewport, &viewport);
	
	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	float lt_x = dx;		//����
	float lt_y = dy;

	float rt_x = dx + dw;	//�E��
	float rt_y = dy;

	float lb_x = dx;		//����
	float lb_y = dy + dh;

	float rb_x = dx + dw;	//�E��
	float rb_y = dy + dh;


	float mx = dx + dw*0.5f;
	float my = dy + dh*0.5f;
	lt_x -= mx;
	lt_y -= my;
	rt_x -= mx;
	rt_y -= my;
	lb_x -= mx;
	lb_y -= my;
	rb_x -= mx;
	rb_y -= my;

	//��]����
	float sin = sinf(angle*0.01745f);
	float cos = cosf(angle*0.01745f);
	float rx, ry;
	
	rx = lt_x;
	ry = lt_y;
	lt_x = cos*rx + -sin*ry;
	lt_y = sin*rx + cos*ry;
	rx = rt_x;
	ry = rt_y;
	rt_x = cos*rx + -sin*ry;
	rt_y = sin*rx + cos*ry;
	rx = lb_x;
	ry = lb_y;
	lb_x = cos*rx + -sin*ry;
	lb_y = sin*rx + cos*ry;
	rx = rb_x;
	ry = rb_y;
	rb_x = cos*rx + -sin*ry;
	rb_y = sin*rx + cos*ry;

	lt_x += mx;
	lt_y += my;
	rt_x += mx;
	rt_y += my;
	lb_x += mx;
	lb_y += my;
	rb_x += mx;
	rb_y += my;

	//NDC���W�ϊ�
	float ndc_lt_x = (lt_x / screen_width) * 2 - 1;
	float ndc_lt_y = 1 - ((lt_y / screen_height) * 2);

	float ndc_rt_x = (rt_x / screen_width) * 2 - 1;
	float ndc_rt_y = 1 - ((rt_y / screen_height) * 2);

	float ndc_lb_x = (lb_x / screen_width) * 2 - 1;
	float ndc_lb_y = 1 - ((lb_y / screen_height) * 2);

	float ndc_rb_x = (rb_x / screen_width) * 2 - 1;
	float ndc_rb_y = 1 - ((rb_y / screen_height) * 2);

	//�v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V����
	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;

	HRESULT hr = S_OK;
	hr = orgDeviceContext->Map(
		vertexBuffer,	//ID3D11Resource�ւ̃|�C���^
		0,				//�����\�[�X�̃C���f�b�N�X�ԍ�
		map,			//���\�[�X�ɑ΂���CPU�̓ǂݎ�肨��я������݌������w�肷��D3D11_MAP�^�C�v�̒l
		0,				//GPU���r�W�[�̂Ƃ���CPU���������邩���w�肷��t���O
		&mapped_buffer	//�}�b�v���ꂽ�T�u���\�[�X��D3D11_MAPPED_SUBRESOURCE�\���̂ւ̃|�C���^
		);
	if (FAILED(hr))
		return;

	//vertices�ɓ����
	vertex* vertices= static_cast<vertex *>(mapped_buffer.pData);
	vertices[0].position.x = ndc_lt_x;
	vertices[0].position.y = ndc_lt_y;
	vertices[1].position.x = ndc_rt_x;
	vertices[1].position.y = ndc_rt_y;
	vertices[2].position.x = ndc_lb_x;
	vertices[2].position.y = ndc_lb_y;
	vertices[3].position.x = ndc_rb_x;
	vertices[3].position.y = ndc_rb_y;
	vertices[0].position.z = vertices[1].position.z = vertices[2].position.z = vertices[3].position.z = 0;

	DirectX::XMFLOAT4 color(r, g, b, a);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

	//�e�N�X�`�����W�̌v�Z�ƒ��_�o�b�t�@�̍X�V
	vertices[0].texcoord.x = static_cast<FLOAT>(sx) / texture2d_desc.Width;
	vertices[0].texcoord.y = static_cast<FLOAT>(sy) / texture2d_desc.Height;
	vertices[1].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2d_desc.Width;
	vertices[1].texcoord.y = static_cast<FLOAT>(sy) / texture2d_desc.Height;
	vertices[2].texcoord.x = static_cast<FLOAT>(sx) / texture2d_desc.Width;
	vertices[2].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2d_desc.Height;
	vertices[3].texcoord.x = static_cast<FLOAT>(sx + sw) / texture2d_desc.Width;
	vertices[3].texcoord.y = static_cast<FLOAT>(sy + sh) / texture2d_desc.Height;

	orgDeviceContext->Unmap(vertexBuffer, 0);


	UINT stride = sizeof(vertex);
	UINT offset = 0;
	//�@���_�o�b�t�@�[�̃o�C���h
	orgDeviceContext->IASetVertexBuffers(
		0,				//�o�C���f�B�O�p�̍ŏ��̓��̓X���b�g
		1,				//�z����̒��_�o�b�t�@�̐�
		&vertexBuffer,	//���_�o�b�t�@�̔z��ւ̃|�C���^
		&stride,		//�X�g���C�h�l�̔z��̃|�C���^
		&offset			//�I�t�Z�b�g�l�ւ̃|�C���^
		);

	//�A�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	orgDeviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	//�v���~�e�B�u�̎�ނƃv���~�e�B�u�f�[�^�̏���
		);

	//�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	orgDeviceContext->IASetInputLayout(
		inputLayout		//ID3D11InputLayout�̃|�C���^
		);

	//�C���X�^���C�U�[�X�e�[�g�̐ݒ�
	orgDeviceContext->RSSetState(
		rasterizerState	//�p�C�v���C���Ƀo�C���h���邽�߂̃��X�^���C�U��ԃC���^�t�F�[�X�ւ̃|�C���^
		);

	//�D�V�F�[�_�̐ݒ�
	//VertexShader
	orgDeviceContext->VSSetShader(
		vertexShader,	//���_�V�F�[�_�ւ̃|�C���^
		nullptr,		//�N���X�C���X�^���X�C���^�[�t�F�[�X�̔z��ւ̃|�C���^�B�V�F�[�_���C���^�[�t�F�[�X���g�p���Ȃ��ꍇ��NULL
		0				//�z����̃N���X�C���X�^���X�C���^�[�t�F�[�X�̐�
		);
	//PixelShader
	orgDeviceContext->PSSetShader(
		pixelShader,	//�s�N�Z���V�F�[�_�ւ̃|�C���^
		nullptr,		//�N���X�C���X�^���X�C���^�[�t�F�[�X�̔z��ւ̃|�C���^�B�V�F�[�_���C���^�[�t�F�[�X���g�p���Ȃ��ꍇ��NULL
		0				//�z����̃N���X�C���X�^���X�C���^�[�t�F�[�X�̐�
		);

	//�V�F�[�_�[���\�[�X�̃o�C���h
	orgDeviceContext->PSSetShaderResources(
		0,					//�f�o�C�X�̃[���x�[�X�̔z��ɃC���f�b�N�X��t���āA�V�F�[�_���\�[�X�̐ݒ���J�n���܂��i0�`D3D11_COMMONSHADER_INPUT_RESOURCE_SHOT_COUNT -1 �͈̔́j
		1,					//�ݒ肷��V�F�[�_���\�[�X�ɂ͍ő�128�X���b�g�܂Ŏg�p�ł��܂�(0�`D3D11_COMMONSHADER_INPUT_RESOURCE_SHOT_COUNT -StartShot �͈̔�)
		&shaderResourceView	//�f�o�C�X�ɐݒ肷��V�F�[�_���\�[�X�r���[�C���^�[�t�F�C�X�̔z��
		);

	//�T���v���[�X�e�[�g�̐ݒ�
	orgDeviceContext->PSSetSamplers(
		0,					//�f�o�C�X�̃[���x�[�X�̔z��ɃC���f�b�N�X��t���āA�T���v���̐ݒ���J�n���܂��i0�`D3D11_COMMONSHADER_INPUT_RESOURCE_SHOT_COUNT -1 �͈̔́j
		1,					//�z����̃T���v���� �e�p�C�v���C���X�e�[�W�ɂ́A���v16�̃T���v���[�X���b�g������܂�(0�`D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT -StartSlot�͈̔�)
		&samplerState		//�T���v����ԃC���^�[�t�F�[�X�̔z��ւ̃|�C���^
		);

	//�[�x�X�e���V���X�e�[�g�̐ݒ�
	orgDeviceContext->OMSetDepthStencilState(
		depthStencilState,
		1
		);

	//�E�v���~�e�B�u�̕`��
	orgDeviceContext->Draw(
		4,	//�`�悷�钸�_�̐�
		0	//�ŏ��̒��_�̃C���f�b�N�X
		);
}


void Sprite::textout(ID3D11DeviceContext *_deviceContext, std::string _s, float _x, float _y, float _w, float _h, float _r, float _g, float _b, float _a)
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float cursor = 0;
	for (auto c : _s)
	{
		LONG sx = c % 0x0F;
		Render(_deviceContext, _x + cursor, _y, _w, _h, sw*(c & 0x0F), sh*(c >> 4), sw, sh, 0, _r, _g, _b, _a);
		cursor += _w;
	}
}



/**********************************************************************************************************************

	SpriteBatch �N���X

**********************************************************************************************************************/

/*--------------------------------*/
//	�R���X�g���N�^
/*--------------------------------*/
SpriteBatch::SpriteBatch(const wchar_t* _fileName, size_t _maxInstance, DirectX::XMFLOAT2 _cutOutSize)
{
	HRESULT hr = S_OK;

	MAX_INSTANCES = _maxInstance;

	ID3D11Device* device = FRAMEWORK->GetDevice();

	//vertexBuffer�̍쐬
	vertex vertices[] = {
		{ DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2(0, 0) },
		{ DirectX::XMFLOAT3(1, 0, 0), DirectX::XMFLOAT2(1, 0) },
		{ DirectX::XMFLOAT3(0, 1, 0), DirectX::XMFLOAT2(0, 1) },
		{ DirectX::XMFLOAT3(1, 1, 0), DirectX::XMFLOAT2(1, 1) },
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;;
	bufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	device->CreateBuffer(&bufferDesc, &subresourceData, &buffer);
	assert(SUCCEEDED(hr) && "VertexBuffer����Ă���~�܂����� by SpriteBatch");


	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",           0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "TEXCOORD",           0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,   0 },
		{ "NDC_TRANSFORM",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM",      3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR",              0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	UINT numElements = ARRAYSIZE(layout);

	//�V�F�[�_�[�̃��[�h
	hr = create_vs_from_cso(device, "Data/Shaders/sprite_batch_vs.cso", &vertexShader, &inputLayout, layout, ARRAYSIZE(layout));
	assert(SUCCEEDED(hr) && "Data/Shaders/vertexShager�̓ǂݍ��ݒ��ɂ����܂����@by SpriteBatch");
	hr = create_ps_from_cso(device, "Data/Shaders/sprite_batch_ps.cso", &pixelShader);
	assert(SUCCEEDED(hr) && "Data/Shaders/pixelShader�̓ǂݍ��ݒ��ɂ����܂����@by SpriteBatch");

	instance* inst = new instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		bd.ByteWidth = sizeof(instance) * MAX_INSTANCES;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		sd.pSysMem = inst;
		sd.SysMemPitch = 0;
		sd.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&bd, &sd, &instanceBuffer);
		assert(SUCCEEDED(hr) && "�o�b�t�@�̐����Ɏ��s���� by SpriteBatch");
	}
	delete[] inst;

	//���X�^���C�U�X�e�[�g�̐���
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	assert(SUCCEEDED(hr) && "���X�^���C�U�X�e�[�g�̐����ł����� by SpriteBatch");

	//�e�N�X�`���ǂݍ���
	hr = load_texture_from_file(device, _fileName, &shaderResourceView, &tex2dDesc);
	assert(SUCCEEDED(hr) && "�e�N�X�`���ǂݍ��݂Ɏ��s���₵�� by SpriteBatch");

	//�T���v���[�X�e�[�g�̐���
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	for (int i = 0; i < 4; i++) samplerDesc.BorderColor[i] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&samplerDesc, &samplerState);
	assert(SUCCEEDED(hr) && "�T���v���[�X�e�[�g�쐬�ł����� by SpriteBatch");

	//�[�x�X�e���V���X�e�[�g�̐���
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = FALSE;;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&dsDesc, &depthStencilState);
	assert(SUCCEEDED(hr) && "�[�x�X�e���V���X�e�[�g�̍쐬�ł����� by SpriteBatch");

	//Assign a orgments to a variable
	//However,	if�u_cutOutSizeX <= 0�vor�u_cutOutSizeY <= 0�v,cutOutSize is natual texture size.
	if (_cutOutSize.x <= 0 || _cutOutSize.y <= 0)
	{
		_cutOutSize.x = static_cast<float>(tex2dDesc.Width);
		_cutOutSize.y = static_cast<float>(tex2dDesc.Height);
	}
	cutOutSize.x = _cutOutSize.x;
	cutOutSize.y = _cutOutSize.y;
	;
	float tentativeTexSizeX = static_cast<float>(tex2dDesc.Width);
	float tentativeTexSizeY = static_cast<float>(tex2dDesc.Height);
	int cutNumX = static_cast<int>(tentativeTexSizeX / cutOutSize.x);
	int cutNumY = static_cast<int>(tentativeTexSizeY / cutOutSize.y);

	int count = 0;
	for (int i = 0; i<cutNumY; i++)
	{
		for (int j = 0; j<cutNumX; j++)
		{
			adress.insert(std::make_pair(count++, DirectX::XMFLOAT2(j*cutOutSize.x, i*cutOutSize.y)));
		}
	}
}

/*--------------------------------*/
//	�f�X�g���N�^
/*--------------------------------*/
SpriteBatch::~SpriteBatch()
{
	if (vertexShader)
		vertexShader->Release();
	if (pixelShader)
		pixelShader->Release();
	if (inputLayout)
		inputLayout->Release();
	if (rasterizerState)
		rasterizerState->Release();
	if (shaderResourceView)
		shaderResourceView->Release();
	if (samplerState)
		samplerState->Release();
	if (depthStencilState)
		depthStencilState->Release();
	if (buffer)
		buffer->Release();
	if (instanceBuffer)
		instanceBuffer->Release();
	vertexShader = nullptr;
	pixelShader = nullptr;
	inputLayout = nullptr;
	rasterizerState = nullptr;
	shaderResourceView = nullptr;
	samplerState = nullptr;
	depthStencilState = nullptr;
	buffer = nullptr;
	instanceBuffer = nullptr;
}

/*--------------------------------*/
//	�`��O����
/*--------------------------------*/
void SpriteBatch::Begin()
{
	HRESULT hr = S_OK;
	ID3D11DeviceContext* deviceContext = FRAMEWORK->GetDeviceContext();

	UINT strides[2] = { sizeof(vertex),sizeof(instance) };
	UINT offsets[2] = { 0,0 };
	ID3D11Buffer* vbs[2] = { buffer,instanceBuffer };
	deviceContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	deviceContext->IASetInputLayout(inputLayout);
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);
	deviceContext->RSSetState(rasterizerState);
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);
	deviceContext->PSSetShaderResources(0, 1, &shaderResourceView);
	deviceContext->PSSetSamplers(0, 1, &samplerState);

	UINT numViewports = 1;
	deviceContext->RSGetViewports(&numViewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mappedBuffer;

	hr = deviceContext->Map(instanceBuffer, 0, map, 0, &mappedBuffer);
	assert(SUCCEEDED(hr) && "�`��O��Map�ň������������� by SpriteBatch");

	instances = static_cast<instance *>(mappedBuffer.pData);

	instanceCount = 0;
}


/*--------------------------------*/
//	�`�揈��( XMFLOAT�� )
/*--------------------------------*/
void	SpriteBatch::Draw(
	DirectX::XMFLOAT2& position, DirectX::XMFLOAT2& scale,
	DirectX::XMFLOAT2& texPos, DirectX::XMFLOAT2& texSize,
	DirectX::XMFLOAT2& center, float angle,
	DirectX::XMFLOAT4& color
	)
{
	//Begin
//	if (instanceCount == 0)
//	{
//		Begin();
//	}

	if (instanceCount >= MAX_INSTANCES)
	{
		assert("instance��MAX_INSTANCES�𒴂��Ă܂���");
		return;
	}
	if (scale.x*scale.y == 0.0f)return;

	float tw = texSize.x;
	float th = texSize.y;
	if (texSize.x <= 0.0f || texSize.y <= 0.0f)
	{
		tw = static_cast<float>(tex2dDesc.Width);
		th = static_cast<float>(tex2dDesc.Height);
	}

	float cx = center.x;
	float cy = center.y;
	cx *= scale.x;
	cy *= scale.y;

	FLOAT c = cosf(angle*0.01745f);
	FLOAT s = sinf(angle*0.01745f);
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;

	instances[instanceCount].ndcTransform._11 = w*scale.x*c;
	instances[instanceCount].ndcTransform._21 = h*scale.x*s;
	instances[instanceCount].ndcTransform._31 = 0.0f;
	instances[instanceCount].ndcTransform._41 = 0.0f;
	instances[instanceCount].ndcTransform._12 = w*scale.y*-s;
	instances[instanceCount].ndcTransform._22 = h*scale.y*c;
	instances[instanceCount].ndcTransform._32 = 0.0f;
	instances[instanceCount].ndcTransform._42 = 0.0f;
	instances[instanceCount].ndcTransform._13 = 0.0f;
	instances[instanceCount].ndcTransform._23 = 0.0f;
	instances[instanceCount].ndcTransform._33 = 1.0f;
	instances[instanceCount].ndcTransform._43 = 0.0f;
	instances[instanceCount].ndcTransform._14 = w*(-cx*c + -cy*-s + cx + position.x) - 1.0f;
	instances[instanceCount].ndcTransform._24 = h*(-cx*s + -cy*c + cy + position.y) + 1.0f;
	instances[instanceCount].ndcTransform._34 = 0.0f;
	instances[instanceCount].ndcTransform._44 = 1.0f;

	float tex_width = static_cast<float>(tex2dDesc.Width);
	float tex_height = static_cast<float>(tex2dDesc.Height);
	instances[instanceCount].texcoordTransform = DirectX::XMFLOAT4(texPos.x / tex_width, texPos.y / tex_height, tw / tex_width, th / tex_height);
	instances[instanceCount].color = color;

	++instanceCount;

	//End
//	if (instanceCount >= MAX_INSTANCES)
//	{
//		End();
//	}
}


/*--------------------------------*/
//	�`�揈��( float�� )
/*--------------------------------*/
void	SpriteBatch::Draw(
	float x,	float y,	float sx,	float sy,
	float tx,	float ty,	float tw,	float th,
	float cx,	float cy,	float angle/*radian*/,
	float r,	float g,	float b,	float a
	)
{
	Draw(
		DirectX::XMFLOAT2(x, y),	DirectX::XMFLOAT2(sx, sy),
		DirectX::XMFLOAT2(tx, ty),	DirectX::XMFLOAT2(tw, th),
		DirectX::XMFLOAT2(cx, cy),	angle, 
		DirectX::XMFLOAT4(r, g, b, a)
		);
}

/*--------------------------------*/
//	�`�揈��( �A�j���[�V�������l�����銴���̂�� )
/*--------------------------------*/
void	SpriteBatch::Draw(
	float x, float y, float sx, float sy,
	float angle,/*degree*/
	float r, float g, float b, float a,
	int cutOutAdress
	)
{
	DirectX::XMFLOAT2 texPos = adress.at(cutOutAdress);
	
	Draw(
		DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(sx, sy),
		texPos, cutOutSize,
		DirectX::XMFLOAT2((texPos.x + cutOutSize.x) / 2, (texPos.y + cutOutSize.y) / 2),
		angle,
		DirectX::XMFLOAT4(r, g, b, a)
		);
}

/*--------------------------------*/
//	�`��㏈��
/*--------------------------------*/
void SpriteBatch::End()
{
	ID3D11DeviceContext* deviceContext = FRAMEWORK->GetDeviceContext();
	deviceContext->Unmap(instanceBuffer, 0);
	deviceContext->DrawInstanced(4, instanceCount, 0, 0);
//	instanceCount = 0;
}


/**********************************************************************************************************************

	�e��֐�

**********************************************************************************************************************/
/*************************************************************************
	�摜��`�悷��֐��i�A�j���[�V�����Ή�Ver.�j
-------------------------------------------------------------------------
���� :
float			x,	y		:	�`��ʒu�̍���
float			sx,	sy		:	�`�悷��摜�̑傫��
float			angle		:	�摜�̉�]�p�x�idegree�j
float			r, g, b, a	:	�摜�̐F���̐ݒ�
int				cutOutAdress:	�X�v���C�g���̂ǂ���`�悷�邩
SpriteBatch**	sprData		:	�摜�f�[�^������|�C���^

�߂�l :
void						:	�Ȃ�
*************************************************************************/
void DrawGraph(float x, float y, float sx, float sy, float angle, int cutOutAdress, SpriteBatch* data, float r, float g, float b, float a)
{
	DirectX::XMFLOAT2 texPos = data->adress.at(cutOutAdress);

	data->Draw(
		DirectX::XMFLOAT2(x, y), DirectX::XMFLOAT2(sx, sy),
		texPos, data->cutOutSize,
		DirectX::XMFLOAT2((texPos.x + data->cutOutSize.x) / 2, (texPos.y + data->cutOutSize.y) / 2),
		angle,
		DirectX::XMFLOAT4(r, g, b, a)
		);
}
