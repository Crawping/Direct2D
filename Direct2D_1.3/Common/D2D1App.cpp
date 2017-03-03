#include "D2D1App.h"


//
// D2D1App Implement
//

D2D1App::D2D1App() 
{
	m_parameters.DirtyRectsCount = 0;
	m_parameters.pDirtyRects = nullptr;
	m_parameters.pScrollRect = nullptr;
	m_parameters.pScrollOffset = nullptr;
}

D2D1App::~D2D1App()
{
	this->DiscardDeviceResources();
	SafeRelease(m_pD2DFactory);
	SafeRelease(m_pWICFactory);
	SafeRelease(m_pDWriteFactory);
}

// ��ʼ��
HRESULT D2D1App::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	HRESULT hr = E_FAIL;
	//register window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = D2D1App::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"D2D1WndClass";
	wcex.hIcon = nullptr;
	// ע�ᴰ��
	RegisterClassExW(&wcex);
	// ���㴰�ڴ�С
	RECT window_rect = { 0, 0, 1080, 640 };
	DWORD window_style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&window_rect, window_style, FALSE);
	window_rect.right -= window_rect.left;
	window_rect.bottom -= window_rect.top;
	window_rect.left = (::GetSystemMetrics(SM_CXFULLSCREEN) - window_rect.right) / 2;
	window_rect.top = (::GetSystemMetrics(SM_CYFULLSCREEN) - window_rect.bottom) / 2;
	// ��������
	m_hWnd = CreateWindowExA(0, "D2D1WndClass", m_wndCaption.c_str(), window_style,
		window_rect.left, window_rect.top, window_rect.right, window_rect.bottom,
		0, 0, hInstance, this);

	hr = m_hWnd ? S_OK : E_FAIL;
	
	// ��ʾ����
	if (SUCCEEDED(hr))
	{
		CreateDeviceIndependentResources();
		CreateDeviceResources();

		ShowWindow(m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);
	}
	return hr;
}

HRESULT D2D1App::CreateDeviceIndependentResources()
{
	// ����D2D����
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory1),
		reinterpret_cast<void**>(&m_pD2DFactory));
	// ���� WIC ����.
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory2,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pWICFactory));
	}
	// ���� DirectWrite ����.
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}

	return hr;
}

HRESULT D2D1App::CreateDeviceResources()
{
	// DXGI ����
	IDXGIFactory2*						pDxgiFactory = nullptr;
	// DXGI �豸
	IDXGIDevice1*						pDxgiDevice = nullptr;

	HRESULT hr = S_OK;


	// ���� D3D11�豸���豸������ 
	if (SUCCEEDED(hr))
	{
		// D3D11 ����flag 
		// һ��Ҫ��D3D11_CREATE_DEVICE_BGRA_SUPPORT�����򴴽�D2D�豸�����Ļ�ʧ��
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
		// Debug״̬ ��D3D DebugLayer�Ϳ���ȡ��ע��
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};
		// �����豸
		hr = D3D11CreateDevice(
			nullptr,					// ��Ϊ��ָ��ѡ��Ĭ���豸
			D3D_DRIVER_TYPE_HARDWARE,	// ǿ��ָ��Ӳ����Ⱦ
			nullptr,					// ǿ��ָ��WARP��Ⱦ D3D_DRIVER_TYPE_WARP û������ӿ�
			creationFlags,				// ����flag
			featureLevels,				// ��ʹ�õ����Եȼ��б�
			ARRAYSIZE(featureLevels),	// ���Եȼ��б���
			D3D11_SDK_VERSION,			// SDK �汾
			&m_pD3DDevice,				// ���ص�D3D11�豸ָ��
			&m_featureLevel,			// ���ص����Եȼ�
			&m_pD3DDeviceContext);		// ���ص�D3D11�豸������ָ��
	}

	// ���� IDXGIDevice
	if (SUCCEEDED(hr))
		hr = m_pD3DDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));
	// ����D2D�豸
	if (SUCCEEDED(hr))
		hr = m_pD2DFactory->CreateDevice(pDxgiDevice, &m_pD2DDevice);
	// ����D2D�豸������
	if (SUCCEEDED(hr))
		hr = m_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pD2DDeviceContext);

	SafeRelease(pDxgiDevice);
	SafeRelease(pDxgiFactory);

	CreateWindowSizeDependentResources();

	return hr;
}

