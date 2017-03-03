#include "D2DApp.h"

namespace
{
	// ��Ϊ�޷�ֱ�ӽ�D2DApp::MsgProc��ֵ��WNDCLASS::lpfnWndProc
	// ���Խ�ȫ�ֺ���MainWndProc��ֵ��WNDCLASS::lpfnWndProc
	// ��MainWndProc��������һ��ȫ�ֵ�D2DApp����ת����Ϣ
	D2DApp* g_D2DApp = 0;
}

LRESULT CALLBACK
	MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// ����֮ǰ��D2DApp�Ĺ��캯�����g_D2DApp��ֵ
	return g_D2DApp->WndProc(hwnd, msg, wParam, lParam);
}

//
// D2DApp Implement
//

D2DApp::D2DApp(HINSTANCE hInstance) 
	: m_hAppInst(hInstance)
	, m_hWnd(NULL)
	, m_wndCaption("D2DApp")
	, m_clientWidth(0)
	, m_clientHeight(0)
	, m_pD2DFactory(NULL)
	, m_pWICFactory(NULL)
	, m_pDWriteFactory(NULL)
	, m_pRT(NULL)
{
	g_D2DApp = this;
}

D2DApp::~D2DApp()
{
	SafeRelease(&m_pRT);

	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pD2DFactory);
}

HINSTANCE D2DApp::AppInst()const
{
	return m_hAppInst;
}

HWND D2DApp::MainWnd()const
{
	return m_hWnd;
}

bool D2DApp::Init()
{
	if (!SUCCEEDED(InitWindow()))
		return false;

	if (!SUCCEEDED(InitDirect2D()))
		return false;

	return true;
}

HRESULT D2DApp::InitWindow()
{
	HRESULT hr;

	m_fRunning = FALSE;

	// �����豸�޹���Դ
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// ע�ᴰ����
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = MainWndProc;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = sizeof(LONG_PTR);
		wcex.hInstance     = m_hAppInst;
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName  = NULL;
		wcex.lpszClassName = "D2DWndClassName";

		RegisterClassEx(&wcex);

		// ��ȡ����DPI
		FLOAT dpiX;
		FLOAT dpiY;
		m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		// ��������
		m_hWnd = CreateWindowA(
			"D2DWndClassName",
			m_wndCaption.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			static_cast<UINT>(ceil(960.0f * dpiX / 96.0f)),
			static_cast<UINT>(ceil(640.0f * dpiY / 96.0f)),
			NULL,
			NULL,
			m_hAppInst,
			this
			);
		hr = m_hWnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			m_fRunning = TRUE;

			ShowWindow(m_hWnd, SW_SHOWNORMAL);
			UpdateWindow(m_hWnd);
		}
	}

	return hr;
}

HRESULT D2DApp::InitDirect2D()
{
	return CreateDeviceResources();
}

HRESULT D2DApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// ����D2D����
	if (m_pD2DFactory == NULL)
	{
		hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, 
			&m_pD2DFactory);
	}

	// ����WIC����
	if (m_pWICFactory == NULL && SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pWICFactory));
	}

	// ����DWrite����
	if (m_pDWriteFactory == NULL && SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED, 
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}

	return hr;
}

HRESULT D2DApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	// ����������
	if (m_pRT == NULL && SUCCEEDED(hr))
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
			);

		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
			&m_pRT
			);
	}

	return hr;
}

void D2DApp::Run()
{
	m_timer.Reset();
    while (this->IsRunning())
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

void D2DApp::CalculateFrameStats()
{
	// ����ÿ��ƽ��֡���Ĵ��룬�������˻���һ֡��ƽ��ʱ��
	// ��Щͳ����Ϣ����ʾ�ڴ��ڱ�������
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// ����һ��ʱ���ڵ�ƽ��ֵ
	if( (m_timer.TotalTime() - timeElapsed) >= 1.0f )
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

void D2DApp::OnResize()
{
	if (m_pRT != NULL)
	{
		D2D1_SIZE_U size;
		size.width = m_clientWidth;
		size.height = m_clientHeight;

		m_pRT->Resize(size);
	}
}

void D2DApp::OnDestroy()
{
    m_fRunning = FALSE;
}

LRESULT D2DApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        D2DApp *pD2DApp = (D2DApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pD2DApp)
            );

        result = 1;
    }
    else
    {
        D2DApp *pD2DApp = reinterpret_cast<D2DApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pD2DApp)
        {
            switch(message)
            {
            case WM_SIZE:
                {
                    m_clientWidth = LOWORD(lParam);
					m_clientHeight = HIWORD(lParam);
                    pD2DApp->OnResize();
                }
                result = 0;
                wasHandled = true;
                break;

			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				OnMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				OnMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
				break;

			case WM_MOUSEMOVE:
				OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
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