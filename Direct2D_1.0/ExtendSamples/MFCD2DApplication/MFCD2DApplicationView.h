
// MFCD2DApplicationView.h : CMFCD2DApplicationView ��Ľӿ�
//

#pragma once


class CMFCD2DApplicationView : public CView
{
protected: // �������л�����
	CMFCD2DApplicationView();
	DECLARE_DYNCREATE(CMFCD2DApplicationView)

// ����
public:
	CMFCD2DApplicationDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
public:
	virtual ~CMFCD2DApplicationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ��Ա
	CD2DTextFormat* m_pTextFormat;
	CD2DSolidColorBrush* m_pBlackBrush;
	CD2DLinearGradientBrush* m_pLinearGradientBrush;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnAfxWmDraw2d(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // MFCD2DApplicationView.cpp �еĵ��԰汾
inline CMFCD2DApplicationDoc* CMFCD2DApplicationView::GetDocument() const
   { return reinterpret_cast<CMFCD2DApplicationDoc*>(m_pDocument); }
#endif

