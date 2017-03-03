/**********************************************************************
 @FILE		D2DApp.h
 @BRIEF		Direct2DӦ�ó����࣬ʹ��ʱ���Լ̳������
 @AUTHOR	Ray1024
 @DATE		2016.11.29
 *********************************************************************/

#ifndef D2DAPP_H
#define D2DAPP_H

#include "D2DUtil.h"
#include "D2DTimer.h"

//------------------------------------------------------------------------------
// D2DApp
//------------------------------------------------------------------------------
class D2DApp
{
public:
    D2DApp(HINSTANCE hInstance);
    virtual ~D2DApp();

	HINSTANCE AppInst()const;
	HWND MainWnd()const;
	void Run();

	// ��ܷ�����������д��ʵ���Լ�����ȾЧ����
	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene()=0; 
	virtual LRESULT WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

	// ������������¼��ı�����غ���
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }

protected:

	HRESULT InitWindow();
	HRESULT InitDirect2D();

	HRESULT CreateDeviceIndependentResources();
	HRESULT CreateDeviceResources();

	void OnDestroy();

    BOOL IsRunning() { return m_fRunning; }

	void CalculateFrameStats();

protected:
	HINSTANCE	m_hAppInst;		// Ӧ�ó���ʵ�����
    HWND		m_hWnd;			// ���ھ��
	std::string	m_wndCaption;	// ���ڱ���
	int			m_clientWidth;	// ���
	int			m_clientHeight;	// �߶�
	BOOL		m_fRunning;		// �Ƿ�����
	D2DTimer	m_timer;		// ���ڼ�¼deltatime����Ϸʱ��

    ID2D1Factory*			m_pD2DFactory;		// D2D����
	IWICImagingFactory*		m_pWICFactory;		// WIC����
	IDWriteFactory*			m_pDWriteFactory;	// DWrite����
    ID2D1HwndRenderTarget*	m_pRT;				// ������
};

#endif // D2DAPP_H