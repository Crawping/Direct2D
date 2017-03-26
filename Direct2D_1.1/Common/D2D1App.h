 /***********************************************************************
 ���ܣ�Direct2D 1.1Ӧ�ó����࣬ʹ��ʱ���Լ̳������
 ���ߣ�Ray1024
 ��ַ��http://www.cnblogs.com/Ray1024/
 ***********************************************************************/


#ifndef RAY1024_D2DAPP
#define RAY1024_D2DAPP

#include "Common.h"
#include "D2D1Timer.h"


// D2D1App
class D2D1App
{
public:
	// ���캯��
    D2D1App();
	// ��������
    virtual ~D2D1App();

	// ��ʼ��
	virtual HRESULT Initialize(HINSTANCE hInstance, int nCmdShow);
	// ������д�˺�����ʵ����������ÿִ֡�еĲ���
	virtual void UpdateScene(float dt) {};
	// ��Ⱦ
	virtual void DrawScene() = 0; 

	// ��Ϣѭ��
	void Run();
	// ������Ϣ
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:

	// �����豸�޹���Դ
	HRESULT CreateDeviceIndependentResources();
	// �����豸�й���Դ
	HRESULT CreateDeviceResources();
	// ÿ�δ��ڴ�С����ʱ������Ҫ���´�����Щ��Դ
	void CreateWindowSizeDependentResources();
	// �����豸�й���Դ
	void DiscardDeviceResources();
	// ����֡����Ϣ
	void CalculateFrameStats();

	// ��Ϣ�������
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }
	virtual void OnMouseWheel(UINT nFlags, short zDelta, int x, int y) { }
	// ��Ϣ��������ߴ�
	virtual void OnResize(UINT width, UINT height);
	// ��Ϣ������������
	virtual void OnDestroy();

protected:
	// Ӧ�ó���ʵ�����
	HINSTANCE							m_hAppInst = nullptr;
	// ���ھ��
	HWND								m_hWnd = nullptr;
	// ���ڱ���
	std::string							m_wndCaption = "D2D1App";
	// �Ƿ�����
	BOOL								m_fRunning = TRUE;
	// ���ڼ�¼deltatime����Ϸʱ��
	D2D1Timer							m_timer;

	// D3D �豸
	ID3D11Device*						m_pD3DDevice;
	// D3D �豸������
	ID3D11DeviceContext*				m_pD3DDeviceContext;

	// D2D ����
	ID2D1Factory1*                      m_pD2DFactory = nullptr;
	// D2D �豸
	ID2D1Device*                       m_pD2DDevice = nullptr;
	// D2D �豸������
	ID2D1DeviceContext*                m_pD2DDeviceContext = nullptr;

	// WIC ����
	IWICImagingFactory2*                m_pWICFactory = nullptr;
	// DWrite����
	IDWriteFactory1*                    m_pDWriteFactory = nullptr;
	// DXGI ������
	IDXGISwapChain1*                    m_pSwapChain = nullptr;
	// D2D λͼ ���浱ǰ��ʾ��λͼ
	ID2D1Bitmap1*                       m_pD2DTargetBimtap = nullptr;
	//  �����豸���Եȼ�
	D3D_FEATURE_LEVEL                   m_featureLevel;
	// �ֶ�������
	DXGI_PRESENT_PARAMETERS             m_parameters;
};

#endif