void D2D1App::CreateWindowSizeDependentResources()
{
	// DXGI ������
	IDXGIAdapter*						pDxgiAdapter = nullptr;
	// DXGI ����
	IDXGIFactory2*						pDxgiFactory = nullptr;
	// DXGI Surface ��̨����
	IDXGISurface*						pDxgiBackBuffer = nullptr;
	// DXGI �豸
	IDXGIDevice1*						pDxgiDevice = nullptr;

	HRESULT hr = S_OK;

	// ���֮ǰ���ڵĳ���������豸
	m_pD2DDeviceContext->SetTarget(nullptr);
	SafeRelease(m_pD2DTargetBimtap);
	m_pD3DDeviceContext->Flush();

	RECT rect = { 0 }; GetClientRect(m_hWnd, &rect);

	if (m_pSwapChain != nullptr)
	{
		// ����������Ѿ������������軺����
		hr = m_pSwapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			lround(rect.right - rect.left),
			lround(rect.bottom - rect.top),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0);

		assert( hr == S_OK );
	}
	else
	{
		// �������Ѵ��ڵ�D3D�豸����һ���µĽ�����
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = lround(rect.right - rect.left);
		swapChainDesc.Height = lround(rect.bottom - rect.top);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// ��ȡ IDXGIDevice
		if (SUCCEEDED(hr))
		{
			hr = m_pD3DDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));
		}
		// ��ȡDxgi������ ���Ի�ȡ����������Ϣ
		if (SUCCEEDED(hr))
		{
			hr = pDxgiDevice->GetAdapter(&pDxgiAdapter);
		}
		// ��ȡDxgi����
		if (SUCCEEDED(hr))
		{
			hr = pDxgiAdapter->GetParent(IID_PPV_ARGS(&pDxgiFactory));
		}
		// ����������
		if (SUCCEEDED(hr))
		{
			hr = pDxgiFactory->CreateSwapChainForHwnd(
				m_pD3DDevice,
				m_hWnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				&m_pSwapChain);
		}
		// ȷ��DXGI������߲��ᳬ��һ֡
		if (SUCCEEDED(hr))
		{
			hr = pDxgiDevice->SetMaximumFrameLatency(1);
		}
	}

	// ������Ļ����
	if (SUCCEEDED(hr))
	{
		hr = m_pSwapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
	}
	// ���ý�������ȡDxgi����
	if (SUCCEEDED(hr))
	{
		hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pDxgiBackBuffer));
	}
	// ����Dxgi���洴��λͼ
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.0f,
			96.0f);
		hr = m_pD2DDeviceContext->CreateBitmapFromDxgiSurface(
			pDxgiBackBuffer,
			&bitmapProperties,
			&m_pD2DTargetBimtap);
	}
	// ����
	if (SUCCEEDED(hr))
	{
		// ���� Direct2D ��ȾĿ��
		m_pD2DDeviceContext->SetTarget(m_pD2DTargetBimtap);
	}

	SafeRelease(pDxgiDevice);
	SafeRelease(pDxgiAdapter);
	SafeRelease(pDxgiFactory);
	SafeRelease(pDxgiBackBuffer);
}

// �����豸�����Դ
void D2D1App::DiscardDeviceResources()
{
	SafeRelease(m_pD2DTargetBimtap);
	SafeRelease(m_pSwapChain);
	SafeRelease(m_pD2DDeviceContext);
	SafeRelease(m_pD2DDevice);
	SafeRelease(m_pD3DDevice);
	SafeRelease(m_pD3DDeviceContext);
}


void D2D1App::CalculateFrameStats()
{
	// ����ÿ��ƽ��֡���Ĵ��룬�������˻���һ֡��ƽ��ʱ��
	// ��Щͳ����Ϣ����ʾ�ڴ��ڱ�������
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// ����һ��ʱ���ڵ�ƽ��ֵ
	if ((m_timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << m_wndCaption.c_str() << " | "
			<< "FPS: " << fps << " | "
			<< "Frame Time: " << mspf << " (ms)";
		SetWindowTextA(m_hWnd, outs.str().c_str());

		// Ϊ�˼�����һ��ƽ��ֵ����һЩֵ
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void D2D1App::Run()
{
	m_timer.Reset();

    while (m_fRunning)
    {
		// ������յ�Window��Ϣ��������Щ��Ϣ
        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		// ���������ж���/��Ϸ
		else
		{
			m_timer.Tick();
			UpdateScene(m_timer.DeltaTime());
			DrawScene();
			CalculateFrameStats();
		}
    }
}

void D2D1App::OnResize(UINT width, UINT height)
{
	CreateWindowSizeDependentResources();
}

void D2D1App::OnDestroy()
{
    m_fRunning = FALSE;
}

LRESULT D2D1App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        D2D1App *pD2DApp = (D2D1App *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pD2DApp)
            );

        result = 1;
    }
    else
    {
        D2D1App *pD2DApp = reinterpret_cast<D2D1App *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pD2DApp)
        {
            switch(message)
            {
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				pD2DApp->OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_SIZE:
				pD2DApp->OnResize(LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				pD2DApp->OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_MOUSEMOVE:
				pD2DApp->OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_MOUSEWHEEL:
				pD2DApp->OnMouseWheel(LOWORD(wParam), HIWORD(wParam), LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_DESTROY:
                {
                    pD2DApp->OnDestroy();
                    PostQuitMessage(0);
                }
                result = 1;
                wasHandled = true;
                break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}