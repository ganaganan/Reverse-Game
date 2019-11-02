#include"texture.h"
#include"misc.h"
#include<WICTextureLoader.h>
#include<wrl.h>
#include<map>
#include<assert.h>
#include<string>
#include<sstream>

	std::map < std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;

	HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView**	shaderResourceView, D3D11_TEXTURE2D_DESC* texture2d_desc)
	{
		HRESULT hr = S_OK;
		Microsoft::WRL::ComPtr<ID3D11Resource> resource;	// TODO namespaceでくくっていくぅ

	//	static std::map < std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> cache;
		auto it = cache.find(file_name);
		if (it != cache.end())
		{
			*shaderResourceView = it->second.Get();
			(*shaderResourceView)->AddRef();
			(*shaderResourceView)->GetResource(resource.GetAddressOf());
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(device, file_name, resource.GetAddressOf(), shaderResourceView);
			if (FAILED(hr))
				assert(hr);

			cache.insert(std::make_pair(file_name, *shaderResourceView));
		}

		// 指定されたIID(Interface ID)に応じて、そのインターフェースポインタを
		// *ppvObjectに渡してやります。
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
		if (FAILED(hr))
			assert(hr);
		texture2d->GetDesc(texture2d_desc);

		return hr;
	}

	void	MakeDummyTexture(ID3D11Device* _device, ID3D11ShaderResourceView** _shaderResourceView, unsigned int _value/*0xAABBGGRR*/, unsigned int _dimensions, bool _forceSrgb, bool _enableCaching)
	{
		std::stringstream filename;
		filename << "dummyTexture" << "." << std::hex << std::uppercase << _value << "." << _dimensions << "." << _forceSrgb;

		wchar_t filename_w[256];
		MultiByteToWideChar(CP_ACP, 0, filename.str().c_str(), static_cast<int>(filename.str().length() + 1), filename_w, 256);

		std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>::iterator it = cache.find(filename_w);
		if (it != cache.end())
		{
			*_shaderResourceView = it->second.Get();
			(*_shaderResourceView)->AddRef();
		}
		else
		{
			HRESULT hr = S_OK;

			D3D11_TEXTURE2D_DESC texture2d_desc = {};
			texture2d_desc.Width = _dimensions;
			texture2d_desc.Height = _dimensions;
			texture2d_desc.MipLevels = 1;
			texture2d_desc.ArraySize = 1;
			texture2d_desc.Format = _forceSrgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
			texture2d_desc.SampleDesc.Count = 1;
			texture2d_desc.SampleDesc.Quality = 0;
			texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
			texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			texture2d_desc.CPUAccessFlags = 0;
			texture2d_desc.MiscFlags = 0;

			unsigned int *sys_mem = new unsigned int[_dimensions*_dimensions];
			for (unsigned int i = 0; i < _dimensions*_dimensions; i++)
			{
				sys_mem[i] = _value;
			}
			D3D11_SUBRESOURCE_DATA subresource_data = {};
			subresource_data.pSysMem = sys_mem;
			subresource_data.SysMemPitch = sizeof(unsigned int) * _dimensions;
			subresource_data.SysMemSlicePitch = 0;

			ID3D11Texture2D *texture2d;
			hr = _device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
			shader_resource_view_desc.Format = texture2d_desc.Format;
			shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_resource_view_desc.Texture2D.MipLevels = 1;

			hr = _device->CreateShaderResourceView(texture2d, &shader_resource_view_desc, _shaderResourceView);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if (_enableCaching)
			{
				cache.insert(std::make_pair(filename_w, *_shaderResourceView));
			}

			texture2d->Release();
			delete[] sys_mem;
		}
	}
