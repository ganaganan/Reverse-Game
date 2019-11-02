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
		cso_data.get(),		//コンパイルされたシェーダへのポインタ
		cso_sz,				//コンパイルされた頂点シェーダのサイズ
		nullptr,			//クラスリンケージインターフェースへのポインタ（nullでかまわない）
		vertexShader		//ID3I11VertexShaderへのポインタ
		);
	if (FAILED(hr))
		assert(hr);
	//④入力レイアウトオブジェクトの生成
	//CreateInputLayout
	hr = device->CreateInputLayout(
		inputElementDesc,	//入力アセンブラステージの入力データ型の配列
		numElements,		//入力用紙の配列内の入力データ型の数
		cso_data.get(),		//コンパイルされたシェーダへのポインタ
		cso_sz,				//コンパイルされたシェーダのサイズ
		inputLayout			//ID3D11InputLayoutのポインタ
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
		cso_data2.get(),	//コンパイルされたシェーダへのポインタ
		cso_sz2,			//コンパイルされたピクセルシェーダのサイズ
		nullptr,			//クラスリンケージインタフェースへのポインタ（nullでかまわない）
		pixelShader		//ID3D11PixelShagerへのポインタ
		);
	if (FAILED(hr))
		assert(hr);

	cache.insert(std::make_pair(cso_name, *pixelShader));

	return hr;
}