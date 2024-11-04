#pragma once

#include "clPartLayoutUICommon.h"
#include "taoresource.h"
#include "SubjectBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// “传播相同零件”对话框。
class ClPartLayoutUI_Export SpreadSamePartDlg : public CLDialog, public SubjectBase
{
	DECLARE_DYNCREATE(SpreadSamePartDlg)

public:
	SpreadSamePartDlg(CWnd* pParent = NULL);
	~SpreadSamePartDlg(void);

	enum { IDD = IDD_clPartLayoutUI_SpreadSameParts };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckSpread();
    afx_msg void OnQuitCurrentActionButton();

	DECLARE_MESSAGE_MAP()

public:
	BOOL IsSpread() const { return m_bSpread; }

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	BOOL m_bSpread;
    CMFCButton m_ctrlQuitCurrentActionButton;
};

END_CUTLEADER_NAMESPACE()
