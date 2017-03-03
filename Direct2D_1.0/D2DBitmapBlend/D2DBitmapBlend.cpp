//-----------------------------------------------------------------
// ���ܣ�������֮��ɫ��ϣ�������ϸ������ο���http://www.cnblogs.com/Ray1024/
// ���ߣ�Ray1024
// ��ַ��http://www.cnblogs.com/Ray1024/
//-----------------------------------------------------------------

#include "D2DBitmapBlend.h"

//-----------------------------------------------------------------
// ���ļ�����D2Dλͼ
//-----------------------------------------------------------------
HRESULT LoadBitmapFromFile(
	ID2D1RenderTarget *pRenderTarget,
	IWICImagingFactory *pIWICFactory,
	LPCWSTR uri,
	UINT width,
	UINT height,
	ID2D1Bitmap **ppBitmap)
{
	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		);

	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		if (width != 0 || height != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (width == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(height) / static_cast<FLOAT>(originalHeight);
					width = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (height == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(width) / static_cast<FLOAT>(originalWidth);
					height = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(
						pSource,
						width,
						height,
						WICBitmapInterpolationModeCubic
						);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(
						pScaler,
						GUID_WICPixelFormat32bppPBGRA,
						WICBitmapDitherTypeNone,
						NULL,
						0.f,
						WICBitmapPaletteTypeMedianCut
						);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
				);
		}
	}
	if (SUCCEEDED(hr))
	{

		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pStream);
	SafeRelease(&pConverter);
	SafeRelease(&pScaler);

	return hr;
}

//-----------------------------------------------------------------
// ���ļ�����WICλͼ�����������ϣ����մ�����D2Dλͼ
//-----------------------------------------------------------------
ID2D1Bitmap* GetBlendedBitmapFromFile(
	IWICImagingFactory *pIWICFactory,
	ID2D1RenderTarget *pRenderTarget,
	LPCWSTR uri,
	const D2D1_COLOR_F &color)
{
	ID2D1Bitmap*			pBitmap		= NULL;
	IWICBitmapDecoder*		pDecoder	= NULL;
	IWICBitmapFrameDecode*	pSource		= NULL;
	IWICBitmap*				pWIC		= NULL;
	IWICFormatConverter*	pConverter	= NULL;
	IWICBitmapScaler*		pScaler		= NULL;
	UINT					originalWidth	= 0;
	UINT					originalHeight	= 0;

	// 1.����IWICBitmap����

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(
		uri,
		NULL,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&pDecoder
		);

	if (SUCCEEDED(hr))
	{
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		hr = pSource->GetSize(&originalWidth,&originalHeight);
	}

	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateBitmapFromSourceRect(
			pSource, 0,0,(UINT)originalWidth,(UINT)originalHeight, &pWIC);
	}

	if (SUCCEEDED(hr))
	{
		// 2.��IWICBitmap�����ȡ��������
		IWICBitmapLock *pILock = NULL;
		WICRect rcLock = { 0, 0, originalWidth, originalHeight };
		hr = pWIC->Lock(&rcLock, WICBitmapLockWrite, &pILock);

		if (SUCCEEDED(hr))
		{
			UINT cbBufferSize = 0;
			BYTE *pv = NULL;

			if (SUCCEEDED(hr))
			{
				// ��ȡ�������������Ͻ����ص�ָ��
				hr = pILock->GetDataPointer(&cbBufferSize, &pv);
			}

			// 3.���������ϵ����ؼ���
			for (unsigned int i=0; i<cbBufferSize; i+=4)
			{
				if (pv[i+3] != 0)
				{
					pv[i]	*=color.b;
					pv[i+1]	*=color.g;
					pv[i+2]	*=color.r;
					pv[i+3] *=color.a;
				}
			}

			// 4.��ɫ��ϲ����������ͷ�IWICBitmapLock����
			pILock->Release();
		}
	}

	// 5.ʹ��IWICBitmap���󴴽�D2Dλͼ

	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		hr = pIWICFactory->CreateBitmapScaler(&pScaler);
	}
	if (SUCCEEDED(hr))
	{
		hr = pScaler->Initialize(pWIC, (UINT)originalWidth, (UINT)originalHeight, WICBitmapInterpolationModeCubic);
	}
	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pScaler,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
			);
	}

	if (SUCCEEDED(hr))
	{
		hr = pRenderTarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			&pBitmap
			);
	}

	SafeRelease(&pConverter);
	SafeRelease(&pScaler);
	SafeRelease(&pDecoder);
	SafeRelease(&pSource);

	return pBitmap;
}

/******************************************************************
�������
******************************************************************/
int WINAPI WinMain(
    HINSTANCE	/* hInstance */		,
    HINSTANCE	/* hPrevInstance */	,
    LPSTR		/* lpCmdLine */		,
    int			/* nCmdShow */		)
{
    // Ignoring the return value because we want to continue running even in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}

/******************************************************************
DemoAppʵ��
******************************************************************/

DemoApp::DemoApp() :
    m_hwnd(NULL),
    m_pD2DFactory(NULL),
	m_pDWriteFactory(NULL),
    m_pRT(NULL),
	m_pWICFactory(NULL),
	m_pBitmap(NULL),
	m_pBitmapBlended(NULL),
	m_pBitmapBlended1(NULL),
	m_pBitmapBlended2(NULL)
{
}

DemoApp::~DemoApp()
{
	SafeRelease(&m_pWICFactory);
    SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRT);
    SafeRelease(&m_pBitmap);
	SafeRelease(&m_pBitmapBlended);
	SafeRelease(&m_pBitmapBlended1);
	SafeRelease(&m_pBitmapBlended2);
}

