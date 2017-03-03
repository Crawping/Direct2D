#pragma once

// �������Ҫ������ָ����ƽ̨֮ǰ��λƽ̨�����޸����¶��塣
// �ο�MSDN�����µ�ƽ̨����Ӧ��ֵ
#ifndef WINVER              // ������Win7��Win7���ϰ汾
#define WINVER 0x0700       // �޸����ֵ��ȷ����ͬ��windows�汾
#endif

#ifndef _WIN32_WINNT        // ������Win7��Win7���ϰ汾
#define _WIN32_WINNT 0x0700 // �޸����ֵ��ȷ����ͬ��windows�汾
#endif

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN     // ��windowsͷ�ļ���ȥ������ʹ�õ�����

// windowsͷ�ļ�
#include <windows.h>

// c����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>

// Direct2D��Ҫ��ͷ�ļ�
#include <d2d1.h>
#include <dwrite.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

/******************************************************************
��Ҫ�õ��ĺ궨��
******************************************************************/

template<class Interface>
inline void
SafeRelease(
    Interface **ppInterfaceToRelease
    )
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif


#include <math.h>

/******************************************************************
DemoApp
******************************************************************/
class DemoApp
{
public:
    DemoApp();
    ~DemoApp();

    HRESULT Initialize();

    void RunMessageLoop();

private:
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources();
    void DiscardDeviceResources();

    HRESULT OnRender();

    void OnResize(
        UINT width,
        UINT height
        );

    void OnTimer();

    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
        );

private:
    HWND m_hwnd;

	// �õ�����Դ-----------------------------------------------------
	ID2D1Factory*			m_pD2DFactory;		// D2D����
	IDWriteFactory*			m_pDWriteFactory;	// DWrite����
	ID2D1HwndRenderTarget*	m_pRT;				// ������

	ID2D1SimplifiedGeometrySink*	m_pGeometrySink;			// 
	ID2D1PathGeometry*				m_pPathGeometry;			// ·������ͼ��
	ID2D1LinearGradientBrush*		m_pLinearGradientBrush;		// ���仭ˢ
	ID2D1SolidColorBrush *			m_pSolidBrushOutline;		// ���ֻ�ˢ

};


