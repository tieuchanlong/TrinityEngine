#include <DirectX11BasePch.h>
#include "Application.h"
#include "EngineKeyboard.h"
#include "EngineMouse.h"
#include "EngineCubeComponent.h"
#include "EngineTextComponent.h"
#include "UI/EngineGUIManager.h"
#include "Event/EngineEventManager.h"
#include "Physics/EngineRigidBodyComponent.h"
#include "Physics/EngineBoundingBoxComponent.h"
#include "EngineRectangleComponent.h"
#include "ObjectSpawner.h"
#include "UI/EngineUIContainer.h"

using namespace DirectX;

Application* Application::instance = nullptr;

Application* Application::GetInstance()
{
	if (!instance)
	{
		instance = new Application();
	}

	return instance;
}

Application::~Application()
{

}

int Application::InitApplication(HINSTANCE hInstance, WNDPROC WndProc, int cmdShow)
{
	// Create window class
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = m_sWindowClassName;

	// Register the window to be used in calls CreateWindowEx function
	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	// Create the window rect
	RECT windowRect = { 0, 0, m_iWindowWidth, m_iWindowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window instance
	m_pWindowHandle = CreateWindowA(m_sWindowClassName, m_sWindowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	if (!m_pWindowHandle)
	{
		return -1;
	}

	ShowWindow(m_pWindowHandle, cmdShow);
	//UpdateWindow(p_WindowHandle);

	GetCurrentDirectory(256, m_sCurDirectory);

	return 0;
}

int Application::InitUtilityClasses()
{
	// Initialize camera
	m_pCameras = new List<Camera>();

	// Initialize level manager
	m_pLevelManager = new EngineLevelManager();

	return 0;
}

int Application::InitDirect3D(HINSTANCE hInstance)
{
	int iRet = 0;

	// A window handle must have been created already.
	Assert(m_pWindowHandle != 0);

	// Get the client rect
	RECT clientRect;
	GetClientRect(m_pWindowHandle, &clientRect);

	// Compute the exact client dimensions. This will be used
	// to initialize the render targets for our swap chain.
	unsigned int clientWidth = clientRect.right - clientRect.left;
	unsigned int clientHeight = clientRect.bottom - clientRect.top;

	iRet = InitDirect3DDeviceAndSwapChain(clientWidth, clientHeight);

	iRet = iRet | InitDirect3DDepthAndStencilBuffer();
	iRet = iRet | InitDirect3DRasterizationBuffer();
	iRet = iRet | InitViewport();

	return iRet;
}

int Application::InitDirect3DDeviceAndSwapChain(unsigned int clientWidth, unsigned int clientHeight)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// Initialize swap chain description
	swapChainDesc.BufferCount = 1; // Only have 1 back buffer
	swapChainDesc.BufferDesc.Width = clientWidth;
	swapChainDesc.BufferDesc.Height = clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate = QueryRefreshRate(clientWidth, clientHeight, m_vSync);
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = m_pWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

	UINT createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// These are the feature levels that we will accept.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// This will be the feature level that 
	// is used to create our device and swap chain.
	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
		&swapChainDesc, &m_pd3dSwapChain, &m_pd3dDevice, &featureLevel, &m_pd3dDeviceContext);

	// if the result is invalid, then create with only D3D_FEATURE_LEVEL_11_0 and later one in the array since old Windows version will not support D3D_FEATURE_LEVEL_11_1
	if (hr == E_INVALIDARG)
	{
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
			nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
			D3D11_SDK_VERSION, &swapChainDesc, &m_pd3dSwapChain, &m_pd3dDevice, &featureLevel,
			&m_pd3dDeviceContext);
	}

	// If entirely failed, return -1, means that the initialization failed
	if (FAILED(hr))
	{
		return -1;
	}

	// Next initialize the back buffer of the swap chain and retrieve a pointer to swap chain single back buffer and assigned to variable backBuffer to be used to retrieve the render target view
	ID3D11Texture2D* backBuffer;
	hr = m_pd3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	if (FAILED(hr))
	{
		return -1;
	}

	// Create render target view using back  buffer resources
	hr = m_pd3dDevice->CreateRenderTargetView(backBuffer, NULL, &m_pd3dRenderTargetView);

	if (FAILED(hr))
	{
		return -1;
	}

	SafeRelease(backBuffer);

	return hr;
}

