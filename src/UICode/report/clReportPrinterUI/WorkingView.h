#pragma once

#include "clReportPrinterCommon.h"
#include "IPrintWnd.h"

BEGIN_CUTLEADER_NAMESPACE()

// ��ӡԤ������е���ͼ��
// ע��
//  1) �����ͼ�ᱻPreviewViewEx�滻����Ϊ����m_pOrigView��Ա��
//  2) ��ӡ�ķ�����ת��������С�
class WorkingView : public CView
{
	DECLARE_DYNCREATE(WorkingView)

public:
	WorkingView(void);
	~WorkingView(void);

public: // inherit from the base class.
	virtual void OnDraw(CDC* pDC);

	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);

protected:
	DECLARE_MESSAGE_MAP()

public:
	void OnPrintPreview();
	void OnFilePrint();

	void SetPrintWnd(IPrintWnd* pPrintWnd) { m_pPrintWnd = pPrintWnd; }
	
	void IsDestroyParentFrm(BOOL bDestroyParentFrm) { m_bDestroyParentFrm = bDestroyParentFrm; }

public:
	friend class PreviewViewEx;

	IPrintWnd* m_pPrintWnd;
	CFrameWnd* m_pCurrentFrame;

	// �����Ҫ��Ҫ���ٸ���ܡ�
	BOOL m_bDestroyParentFrm;
};

END_CUTLEADER_NAMESPACE()
