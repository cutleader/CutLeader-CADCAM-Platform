#pragma once

#include "clOptionUICommon.h"
#include "taoresource.h"
#include "CLResizableDialog.h"
#include "CGridListCtrlGroups.h"
#include "NumEdit.h"

DECLARE_CUTLEADER_CLASS(PartEditParam)

BEGIN_CUTLEADER_NAMESPACE()

class PartEditParamPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(PartEditParamPage)

public:
	PartEditParamPage();
	~PartEditParamPage();

	enum { IDD = IDD_clOptionUI_PartEdit };

protected: // inherit the interface
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void AddExtProp();
	afx_msg void DelExtProp();
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	void DisplayPartEditParam(PartEditParamPtr pPartEditParam);
	PartEditParamPtr GetPartEditParam();

private: // controls
	CString m_strMsg;

	// ¿©’π Ù–‘°£
	CGridListCtrlGroups m_extPropList;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data.
	PartEditParamPtr m_pPartEditParam;
};

END_CUTLEADER_NAMESPACE()
