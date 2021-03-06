#include "framework.h"
#include"sprite.h"
#include<wrl.h>
#include<deque>
#include<algorithm>
#include <cstdlib>
#include <ctime>
#include "GamePad.h"

#include "../Source/Scene.h"

 ID3D11Device* framework::device = nullptr;
 ID3D11DeviceContext* framework::deviceContext = nullptr;

 int framework::run()
 {
	 MSG msg = {};

	 if (!initialize()) return 0;
#ifdef USE_IMGUI
	 IMGUI_CHECKVERSION();
	 ImGui::CreateContext();

	 ImGui_ImplWin32_Init(hwnd);
	 ImGui_ImplDX11_Init(device, deviceContext);
	 //ImGui::StyleColorsClassic();
	 //ImGui::StyleColorsLight();
	 ImGui::StyleColorsDark();

	 ImGuiIO& io = ImGui::GetIO();
	 //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	 io.Fonts->AddFontFromFileTTF("ImGui\\consolab.ttf", 10.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	 //io.Fonts->AddFontFromFileTTF(".\\Inconsolata-Bold.ttf", 12.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
#endif
	 while (WM_QUIT != msg.message)
	 {
		 if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		 {
			 TranslateMessage(&msg);
			 DispatchMessage(&msg);
		 }
		 else
		 {
			 timer.tick();
			 calculate_frame_stats();
			 update(timer.time_interval());
			 render(timer.time_interval());
		 }
	 }

	 //		SceneManager::Get().nowScene->Uninit();
	 		SceneManager::Get().Uninit();
#ifdef USE_IMGUI
	 ImGui_ImplDX11_Shutdown();
	 ImGui_ImplWin32_Shutdown();
	 ImGui::DestroyContext();
#endif
	 return static_cast<int>(msg.wParam);
 }

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
        D3D_DRIVER_TYPE_HARDWARE,     //装置
        D3D_DRIVER_TYPE_WARP,         //IBMとMicroSoftが開発したOS
        D3D_DRIVER_TYPE_REFERENCE,    //ポインタみたいなもの、何かを参照しているもの
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,    //シェーダーモデル5を含むDirect3D 11.0でサポートされている機能を対象とする。
        D3D_FEATURE_LEVEL_10_1,    //シェーダーモデル4を含むDirect3D 10.1でサポートされている機能を対象とする。
        D3D_FEATURE_LEVEL_10_0,    //シェーダーモデル4を含むDirect3D 10.0でサポートされている機能を対象とする。
    };

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);


    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {

		driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevel, &deviceContext );
        if (SUCCEEDED(hr))      //生成が成功したか判定。成功した場合for文から抜ける
            break;
    }
	if (FAILED(hr))             //アラート
		assert(hr);

	//Render Target View の作成
    ID3D11Texture2D* pBackBuffer = NULL;
    D3D11_TEXTURE2D_DESC back_buffer_desc;

    hr = swapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer );
	if (FAILED(hr))    //アラート
		assert(hr);

    hr = device->CreateRenderTargetView( pBackBuffer, NULL, &renderTaegetView );
    pBackBuffer->Release();
    if (FAILED(hr))
		assert(hr);

    pBackBuffer->GetDesc(&back_buffer_desc);


    //深度ステンシルバッファ用のテクスチャを作成。
	D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc = back_buffer_desc;

	ID3D11Texture2D* depth_stencil_buffer;
	depth_stencil_buffer_desc.Width = width;							//横幅
	depth_stencil_buffer_desc.Height = height;							//縦幅
    depth_stencil_buffer_desc.MipLevels = 1;                            //テクスチャー内のミップマップレベルの最大数。
    depth_stencil_buffer_desc.ArraySize = 1;                            //テクスチャー配列内のテクスチャーの数。
    depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;   //テキスチャーフォーマット。
    depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;              //テクスチャーの読み込みおよび書き込み方法を識別する値。最も一般的な値はD3D11_USAGE_DEFAULT。
    depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;     //パイプライン ステージへのバインドに関するフラグです
    depth_stencil_buffer_desc.CPUAccessFlags = 0;                       //許可するCPUアクセスの種類を指定するフラグ。アクセスが不要な場合は0を使用する。
    depth_stencil_buffer_desc.MiscFlags = 0;                            //他の一般性の低いリソースオプションを識別するフラグ。これらのフラグを適用しない場合は0を使用します。

	hr = device->CreateTexture2D( &depth_stencil_buffer_desc, NULL, &depth_stencil_buffer );
    if (FAILED(hr))     //アラート
		assert(hr);

    //深度ステンシルビューを作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
    depth_stencil_view_desc.Format = depth_stencil_buffer_desc.Format;
    depth_stencil_view_desc.ViewDimension = enable_4x_msaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Flags = 0;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, &depthStencilView);
	if (FAILED(hr))     //アラート
		assert(hr);

	srand((unsigned int)time(NULL)); // 乱数系列の変更
	InitControllers();

	SceneManager::Get().Init();

    return true;
}
void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	UpdateControllerState();

#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

	SceneManager::Get().Update();

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

	static Blender b(device);

	//ビューポートの設定
	D3D11_VIEWPORT vp;
	vp.Width = SCREEN_WIDTH;
	vp.Height = SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);

#if 1
	FLOAT clearColor[4] = { 0.0f,0.0f,0.0f,0.0f };
#else
	FLOAT clearColor[4] = { 0.3f,0.3f,0.3f,0.3f };
#endif

	//レンダーターゲットビューをクリアする
	deviceContext->ClearRenderTargetView(renderTaegetView, clearColor);
	//深度ステンシルリソースのクリア
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//レンダーターゲットと深度ステンシルバッファのバインド
	deviceContext->OMSetRenderTargets(1, &renderTaegetView, depthStencilView);

	SceneManager::Get().Render();

#ifdef USE_IMGUI
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	//レンダリングされたイメージの表示
	swapChain->Present(1, 0);
}