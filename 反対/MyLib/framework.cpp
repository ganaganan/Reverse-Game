#include "framework.h"
#include"sprite.h"
#include<wrl.h>
#include<deque>
#include<algorithm>
#include <cstdlib>
#include <ctime>
//#include "GamePad.h"
#include "GamePad.h"

 ID3D11Device* framework::device = nullptr;
 ID3D11DeviceContext* framework::deviceContext = nullptr;

 float posX = 0.0f;
 float posY = 1.0f;
 float posZ = 0.0f;

bool framework::initialize()
{
    HRESULT hr = S_OK;
    RECT rc;
    GetClientRect(hwnd, &rc);

    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    UINT createDeviceFlags = 0;
    BOOL enable_4x_msaa = TRUE;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.OutputWindow = hwnd;

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,     //���u
        D3D_DRIVER_TYPE_WARP,         //IBM��MicroSoft���J������OS
        D3D_DRIVER_TYPE_REFERENCE,    //�|�C���^�݂����Ȃ��́A�������Q�Ƃ��Ă������
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,    //�V�F�[�_�[���f��5���܂�Direct3D 11.0�ŃT�|�[�g����Ă���@�\��ΏۂƂ���B
        D3D_FEATURE_LEVEL_10_1,    //�V�F�[�_�[���f��4���܂�Direct3D 10.1�ŃT�|�[�g����Ă���@�\��ΏۂƂ���B
        D3D_FEATURE_LEVEL_10_0,    //�V�F�[�_�[���f��4���܂�Direct3D 10.0�ŃT�|�[�g����Ă���@�\��ΏۂƂ���B
    };

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);


    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {

		driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &deviceContext );
        if (SUCCEEDED(hr))      //��������������������B���������ꍇfor�����甲����
            break;
    }
	if (FAILED(hr))             //�A���[�g
		assert(hr);

	//Render Target View �̍쐬
    ID3D11Texture2D* pBackBuffer = NULL;
    D3D11_TEXTURE2D_DESC back_buffer_desc;

    hr = swapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer );
	if (FAILED(hr))    //�A���[�g
		assert(hr);

    hr = device->CreateRenderTargetView( pBackBuffer, NULL, &renderTaegetView );
    pBackBuffer->Release();
    if (FAILED(hr))
		assert(hr);

    pBackBuffer->GetDesc(&back_buffer_desc);


    //�[�x�X�e���V���o�b�t�@�p�̃e�N�X�`�����쐬�B
	D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc = back_buffer_desc;

	ID3D11Texture2D* depth_stencil_buffer;
	depth_stencil_buffer_desc.Width = width;							//����
	depth_stencil_buffer_desc.Height = height;							//�c��
    depth_stencil_buffer_desc.MipLevels = 1;                            //�e�N�X�`���[���̃~�b�v�}�b�v���x���̍ő吔�B
    depth_stencil_buffer_desc.ArraySize = 1;                            //�e�N�X�`���[�z����̃e�N�X�`���[�̐��B
    depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;   //�e�L�X�`���[�t�H�[�}�b�g�B
    depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;              //�e�N�X�`���[�̓ǂݍ��݂���я������ݕ��@�����ʂ���l�B�ł���ʓI�Ȓl��D3D11_USAGE_DEFAULT�B
    depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;     //�p�C�v���C�� �X�e�[�W�ւ̃o�C���h�Ɋւ���t���O�ł�
    depth_stencil_buffer_desc.CPUAccessFlags = 0;                       //������CPU�A�N�Z�X�̎�ނ��w�肷��t���O�B�A�N�Z�X���s�v�ȏꍇ��0���g�p����B
    depth_stencil_buffer_desc.MiscFlags = 0;                            //���̈�ʐ��̒Ⴂ���\�[�X�I�v�V���������ʂ���t���O�B�����̃t���O��K�p���Ȃ��ꍇ��0���g�p���܂��B

	hr = device->CreateTexture2D( &depth_stencil_buffer_desc, NULL, &depth_stencil_buffer );
    if (FAILED(hr))     //�A���[�g
		assert(hr);

    //�[�x�X�e���V���r���[���쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
    depth_stencil_view_desc.Format = depth_stencil_buffer_desc.Format;
    depth_stencil_view_desc.ViewDimension = enable_4x_msaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Flags = 0;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depthStencilView);
	if (FAILED(hr))     //�A���[�g
		assert(hr);

	srand((unsigned int)time(NULL)); // �����n��̕ύX

	skinMesh = std::make_unique<SkinnedMesh>(device, "sitsunai.fbx");

	InitControllers();

    return true;
}
void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	UpdateControllerState();

#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("pos");
	ImGui::SliderFloat("x", &posX, -1000.0f, 1000.0f);
	ImGui::SliderFloat("y", &posY, -1000.0f, 1000.0f);
	ImGui::SliderFloat("z", &posZ, -1000.0f, 1000.0f);
	ImGui::End();
#endif

	if (InputState(XINPUT_DPAD_UP))
	{
		int debug = 0;
	}
	if (InputState(XINPUT_DPAD_DOWN))
	{
		int debug = 0;
	}

}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui::Render();
#endif

	//�r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = SCREEN_WIDTH;
	vp.Height = SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);

#if 0
	FLOAT clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
#else
	FLOAT clearColor[4] = { 0.3f,0.3f,0.3f,0.3f };
#endif

	//�����_�[�^�[�Q�b�g�r���[���N���A����
	deviceContext->ClearRenderTargetView(renderTaegetView, clearColor);
	//�[�x�X�e���V�����\�[�X�̃N���A
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//�����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���o�b�t�@�̃o�C���h
	deviceContext->OMSetRenderTargets(1, &renderTaegetView, depthStencilView);

	/************************/
	//���[���h�ϊ��s��擾
	/************************/
	DirectX::XMMATRIX W, S, R, T, V;
	DirectX::XMFLOAT4X4	world_view_projection;
	DirectX::XMFLOAT4X4	world_m;
	DirectX::XMFLOAT4 color(1.0f, 1.0f, 1.0f, 1.0f);
	DirectX::XMFLOAT4 light(1.0f, -1.0f, 0.0f, 0.0f);
	{
		//	������
		W = DirectX::XMMatrixIdentity();
		//	�g��E�k��
		S = DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f);
		//	��]
		R = DirectX::XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		//	���s�ړ�
		T = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		//	���[���h�ϊ��s��
		W = S * R * T;

		/***************************/
		//�r���[�s���ݒ�
		/***************************/
		DirectX::XMVECTOR p = DirectX::XMVectorSet(posX, posY, posZ, 1.0f);
		DirectX::XMVECTOR t = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		V = DirectX::XMMatrixLookAtLH(p, t, up);

		/****************************/
		//	�v���W�F�N�V�����s����쐬
		/****************************/
		float	fov = DirectX::XMConvertToRadians(30.0f);
		float	aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
		DirectX::XMMATRIX	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, 0.1f, 1000.0f);
		//Matrix -> Float4x4 �ϊ�
		DirectX::XMStoreFloat4x4(&world_view_projection, W * V * projection);
		DirectX::XMStoreFloat4x4(&world_m, W);
	}

	skinMesh->Render(deviceContext, world_view_projection, world_m, light, color);

#ifdef USE_IMGUI
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	//�����_�����O���ꂽ�C���[�W�̕\��
	swapChain->Present(0, 0);
}