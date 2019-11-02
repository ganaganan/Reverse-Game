#include"shader.h"
#include"misc.h"
#include<memory>
#include<memory>
#include<map>
#include<wrl.h>
#include<assert.h>

HRESULT create_vs_from_cso(ID3D11Device *device, const char *cso_name, ID3D11VertexShader **vertexShader, ID3D11InputLayout **inputLayout, D3D11_INPUT_ELEMENT_DESC *inputElementDesc, UINT numElements)
{
	HRESULT hr = S_OK;
	struct set_of_vertex_shader_and_input_layout
	{
		set_of_vertex_shader_and_input_layout(ID3D11VertexShader  *vertex_shader, ID3D11InputLayout *input_layout) : vertex_shader(vertex_shader), input_layout(input_layout) {}
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	};
	static std::map<std::string, set_of_vertex_shader_and_input_layout> cache;

	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*vertexShader = it->second.vertex_shader.Get();
		(*vertexShader)->AddRef();
		*inputLayout = it->second.input_layout.Get();
		(*inputLayout)->AddRef();
		return S_OK;
	}

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	//CreateVertexShader methot
	hr = device->CreateVertexShader(
		cso_data.get(),		//�R���p�C�����ꂽ�V�F�[�_�ւ̃|�C���^
		cso_sz,				//�R���p�C�����ꂽ���_�V�F�[�_�̃T�C�Y
		nullptr,			//�N���X�����P�[�W�C���^�[�t�F�[�X�ւ̃|�C���^�inull�ł��܂�Ȃ��j
		vertexShader		//ID3I11VertexShader�ւ̃|�C���^
		);
	if (FAILED(hr))
		assert(hr);
	//�C���̓��C�A�E�g�I�u�W�F�N�g�̐���
	//CreateInputLayout
	hr = device->CreateInputLayout(
		inputElementDesc,	//���̓A�Z���u���X�e�[�W�̓��̓f�[�^�^�̔z��
		numElements,		//���͗p���̔z����̓��̓f�[�^�^�̐�
		cso_data.get(),		//�R���p�C�����ꂽ�V�F�[�_�ւ̃|�C���^
		cso_sz,				//�R���p�C�����ꂽ�V�F�[�_�̃T�C�Y
		inputLayout			//ID3D11InputLayout�̃|�C���^
		);
	if (FAILED(hr))
		assert(hr);

	cache.insert(std::make_pair(cso_name, set_of_vertex_shader_and_input_layout(*vertexShader, *inputLayout)));

	return hr;
}

HRESULT create_ps_from_cso(ID3D11Device *device, const char *cso_name, ID3D11PixelShader **pixelShader)
{
	HRESULT hr = S_OK;

	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{
		*pixelShader = it->second.Get();
		(*pixelShader)->AddRef();
		return S_OK;
	}

	FILE* fp2 = nullptr;
	fopen_s(&fp2, cso_name, "rb");

	fseek(fp2, 0, SEEK_END);
	long cso_sz2 = ftell(fp2);
	fseek(fp2, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data2 = std::make_unique<unsigned char[]>(cso_sz2);
	fread(cso_data2.get(), cso_sz2, 1, fp2);
	fclose(fp2);

	//CreatePixelShader methot
	hr = device->CreatePixelShader(
		cso_data2.get(),	//�R���p�C�����ꂽ�V�F�[�_�ւ̃|�C���^
		cso_sz2,			//�R���p�C�����ꂽ�s�N�Z���V�F�[�_�̃T�C�Y
		nullptr,			//�N���X�����P�[�W�C���^�t�F�[�X�ւ̃|�C���^�inull�ł��܂�Ȃ��j
		pixelShader		//ID3D11PixelShager�ւ̃|�C���^
		);
	if (FAILED(hr))
		assert(hr);

	cache.insert(std::make_pair(cso_name, *pixelShader));

	return hr;
}