#pragma once

#include "clExpertLibUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(NestConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

class NestAdvancedParamDlg : public CLDialog
{
	DECLARE_DYNCREATE(NestAdvancedParamDlg)

public:
	NestAdvancedParamDlg(void);
	NestAdvancedParamDlg(NestConfigItemPtr pNestConfigItem);
	~NestAdvancedParamDlg(void);

	enum { IDD = IDD_clExpertLib_Nest_AdvancedConfig };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private: // for controls.
	CNumEdit m_dPartRotStep;
	BOOL m_bOnlyUseGridNest;
	BOOL m_bCreateGrid;
	CNumEdit m_ctrlAllowedNestTime;
	CNumEdit m_ctrlEvaluateFactor;

	CBrush m_brush;

private: // for data.
	NestConfigItemPtr m_pNestConfigItem;
};

END_CUTLEADER_NAMESPACE()