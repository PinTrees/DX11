#include "pch.h"
#include "App.h"
#include <WindowsX.h>
#include <sstream>

#include "SceneHierachyEditorWindow.h"
#include "SceneEditorWindow.h"
#include "ProjectEditorWindow.h"
#include "InspectorEditorWindow.h"

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	App* gApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gApp->MsgProc(hwnd, msg, wParam, lParam);
}

App::App(HINSTANCE hInstance)
{
	ZeroMemory(&_viewport, sizeof(D3D11_VIEWPORT));

	// Get a pointer to the application object so we can forward 
	// Windows messages to the object's window procedure through
	// the global window procedure.
	Application::GetI()->SetApp(this);
	gApp = this;
}

App::~App()
{
	if (_deviceContext)
		_deviceContext->ClearState();
}

int32 App::Run()
{
	MSG msg = {0};
 
	_timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
        {	
			_timer.Tick();
			TimeManager::GetI()->Update();

			if (!_appPaused)
			{
				CalculateFrameStats();

				// Update
				UpdateScene(_timer.DeltaTime());	
				SceneManager::GetI()->UpdateScene();
				PhysicsManager::GetI()->Update(_timer.DeltaTime());
				EditorGUIManager::GetI()->Update();

				// Render
				RenderApplication();
				EditorGUIManager::GetI()->RenderEditorWindows();

				HR(_swapChain->Present(0, 0)); 
			}
			else
			{
				::Sleep(100);
			}
        }
    }

	return (int)msg.wParam;
}

bool App::Init()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	EditorGUIManager::GetI()->Init();
	EditorGUIManager::GetI()->RegisterWindow(new SceneHierachyEditorWindow);
	EditorGUIManager::GetI()->RegisterWindow(new ProjectEditorWindow);
	EditorGUIManager::GetI()->RegisterWindow(new SceneEditorWindow);
	EditorGUIManager::GetI()->RegisterWindow(new InspectorEditorWindow);

	// Singleton Init
	SceneManager::GetI()->Init();
	SceneManager::GetI()->LoadScene("");

	// TimeManager Init
	TimeManager::GetI()->Init();

	return true;
}
 
void App::OnResize()
{
	assert(_deviceContext);
	assert(_device);
	assert(_swapChain);

	CreateRenderTargetView();
	CreateDepthStencilView();

	// Bind the render target view and depth/stencil view to the pipeline.`
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());

	// Set the viewport transform.
	_viewport.TopLeftX = 0;
	_viewport.TopLeftY = 0;
	_viewport.Width    = static_cast<float>(_clientWidth);
	_viewport.Height   = static_cast<float>(_clientHeight);
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;

	_deviceContext->RSSetViewports(1, &_viewport);
}

void App::RenderApplication()
{

}
 
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;

	switch( msg )
	{
	// WM_ACTIVATE is sent when the window is activated or deactivated.  
	// We pause the game when the window is deactivated and unpause it 
	// when it becomes active.  
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
		{
			_appPaused = true;
			_timer.Stop();
		}
		else
		{
			_appPaused = false;
			_timer.Start();
		}
		return 0;

	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		_clientWidth  = LOWORD(lParam);
		_clientHeight = HIWORD(lParam);
		if( _device )
		{
			if( wParam == SIZE_MINIMIZED )
			{
				_appPaused = true;
				_minimized = true;
				_maximized = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				_appPaused = false;
				_minimized = false;
				_maximized = true;
				OnResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if( _minimized )
				{
					_appPaused = false;
					_minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if( _maximized )
				{
					_appPaused = false;
					_maximized = false;
					OnResize();
				}
				else if( _resizing )
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		_appPaused = true;
		_resizing  = true;
		_timer.Stop();
		return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		_appPaused = false;
		_resizing  = false;
		_timer.Start();
		OnResize();
		return 0;
 
	// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// The WM_MENUCHAR message is sent when a menu is active and the user presses 
	// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}


void App::SetScreenSize(UINT width, UINT height)
{
	_clientHeight = height;
	_clientWidth = width;

	OnResize();
}

bool App::InitMainWindow()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = MainWndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = _hAppInst;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = L"DX11";

	if (!RegisterClass(&wc))
	{
		::MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, _clientWidth, _clientHeight };
	::AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int32 width  = R.right - R.left;
	int32 height = R.bottom - R.top;

	_hMainWnd = ::CreateWindow(L"DX11", _mainWindowCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, _hAppInst, 0); 
	
	if (_hMainWnd == nullptr)
	{
		::MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	::ShowWindow(_hMainWnd, SW_SHOW);
	::UpdateWindow(_hMainWnd);

	return true;
}

bool App::InitDirect3D()
{
	CreateDeviceAndSwapChain();	
	OnResize();

	return true;
}

void App::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if( (_timer.TotalTime() - timeElapsed) >= 1.0f )
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;   
		outs.precision(6);
		outs << _mainWindowCaption << L"    "  << L"FPS: " << fps << L"    "  << L"Frame Time: " << mspf << L" (ms)";

		::SetWindowText(_hMainWnd, outs.str().c_str());
		
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void App::CreateDeviceAndSwapChain()
{
	uint32 createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.BufferDesc.Width = _clientWidth;
		desc.BufferDesc.Height = _clientHeight;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		if (_enable4xMsaa)
		{
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = _4xMsaaQuality - 1;
		}
		else
		{
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
		
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = _hMainWnd;
		desc.Windowed = true;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0;
	}

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr, // default adapter
		_driverType,
		nullptr, // no software device
		createDeviceFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}


void App::CreateRenderTargetView()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

void App::CreateDepthStencilView()
{
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = _clientWidth;
		desc.Height = _clientHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		if (_enable4xMsaa)
		{
			desc.SampleDesc.Count = 4;
			desc.SampleDesc.Quality = _4xMsaaQuality - 1;
		}
		else
		{
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
		}
	
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = _device->CreateTexture2D(&desc, nullptr, _depthStencilBuffer.GetAddressOf());
		CHECK(hr);
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		//HRESULT hr = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), &desc, _depthStencilView.GetAddressOf());
		HRESULT hr = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), nullptr, _depthStencilView.GetAddressOf());
		CHECK(hr);
	}
}
