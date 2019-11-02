#include"Polygon.h"
#include<stdio.h>
#include<memory>
#include"shader.h"


Box::Box(ID3D11Device* _device)
{
	//�@���_���̃Z�b�g
	vertex virtices[] = {
		{ DirectX::XMFLOAT3(-0.5,+0.5,0),DirectX::XMFLOAT4(1,1,1,1) },
		{ DirectX::XMFLOAT3(+0.5,+0.5,0),DirectX::XMFLOAT4(1,0,0,1) },
		{ DirectX::XMFLOAT3(-0.5,-0.5,0),DirectX::XMFLOAT4(0,1,0,1) },
		{ DirectX::XMFLOAT3(+0.5,-0.5,0),DirectX::XMFLOAT4(0,0,1,1) },
	};

	//�A���_�o�b�t�@�I�u�W�F�N�g�̐���

	//TODO : 3-2 sprite�N���X�̃R���X�g���N�^�Œ��_�o�b�t�@�I�u�W�F�N�g�̐������@��ύX����
	//GPU�i�ǂݎ��̂݁j��CPU�i�������݂̂݁j�ɂ��A�N�Z�X���\�ɂ���
	//�o�b�t�@���\�[�X���L�q����
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(virtices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//�������݂̂݃A�N�Z�X���\�ɂ���
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	//�������������ݒ肷�邽�߂̃f�[�^���w�肵�܂�
	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = virtices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	//�o�b�t�@�i���_�o�b�t�@�A�C���f�b�N�X�o�b�t�@�A�܂��̓V�F�[�_�萔�o�b�t�@�j���쐬���܂�
	HRESULT hr = S_OK;
	hr = _device->CreateBuffer(&bufferDesc, &subResourceData, &vertexBuffer);
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
	};

	//CSO�t�@�C�������[�h����
#if 0 
	FILE* fp = nullptr;
	fopen_s(&fp, "sprite_vs.cso", "rb");
	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	//CreateVertexShader methot
	hr = _device->CreateVertexShader(
		cso_data.get(),		//�R���p�C�����ꂽ�V�F�[�_�ւ̃|�C���^
		cso_sz,				//�R���p�C�����ꂽ���_�V�F�[�_�̃T�C�Y
		nullptr,			//�N���X�����P�[�W�C���^�[�t�F�[�X�ւ̃|�C���^�inull�ł��܂�Ȃ��j
		&vertexShader		//ID3I11VertexShader�ւ̃|�C���^
		);
	if (FAILED(hr))
		assert(hr);
	//�C���̓��C�A�E�g�I�u�W�F�N�g�̐���
	//CreateInputLayout
	hr = _device->CreateInputLayout(
		input_element_desc,				//���̓A�Z���u���X�e�[�W�̓��̓f�[�^�^�̔z��
		ARRAYSIZE(input_element_desc),	//���͗p���̔z����̓��̓f�[�^�^�̐�
		cso_data.get(),					//�R���p�C�����ꂽ�V�F�[�_�ւ̃|�C���^
		cso_sz,							//�R���p�C�����ꂽ�V�F�[�_�̃T�C�Y
		&inputLayout					//ID3D11InputLayout�̃|�C���^
		);
	if (FAILED(hr))
		assert(hr);

	//�D�s�N�Z���V�F�[�_�I�u�W�F�N�g�̐���
	//sprite_ps�����[�h
	FILE* fp2 = nullptr;
	fopen_s(&fp2, "sprite_ps.cso", "rb");

	fseek(fp2, 0, SEEK_END);
	long cso_sz2 = ftell(fp2);
	fseek(fp2, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data2 = std::make_unique<unsigned char[]>(cso_sz2);
	fread(cso_data2.get(), cso_sz2, 1, fp2);
	fclose(fp2);

	//CreatePixelShader methot
	hr = _device->CreatePixelShader(
		cso_data2.get(),	//�R���p�C�����ꂽ�V�F�[�_�ւ̃|�C���^
		cso_sz2,			//�R���p�C�����ꂽ�s�N�Z���V�F�[�_�̃T�C�Y
		nullptr,			//�N���X�����P�[�W�C���^�t�F�[�X�ւ̃|�C���^�inull�ł��܂�Ȃ��j
		&pixelShader		//ID3D11PixelShager�ւ̃|�C���^
		);
	if (FAILED(hr))
		assert(hr);
#else
	create_vs_from_cso(_device, "Polygon_vs.cso", &vertexShader, &inputLayout, input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(_device, "Polygon_ps.cso", &pixelShader);
#endif
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
	hr = _device->CreateRasterizerState(&rasterizer_desc, &rasterizerState);
	if (FAILED(hr))
		assert(hr);
}

void Box::Render(ID3D11DeviceContext* _deviceContext, float dx, float dy, float dw, float dh, float angle, float r, float g, float b, float a)
{
	// dx, dy : Coordinate of sprite's left-top corner in screen space 
	// dw, dh : Size of sprite in screen space 
	// angle : Raotation angle (Rotation centre is sprite's centre), unit is degree
	// r, g, b : Color of sprite's each vertices 

	D3D11_VIEWPORT viewport;
	UINT numViewport = 1;
	_deviceContext->RSGetViewports(&numViewport, &viewport);

	float screen_width = viewport.Width;
	float screen_height = viewport.Height;

	float lt_x = dx - dw;	//����
	float lt_y = dy - dh;

	float rt_x = dx + dw;	//�E��
	float rt_y = dy - dh;

	float lb_x = dx - dw;	//����
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
	hr = _deviceContext->Map(
		vertexBuffer,	//ID3D11Resource�ւ̃|�C���^
		0,				//�����\�[�X�̃C���f�b�N�X�ԍ�
		map,			//���\�[�X�ɑ΂���CPU�̓ǂݎ�肨��я������݌������w�肷��D3D11_MAP�^�C�v�̒l
		0,				//GPU���r�W�[�̂Ƃ���CPU���������邩���w�肷��t���O
		&mapped_buffer	//�}�b�v���ꂽ�T�u���\�[�X��D3D11_MAPPED_SUBRESOURCE�\���̂ւ̃|�C���^
		);
	if (FAILED(hr))
		return;

	//vertices�ɓ����
	vertex* vertices = static_cast<vertex *>(mapped_buffer.pData);
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

	_deviceContext->Unmap(vertexBuffer, 0);


	UINT stride = sizeof(vertex);
	UINT offset = 0;
	//�@���_�o�b�t�@�[�̃o�C���h
	_deviceContext->IASetVertexBuffers(
		0,				//�o�C���f�B�O�p�̍ŏ��̓��̓X���b�g
		1,				//�z����̒��_�o�b�t�@�̐�
		&vertexBuffer,	//���_�o�b�t�@�̔z��ւ̃|�C���^
		&stride,		//�X�g���C�h�l�̔z��̃|�C���^
		&offset			//�I�t�Z�b�g�l�ւ̃|�C���^
		);

	//�A�v���~�e�B�u�^�C�v�y�уf�[�^�̏����Ɋւ�����̃o�C���h
	_deviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP	//�v���~�e�B�u�̎�ނƃv���~�e�B�u�f�[�^�̏���
		);

	//�B���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h
	_deviceContext->IASetInputLayout(
		inputLayout		//ID3D11InputLayout�̃|�C���^
		);

	//�C���X�^���C�U�[�X�e�[�g�̐ݒ�
	_deviceContext->RSSetState(
		rasterizerState	//�p�C�v���C���Ƀo�C���h���邽�߂̃��X�^���C�U��ԃC���^�t�F�[�X�ւ̃|�C���^
		);

	//�D�V�F�[�_�̐ݒ�
	_deviceContext->VSSetShader(
		vertexShader,	//���_�V�F�[�_�ւ̃|�C���^
		nullptr,		//�N���X�C���X�^���X�C���^�[�t�F�[�X�̔z��ւ̃|�C���^�B�V�F�[�_���C���^�[�t�F�[�X���g�p���Ȃ��ꍇ��NULL
		0				//�z����̃N���X�C���X�^���X�C���^�[�t�F�[�X�̐�
		);
	_deviceContext->PSSetShader(
		pixelShader,	//�s�N�Z���V�F�[�_�ւ̃|�C���^
		nullptr,		//�N���X�C���X�^���X�C���^�[�t�F�[�X�̔z��ւ̃|�C���^�B�V�F�[�_���C���^�[�t�F�[�X���g�p���Ȃ��ꍇ��NULL
		0				//�z����̃N���X�C���X�^���X�C���^�[�t�F�[�X�̐�
		);

	//�E�v���~�e�B�u�̕`��
	_deviceContext->Draw(
		4,	//�`�悷�钸�_�̐�
		0	//�ŏ��̒��_�̃C���f�b�N�X
		);
}