HRESULT DemoApp::Initialize()
{
    HRESULT hr;

    //register window class
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = DemoApp::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = HINST_THISCOMPONENT;
    wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = NULL;
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = L"D2DDemoApp";

    RegisterClassEx(&wcex);

    hr = CreateDeviceIndependentResources();
    if (SUCCEEDED(hr))
    {
        // Create the application window.
        //
        // Because the CreateWindow function takes its size in pixels, we
        // obtain the system DPI and use it to scale the window size.
        FLOAT dpiX, dpiY;
        m_pD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

        m_hwnd = CreateWindow(
            L"D2DDemoApp",
            L"D2DBitmapBlend",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            static_cast<UINT>(ceil(960.f * dpiX / 96.f)),
            static_cast<UINT>(ceil(640.f * dpiY / 96.f)),
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this
            );
        hr = m_hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr))
        {
			ShowWindow(m_hwnd, SW_SHOWNORMAL);

			UpdateWindow(m_hwnd);
        }
    }

    return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
    HRESULT hr;
    ID2D1GeometrySink *pSink = NULL;

    // ����D2D����
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (m_pDWriteFactory == NULL && SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}

	if (m_pWICFactory == NULL && SUCCEEDED(hr))
	{
		if (!SUCCEEDED(
			CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pWICFactory)
			)
			))
			return FALSE;
	}

    return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRT)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
            );

        // ����render target
        hr = m_pD2DFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRT
            );

		// ����λͼ
		if (SUCCEEDED(hr))
		{
			LoadBitmapFromFile(m_pRT,m_pWICFactory, L"bitmap.png",0,0, &m_pBitmap);
		}

		// ����λͼ����������ɫ���
		if (SUCCEEDED(hr))
		{
			// ���ļ�����WICλͼ����WICλͼ������ɫ��ϣ�֮�󴴽�D2Dλͼ
			m_pBitmapBlended = GetBlendedBitmapFromFile(m_pWICFactory, m_pRT, L"bitmap.png", D2D1::ColorF(1, 0, 0, 1));//��ɫ
			m_pBitmapBlended1 = GetBlendedBitmapFromFile(m_pWICFactory, m_pRT, L"bitmap.png", D2D1::ColorF(0, 1, 0, 1));//��ɫ
			m_pBitmapBlended2 = GetBlendedBitmapFromFile(m_pWICFactory, m_pRT, L"bitmap.png", D2D1::ColorF(0, 0, 1, 1));//��ɫ
		}
    }

    return hr;
}

void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRT);
    SafeRelease(&m_pBitmap);
	SafeRelease(&m_pBitmapBlended);
	SafeRelease(&m_pBitmapBlended1);
	SafeRelease(&m_pBitmapBlended2);
}

void DemoApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT DemoApp::OnRender()
{
    HRESULT hr;

    hr = CreateDeviceResources();
    if (SUCCEEDED(hr) && !(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
    {
        // ��ʼ����
        m_pRT->BeginDraw();

        m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
        m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::White));

		// ����λͼ
        m_pRT->DrawBitmap(
			m_pBitmap,
			D2D1::RectF(0,0,
			m_pBitmap->GetSize().width,
			m_pBitmap->GetSize().height));

		// ������ɫ��Ϻ��λͼ
		m_pRT->DrawBitmap(
			m_pBitmapBlended,
			D2D1::RectF(
			0 + m_pBitmap->GetSize().width,
			0,
			m_pBitmap->GetSize().width + m_pBitmapBlended->GetSize().width,
			0 + m_pBitmapBlended->GetSize().height));

		m_pRT->DrawBitmap(
			m_pBitmapBlended1,
			D2D1::RectF(
			0,
			0 + m_pBitmap->GetSize().height,
			0 + m_pBitmapBlended1->GetSize().width,
			m_pBitmap->GetSize().height + m_pBitmapBlended1->GetSize().height));

		m_pRT->DrawBitmap(
			m_pBitmapBlended2,
			D2D1::RectF(
			0 + m_pBitmap->GetSize().width,
			0 + m_pBitmap->GetSize().height,
			m_pBitmap->GetSize().width + m_pBitmapBlended1->GetSize().width,
			m_pBitmap->GetSize().height + m_pBitmapBlended1->GetSize().height));
		
        // ��������
        hr = m_pRT->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
    }

    InvalidateRect(m_hwnd, NULL, FALSE);

    return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRT)
    {
        D2D1_SIZE_U size;
        size.width = width;
        size.height = height;

        // Note: This method can fail, but it's okay to ignore the
        // error here -- it will be repeated on the next call to
        // EndDraw.
        m_pRT->Resize(size);
    }
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
            );

        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
                )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch(message)
            {
            case WM_SIZE:
                {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pDemoApp->OnResize(width, height);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_PAINT:
            case WM_DISPLAYCHANGE:
                {
                    PAINTSTRUCT ps;
                    BeginPaint(hwnd, &ps);

                    pDemoApp->OnRender();
                    EndPaint(hwnd, &ps);
                }
                result = 0;
                wasHandled = true;
                break;

            case WM_DESTROY:
                {
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