
// MFCD2DApplicationView.cpp : CMFCD2DApplicationView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "MFCD2DApplication.h"
#endif

#include "MFCD2DApplicationDoc.h"
#include "MFCD2DApplicationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCD2DApplicationView

IMPLEMENT_DYNCREATE(CMFCD2DApplicationView, CView)

BEGIN_MESSAGE_MAP(CMFCD2DApplicationView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCD2DApplicationView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(AFX_WM_DRAW2D, &CMFCD2DApplicationView::OnAfxWmDraw2d)
END_MESSAGE_MAP()

// CMFCD2DApplicationView ����/����

CMFCD2DApplicationView::CMFCD2DApplicationView()
{
	// TODO: �ڴ˴���ӹ������
	// Enable D2D support for this window:  
	EnableD2DSupport();

	// Initialize D2D resources:  
	m_pBlackBrush = new CD2DSolidColorBrush(GetRenderTarget(), D2D1::ColorF(D2D1::ColorF::Black));

	m_pTextFormat = new CD2DTextFormat(GetRenderTarget(), _T("Verdana"), 50);
	m_pTextFormat->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pTextFormat->Get()->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	D2D1_GRADIENT_STOP gradientStops[2];

	gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::White);
	gradientStops[0].position = 0.f;
	gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::Indigo);
	gradientStops[1].position = 1.f;

	m_pLinearGradientBrush = new CD2DLinearGradientBrush(GetRenderTarget(),
		gradientStops, ARRAYSIZE(gradientStops),
		D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0), D2D1::Point2F(0, 0)));
}

CMFCD2DApplicationView::~CMFCD2DApplicationView()
{
}

BOOL CMFCD2DApplicationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CMFCD2DApplicationView ����

void CMFCD2DApplicationView::OnDraw(CDC* /*pDC*/)
{
	CMFCD2DApplicationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CMFCD2DApplicationView ��ӡ


void CMFCD2DApplicationView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCD2DApplicationView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CMFCD2DApplicationView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CMFCD2DApplicationView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CMFCD2DApplicationView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCD2DApplicationView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCD2DApplicationView ���

#ifdef _DEBUG
void CMFCD2DApplicationView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCD2DApplicationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCD2DApplicationDoc* CMFCD2DApplicationView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCD2DApplicationDoc)));
	return (CMFCD2DApplicationDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCD2DApplicationView ��Ϣ�������


void CMFCD2DApplicationView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������

	m_pLinearGradientBrush->SetEndPoint(CPoint(cx, cy));
}


afx_msg LRESULT CMFCD2DApplicationView::OnAfxWmDraw2d(WPARAM wParam, LPARAM lParam)
{
	CHwndRenderTarget* pRenderTarget = (CHwndRenderTarget*)lParam;
	ASSERT_VALID(pRenderTarget);

	CRect rect;
	GetClientRect(rect);

	pRenderTarget->FillRectangle(rect, m_pLinearGradientBrush);
	pRenderTarget->DrawText(_T("Hello, World!"), rect, m_pBlackBrush, m_pTextFormat);

	return TRUE;
}
