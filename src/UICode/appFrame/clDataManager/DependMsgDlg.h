#pragma once

#include "clDataManagerCommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(DependNodeList)

BEGIN_CUTLEADER_NAMESPACE()

// this msg box will prompt user about the dependence info.
class ClDataManager_Export DependMsgDlg : public CLDialog
{
	DECLARE_DYNAMIC(DependMsgDlg)

public:
	DependMsgDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~DependMsgDlg();

// Dialog Data
	enum { IDD = IDD_clDataManagerUI_Depend_Message };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnYes();
	afx_msg void OnNo();
	afx_msg void OnViewDetail();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	//
	void Init(CString strMsg, DependNodeListPtr pDependNodeList);

private: // for controls
	CString m_strMsg;

	// the dependence info.
	DependNodeListPtr m_pDependNodeList;

	// use this brush to paint the background.
	CBrush m_brush;
};

END_CUTLEADER_NAMESPACE()