int Application::InitDirect3DDepthAndStencilBuffer()
{
	HRESULT hr = S_OK;

	RECT clientRect;
	GetClientRect(m_pWindowHandle, &clientRect);
	unsigned int clientWidth = clientRect.right - clientRect.left;
	unsigned int clientHeight = clientRect.bottom - clientRect.top;

	// Now we need to create depth buffer to make sure far object is not rendered over the near object
	// Create the depth buffer for use with the depth/stencil view.
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = clientWidth;  // Texture Height
	depthStencilBufferDesc.Height = clientHeight;  // Texture width
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = m_pd3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &m_pd3dDepthStencilBuffer);
	if (FAILED(hr))
	{
		return -1;
	}

	// Create a depth stencil view using the depth stencil buffer
	hr |= m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, NULL, &m_pd3dDepthStencilView);

	if (FAILED(hr))
	{
		return -1;
	}

	// Setup depth/stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = TRUE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	hr |= m_pd3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &m_pd3dDepthStencilState);

	return hr;
}

int Application::InitDirect3DRasterizationBuffer()
{
	// Set up the rasterizer state object
	// Setup rasterizer state object
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state object.
	HRESULT hr = m_pd3dDevice->CreateRasterizerState(&rasterizerDesc, &m_pd3dRasterizerState);
	
	return hr;
}

int Application::InitViewport()
{
	int iRet = 0;

	RECT clientRect;
	GetClientRect(m_pWindowHandle, &clientRect);
	unsigned int clientWidth = clientRect.right - clientRect.left;
	unsigned int clientHeight = clientRect.bottom - clientRect.top;

	// Can create 2 view ports for split screen
	m_pViewport.Width = static_cast<float>(clientWidth);
	m_pViewport.Height = static_cast<float>(clientHeight);
	m_pViewport.TopLeftX = 0.0f;
	m_pViewport.TopLeftY = 0.0f;
	m_pViewport.MinDepth = 0.0f;
	m_pViewport.MaxDepth = 1.0f;

	return iRet;
}

int Application::Initialize(HINSTANCE hInstance, WNDPROC WndProc, int cmdShow)
{
	int iRet = InitApplication(hInstance, WndProc, cmdShow);
	iRet |= InitUtilityClasses();
	iRet |= InitDirect3D(hInstance); // initialize direct 3D first

	XMStoreFloat3(&m_vBackgroundColor, Colors::Gray);

	// Initialize Event Manager
	EngineEventManager::GetInstance()->Initialize();

	// Initialize Camera
	m_pCameras->Add(new Camera());

	// Initialize keyboard
	EngineKeyboard::GetInstance()->Initialize();

	// Initialize Mouse
	EngineMouse::GetInstance()->Initialize();

	// Initialize levels
	m_pLevelManager->Initialize();

	// Initialize UI
	EngineGUIManager::GetInstance()->Initialize();

	m_bInitialized = true;

	return iRet;
}

int Application::Run()
{
	// Create message instance to handle the user feedback
	MSG msg = { 0 };

	static DWORD previousTime = timeGetTime();
	static const float targetFramerate = 30.0f;
	static const float maxTimeStep = 1.0f / targetFramerate;

	// If the user didn't send the quit message, keep updating and rendering
	while (msg.message != WM_QUIT)
	{
		// If not having the middle part to handle the message queue, the application will keep hanging
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - previousTime)/* / 1000.0f*/;

			if (deltaTime <= 0.f)
			{
				continue;
			}

			previousTime = currentTime;

			PreUpdate();

			if (!m_bInitializeRender)
			{
				m_bInitializeRender = true;
				ReRender();
			}

			// Do everything you like during the update here
			Update(deltaTime);
		}
	}

	return static_cast<int>(msg.wParam);
}

DXGI_RATIONAL Application::QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vSync)
{
	DXGI_RATIONAL refreshRate = { 0, 1 };

	if (vSync)
	{
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		DXGI_MODE_DESC* displayModeList;

		// Create a DirectX graphics interface factory.
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Could not create DXGIFactory instance."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to create DXGIFactory.");
		}

		hr = factory->EnumAdapters(0, &adapter);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapters."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapters.");
		}

		hr = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapter outputs."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapter outputs.");
		}

		UINT numDisplayModes;
		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		displayModeList = new DXGI_MODE_DESC[numDisplayModes];
		Assert(displayModeList);

		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list.");
		}

		// Now store the refresh rate of the monitor that matches the width and height of the requested screen.
		for (UINT i = 0; i < numDisplayModes; ++i)
		{
			if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
			{
				refreshRate = displayModeList[i].RefreshRate;
			}
		}

		delete[] displayModeList;
		SafeRelease(adapterOutput);
		SafeRelease(adapter);
		SafeRelease(factory);
	}

	return refreshRate;
}

