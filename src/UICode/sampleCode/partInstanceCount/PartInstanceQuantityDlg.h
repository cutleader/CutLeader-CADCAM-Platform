#pragma once

#include "partInstanceCountCommon.h"
#include "taoresource.h"


DECLARE_CUTLEADER_CLASS(NestJob)
DECLARE_CUTLEADER_CLASS(Sheet)

BEGIN_CUTLEADER_NAMESPACE()

// 这个对话框显示板材上面零件实例的个数
class PartInstanceCount_Export PartInstanceQuantityDlg : public CLDialog
{
	DECLARE_DYNAMIC(PartInstanceQuantityDlg)

public:
	PartInstanceQuantityDlg(SheetPtr pSheet, CWnd* pParent = NULL);
	virtual ~PartInstanceQuantityDlg(void);

	enum { IDD = IDD_partInstanceCount_partInstanceQuantity };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnBnClickedOk();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

private:
    SheetPtr m_pSheet;

	CString m_strPartInstanceQuantity;

	// use this brush to paint the background.
	CBrush m_brush;
};

END_CUTLEADER_NAMESPACE()
