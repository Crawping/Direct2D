/**********************************************************************
 @FILE		D2DMatrixPro.h
 @BRIEF		�˹�����ʾ��Direct2D����ĸ߼�ʹ�÷�����
			��˵�����ӽڵ�͸��ڵ�ľ��󸴺�ʹ�÷���
 @AUTHOR	Ray1024
 @DATE		2016.11.29
 *********************************************************************/

#include "../Common/D2DApp.h"

//------------------------------------------------------------------------------
// D2DMatrixPro
//------------------------------------------------------------------------------
class D2DMatrixPro : public D2DApp
{
public:
	D2DMatrixPro(HINSTANCE hInstance);
	~D2DMatrixPro();

	bool Init();
	void UpdateScene(float dt);
	void DrawScene();

private:

	ID2D1SolidColorBrush*	m_pBrush;
	IDWriteTextFormat*		m_pTextFormat;

	ID2D1Bitmap*			m_pBackGround;		// ����λͼ
	ID2D1Bitmap*			m_pNodeChild;		// �ӽڵ�λͼ
	ID2D1Bitmap*			m_pNodeParent;		// ���ڵ�λͼ
};


//------------------------------------------------------------------------------
// �������WinMain
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			D2DMatrixPro app(hInstance);

			if (app.Init())
			{
				app.Run();
			}
		}
		CoUninitialize();
	}

	return 0;
}

//------------------------------------------------------------------------------
// D2DMatrixPro Implement
//------------------------------------------------------------------------------

D2DMatrixPro::D2DMatrixPro(HINSTANCE hInstance)
	: D2DApp(hInstance)
	, m_pBrush(NULL)
	, m_pTextFormat(NULL)
	, m_pBackGround(NULL)
	, m_pNodeChild(NULL)
	, m_pNodeParent(NULL)
{
	m_wndCaption = "D2DMatrixPro";
}

D2DMatrixPro::~D2DMatrixPro()
{
	SafeRelease(&m_pBrush);
	SafeRelease(&m_pTextFormat);
	SafeRelease(&m_pBackGround);
	SafeRelease(&m_pNodeChild);
	SafeRelease(&m_pNodeParent);
}

bool D2DMatrixPro::Init()
{
	if (!D2DApp::Init())
	{
		return false;
	}

	HRESULT hr = S_OK;

	// ���������ʽ
	if (m_pDWriteFactory != NULL)
	{
		hr = m_pDWriteFactory->CreateTextFormat( 
			L"Edwardian Script ITC", 
			NULL, 
			DWRITE_FONT_WEIGHT_NORMAL, 
			DWRITE_FONT_STYLE_NORMAL, 
			DWRITE_FONT_STRETCH_NORMAL, 
			68.0, 
			L"en-us",
			&m_pTextFormat);
	}

	// ������ˢ
	if (m_pRT != NULL && SUCCEEDED(hr))
	{
		hr = m_pRT->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::DarkBlue),
			&m_pBrush
			);
	}

	// ��������λͼ
	if (SUCCEEDED(hr))
	{
		LoadBitmapFromFile(m_pRT, m_pWICFactory, L"bg.png", 0, 0, &m_pBackGround);
	}

	// �������ڵ�
	if (SUCCEEDED(hr))
	{
		LoadBitmapFromFile(m_pRT, m_pWICFactory, L"node_parent.png", 0, 0, &m_pNodeParent);
	}

	// �����ӽڵ�
	if (SUCCEEDED(hr))
	{
		LoadBitmapFromFile(m_pRT, m_pWICFactory, L"node_child.png", 0, 0, &m_pNodeChild);
	}

	return true;
}

// ���ڵ�����
D2D1_POINT_2F pt_parent = D2D1::Point2F(100, 100);
// ���ڵ���ת�Ƕ�
float rotate_parent = 0.f;


// �ӽڵ�����븸�ڵ������
D2D1_POINT_2F pt_child = D2D1::Point2F(100, 0);
// �ӽڵ���ת�Ƕ�
float rotate_child = 0.f;

void D2DMatrixPro::UpdateScene(float dt)
{
	if (rotate_parent <= 360)
	{
		rotate_parent += 0.05;
	}
}

void D2DMatrixPro::DrawScene()
{
	HRESULT hr;

	if (!(m_pRT->CheckWindowState() & D2D1_WINDOW_STATE_OCCLUDED))
	{
		// ��ʼ����
		m_pRT->BeginDraw();

		m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		m_pRT->DrawBitmap(
			m_pBackGround,
			D2D1::RectF(0,0,m_pBackGround->GetSize().width,m_pBackGround->GetSize().height));

		//////////////////////////////////////////////////////////////////
		// ����

		// ���ڵ�
		D2D1::Matrix3x2F matrix_parent = 
			D2D1::Matrix3x2F::Rotation(rotate_parent,
			D2D1::Point2F(m_pNodeParent->GetSize().width/2, m_pNodeParent->GetSize().height/2)
			)
			*
			D2D1::Matrix3x2F::Translation(pt_parent.x, pt_parent.y)
			;
		m_pRT->SetTransform(matrix_parent);

		m_pRT->DrawBitmap(
			m_pNodeParent,
			D2D1::RectF(0,0,m_pNodeParent->GetSize().width,m_pNodeParent->GetSize().height),
			0.5);

		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

		// �ӽڵ�
		D2D1::Matrix3x2F matrix_child = 
			D2D1::Matrix3x2F::Rotation(rotate_child,
			D2D1::Point2F(m_pNodeChild->GetSize().width/2, m_pNodeChild->GetSize().height/2)
			)
			*
			D2D1::Matrix3x2F::Translation(pt_child.x, pt_child.y)
			;
		m_pRT->SetTransform(matrix_child*matrix_parent);

		m_pRT->DrawBitmap(
			m_pNodeChild,
			D2D1::RectF(0,0,m_pNodeChild->GetSize().width,m_pNodeChild->GetSize().height),
			0.5);

		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

		// ���Խ���
		//////////////////////////////////////////////////////////////////

		hr = m_pRT->EndDraw();
	}
}