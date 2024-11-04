#pragma once

#include "clCutSequenceUICommon.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// “以零件为单位”对话框。
class ClCutSequenceUI_Export ApplyPartDlg : public CLDialog
{
	DECLARE_DYNCREATE(ApplyPartDlg)

public:
	ApplyPartDlg(CWnd* pParent = NULL);
	~ApplyPartDlg(void);

	enum { IDD = IDD_clCutSequenceUI_ApplyPart };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckPart();
    afx_msg void OnQuitCurrentActionButton();

	DECLARE_MESSAGE_MAP()

public:
	BOOL IsApplyPart() const { return m_bApplyPart; }
    void IsApplyPart(BOOL bApplyPart) { m_bApplyPart = bApplyPart; }

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	BOOL m_bApplyPart;
    CMFCButton m_ctrlQuitCurrentActionButton;
};

END_CUTLEADER_NAMESPACE()
