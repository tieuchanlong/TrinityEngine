#pragma once
#include "EngineObject.h"
#include "EngineCamera.h"
#include "EngineLevelManager.h"

class Application
{
public:
	static Application* GetInstance();
	~Application();

	int InitApplication(HINSTANCE hInstance, WNDPROC WndProc, int cmdShow); // Initialize Win32 stuff
	virtual int InitUtilityClasses();
	int InitDirect3D(HINSTANCE hInstance); // Initialize Direct3D stuff
	int InitDirect3DDeviceAndSwapChain(unsigned int clientWidth, unsigned int clientHeight);
	int InitDirect3DDepthAndStencilBuffer();
	int InitDirect3DRasterizationBuffer();
	int InitViewport();
	virtual int Initialize(HINSTANCE hInstance, WNDPROC WndProc, int cmdShow); // Initialize required application variables

	int Run();
	DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vSync);

	int PreUpdate();
	int Update(int fDeltaTime);
	int OnRender(HWND hwnd);
	int OnResize(UINT width, UINT height);
	void Destroy();

	virtual void Quit();

	// Render functions
	void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
	int Present(bool vSync);

	// Some useful get functions
	ID3D11Device* GetDevice() { return m_pd3dDevice; }
	D3D11_VIEWPORT* GetViewPort() { return &m_pViewport; }
	ID3D11DepthStencilState* GetDepthStencilState() { return m_pd3dDepthStencilState; }
	ID3D11RenderTargetView** GetRenderTargetView() { return &m_pd3dRenderTargetView; }
	ID3D11DepthStencilView* GetDepthStencilView() { return m_pd3dDepthStencilView; }
	ID3D11DeviceContext* GetDeviceContext() { return m_pd3dDeviceContext; }
	ID3D11RasterizerState* GetRasterizerState() { return m_pd3dRasterizerState; }
	HWND* GetWindowHandle() { return &m_pWindowHandle; }

	// Camera management
	Camera* GetMainCamera() { return m_pCameras->GetHead()->GetValue(); }
	Camera* GetCameraByIndex(int iIndex) { return m_pCameras->GetElementAtIndex(iIndex)->GetValue(); }

	// Register object and component class
	virtual void RegisterGameClass();

	virtual EngineLevelManager* GetLevelManager() { return m_pLevelManager; }
	virtual void ReRender();

	const char* GetCurDirectory() { return m_sCurDirectory; }

	XMFLOAT3 GetBackgroundColor() { return m_vBackgroundColor; }

protected:
	static Application* instance;

	LPCSTR m_sWindowClassName = "Direct2DWindowClass";
	LPCSTR m_sWindowName = "DirectX3D Engine";
	const LONG m_iWindowWidth = 1920;
	const LONG m_iWindowHeight = 1080;
	HWND m_pWindowHandle = 0; // Handle the instance of the window created

	// Direct3D device and swap chain.
	ID3D11Device* m_pd3dDevice = nullptr;
	ID3D11DeviceContext* m_pd3dDeviceContext = nullptr;
	IDXGISwapChain* m_pd3dSwapChain = nullptr;

	// Render target view for the back buffer of the swap chain.
	ID3D11RenderTargetView* m_pd3dRenderTargetView = nullptr;
	// Depth/stencil view for use as a depth buffer.
	ID3D11DepthStencilView* m_pd3dDepthStencilView = nullptr;
	// A texture to associate to the depth stencil view.
	ID3D11Texture2D* m_pd3dDepthStencilBuffer = nullptr;

	// Define the functionality of the depth/stencil stages.
	ID3D11DepthStencilState* m_pd3dDepthStencilState = nullptr;
	// Define the functionality of the rasterizer stage.
	ID3D11RasterizerState* m_pd3dRasterizerState = nullptr;

	D3D11_VIEWPORT m_pViewport = { 0 };

	BOOL m_vSync;

	bool m_bInitialized;
	bool m_bInitializeRender;

	XMFLOAT3 m_vBackgroundColor;

	// Camera management
	List<Camera>* m_pCameras;

	// Level Manager
	EngineLevelManager* m_pLevelManager;

	char m_sCurDirectory[256] = { 0 };

	Application();
};