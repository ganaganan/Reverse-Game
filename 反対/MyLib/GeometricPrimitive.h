#ifndef _GEOMETRIC_PRIMITIVE_H_
#define _GEOMETRIC_PRIMITIVE_H_

#include<DirectXMath.h>
#include<d3d11.h>
#include <array>

//#define USE_OLD_CLASS

/******************************************
	Geometric Primitive
******************************************/
#ifdef USE_OLD_CLASS
class geometric_primitive
{
public:
	/******************************
	頂点フォーマット
	*******************************/
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	/******************************
	定数バッファ
	******************************/
	struct cbuffer
	{
		DirectX::XMFLOAT4X4 world_view_projection;	//ワールド・ビュー・プロジェクション合成行列
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4 light_direction;
	};
public:
	//頂点シェーダー
	ID3D11VertexShader*			vertexShader;
	//ピクセルシェーダー
	ID3D11PixelShader*			pixelShader;
	//入力レイアウト
	ID3D11InputLayout*			inputLayout;
	//頂点バッファ
	ID3D11Buffer*				vertexBuffer;
	//インデックスバッファ
	ID3D11Buffer*				indexBuffer;
	//定数バッファ
	ID3D11Buffer*				constantBuffer;
	//線描画
	ID3D11RasterizerState*		renderWire;
	//塗りつぶし
	ID3D11RasterizerState*		renderSolid;
	//深度ステンシルステート
	ID3D11DepthStencilState*	depthStencilState;

	geometric_primitive(ID3D11Device*);
	~geometric_primitive()
	{
		if (vertexShader)
			vertexShader->Release();
		if (pixelShader)
			pixelShader->Release();
		if (inputLayout)
			inputLayout->Release();
		if (vertexBuffer)
			vertexBuffer->Release();
		if (indexBuffer)
			indexBuffer->Release();
		if (constantBuffer)
			constantBuffer->Release();
		if (renderWire)
			renderWire->Release();
		if (renderSolid)
			renderSolid->Release();
		if (depthStencilState)
			depthStencilState->Release();
		vertexShader = nullptr;
		pixelShader = nullptr;
		inputLayout = nullptr;
		vertexBuffer = nullptr;
		indexBuffer = nullptr;
		constantBuffer = nullptr;
		renderWire = nullptr;
		renderSolid = nullptr;
		depthStencilState = nullptr;
	}

public:
	void						Render(
		ID3D11DeviceContext*		_deviceContext,	//deviceContext
		const DirectX::XMFLOAT4X4 &	_wvp,			//world view projection合成行列
		const DirectX::XMFLOAT4X4 &	_world,			//world 変換座標
		const DirectX::XMFLOAT4 &	_color,			//材質color
		const DirectX::XMFLOAT4 &	_light,			//ライトの方向
		bool						bSolid			//線・塗りつぶしrender flag
		);

protected:
	void						CreateBuffers(
		ID3D11Device*	_device,		//ID3D11Deviceのポインタ
		vertex*			_vertices,		//vertex配列の先頭アドレス
		int				_numVertices,	//頂点の数
		u_int*			_indices,		//indicesの先頭アドレス
		int				_numIndices		//indexの数
		);

};

#else

class GeometricPrimitive
{
protected:
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 normal;
	};

	struct Cbuffer
	{
		DirectX::XMFLOAT4X4 wvp;
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4	materialColor;
		DirectX::XMFLOAT4	lightDirection;
	};

	ID3D11VertexShader*			vertexShader;
	ID3D11PixelShader*			pixelShader;
	ID3D11InputLayout*			inputLayout;
	ID3D11Buffer*				vertexBuffer;
	ID3D11Buffer*				indexBuffer;
	ID3D11Buffer*				constantBuffer;
	ID3D11RasterizerState*		wireframeRasterizerState;
	ID3D11RasterizerState*		solidRasterizerState;
	ID3D11DepthStencilState*	depthStencilState;

	int							numIndices;
	bool						isMakeBuffer;
	bool						isUseTexture;

	bool	CreateBuffers(ID3D11Device* _device, Vertex* _vertices, int _numV, unsigned int* _indices, int _numI);

	void	Init(ID3D11Device* _device, const char* _vsname, D3D11_INPUT_ELEMENT_DESC* _inputElements, int _numElement, const char* psName);

protected:
	GeometricPrimitive() :
		vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr),vertexBuffer(nullptr),indexBuffer(nullptr),constantBuffer(nullptr),wireframeRasterizerState(nullptr),solidRasterizerState(nullptr),depthStencilState(nullptr),numIndices(0),isMakeBuffer(false)
	{}
	GeometricPrimitive(ID3D11Device* _device, bool _isUseTexture = false) :
		vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr),vertexBuffer(nullptr),indexBuffer(nullptr),constantBuffer(nullptr),wireframeRasterizerState(nullptr),solidRasterizerState(nullptr),depthStencilState(nullptr),numIndices(0),isMakeBuffer(false)
	{
		const char* vsName = "./Data/Shader/geometric_primitive_vs.cso";
		const char* psName = "./Data/Shader/geometric_primitive_ps.cso";

		
		// 入力レイアウトの定義
		if (isUseTexture)
		{
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                         D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
				// TEXTUREを追加する
			};
			//	初期化
			Init(_device, vsName, layout, ARRAYSIZE(layout), psName);
		}
		else
		{
			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                         D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			//	初期化
			Init(_device, vsName, layout, ARRAYSIZE(layout), psName);
		}
	}

	virtual ~GeometricPrimitive();

	virtual void Render(ID3D11DeviceContext* context,
		const DirectX::XMFLOAT4X4& wvp,
		const DirectX::XMFLOAT4X4& world,
		const DirectX::XMFLOAT4& lightDirection,
		const DirectX::XMFLOAT4& materialColor = DirectX::XMFLOAT4(1, 1, 1, 1),
		bool bSolid = true
	);
};

class GeometricRect : public GeometricPrimitive
{
public:
	GeometricRect(ID3D11Device* _device, bool _isUseTexture = false);
	~GeometricRect() {};
};

#endif

#endif //!_GEOMETRIC_PRIMITIVE_H_