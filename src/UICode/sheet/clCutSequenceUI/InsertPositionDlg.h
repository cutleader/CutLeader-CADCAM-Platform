#pragma once

#include "clCutSequenceUICommon.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// “指定插入位置”对话框。
class ClCutSequenceUI_Export InsertPositionDlg : public CLDialog
{
	DECLARE_DYNCREATE(InsertPositionDlg)

public:
	InsertPositionDlg(CWnd* pParent = NULL);
	~InsertPositionDlg(void);

	enum { IDD = IDD_clCutSequenceUI_InsertPosition };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckBehind();
    afx_msg void OnQuitCurrentActionButton();

	DECLARE_MESSAGE_MAP()

public:
	BOOL IsInsertBehind() { return m_bInsertBehind; }

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	BOOL m_bInsertBehind;
    CMFCButton m_ctrlQuitCurrentActionButton;
};

END_CUTLEADER_NAMESPACE()