int Application::PreUpdate()
{
	int iRet = 0;

	EngineGUIManager::GetInstance()->PreUpdate();
	m_pLevelManager->PreUpdate();

	return iRet;
}

int Application::Update(int fDeltaTime)
{
	int iRet = 0;

	// Update keyboard
	EngineKeyboard::GetInstance()->Update(fDeltaTime);

	// Update Mouse
	EngineMouse::GetInstance()->Update(fDeltaTime);

	// Update events
	EngineEventManager::GetInstance()->Update(fDeltaTime);

	// Update camera
	for (Iterator<Camera>* it = m_pCameras->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Update(fDeltaTime);
	}

	// Update UI
	EngineGUIManager::GetInstance()->Update(fDeltaTime);

	// Update levels
	m_pLevelManager->Update(fDeltaTime);

	return iRet;
}

int Application::OnRender(HWND hwnd)
{
	if (!m_bInitialized)
	{
		return -1;
	}

	PAINTSTRUCT ps; // Contains info about the application, and used to paint the client area of a window
	HRESULT hr = S_OK;

	if (BeginPaint(hwnd, &ps))
	{
		// Check if device and device context are available
		assert(m_pd3dDevice);
		assert(m_pd3dDeviceContext);

		// Clear the screen first
		Clear(Colors::Gray, 1.0f, 0);

		// Render UIs
		EngineGUIManager::GetInstance()->Render();

		// Render levels
		m_pLevelManager->Render();

		Present(m_vSync);
		EndPaint(hwnd, &ps); // Paint and clear the background content
	}

	return hr;
}

int Application::OnResize(UINT width, UINT height)
{
	return 0;
}

void Application::Destroy()
{
	// Cleanup keyboard
	EngineKeyboard::GetInstance()->Destroy();

	// Cleanup Mouse
	EngineMouse::GetInstance()->Destroy();

	// Switch to windowed mode
	m_pd3dSwapChain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	// Clean up Direct3D components
	SafeRelease(m_pd3dDepthStencilView);
	SafeRelease(m_pd3dRenderTargetView);
	SafeRelease(m_pd3dDepthStencilBuffer);
	SafeRelease(m_pd3dDepthStencilState);
	SafeRelease(m_pd3dRasterizerState);
	SafeRelease(m_pd3dSwapChain);
	SafeRelease(m_pd3dDeviceContext);
	SafeRelease(m_pd3dDevice);

	// Clean up events
	EngineEventManager::GetInstance()->Destroy();

	// Clean up UIs
	EngineGUIManager::GetInstance()->Destroy();

	// Clean up levels
	m_pLevelManager->Destroy();
}

void Application::Quit()
{
	DestroyWindow(m_pWindowHandle);
}

void Application::Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, clearColor);
	m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

int Application::Present(bool vSync)
{
	HRESULT hr;

	// Tell the swap chain to present the content of the back bugger ons screen
	if (vSync)
	{
		hr = m_pd3dSwapChain->Present(1, 0);
	}
	else
	{
		hr = m_pd3dSwapChain->Present(0, 0);
	}

	return hr;
}

void Application::RegisterGameClass()
{
	Object::RegisterGameClass();
	Component::RegisterGameClass();
	Level::RegisterGameClass();
	EngineUIContainer::RegisterGameClass();

	EngineRenderComponent::RegisterGameClass();
	EngineCubeComponent::RegisterGameClass();
	EngineRectangleComponent::RegisterGameClass();
	EngineTextComponent::RegisterGameClass();
	EngineRigidComponent::RegisterGameClass();
	EngineBoundingBoxComponent::RegisterGameClass();
	ObjectSpawner::RegisterGameClass();
}

void Application::ReRender()
{
	InvalidateRect(*Application::GetInstance()->GetWindowHandle(), NULL, FALSE);
}

Application::Application()
{
	m_bInitialized = false;
	m_bInitializeRender = false;
}
