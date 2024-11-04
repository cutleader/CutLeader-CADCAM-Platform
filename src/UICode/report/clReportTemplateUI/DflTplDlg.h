#pragma once

#include "clReportTemplateUICommon.h"
#include "taoResource.h"

BEGIN_CUTLEADER_NAMESPACE()

// 这个对话框选择默认的“报表模板”。
class ClReportTemplateUI_Export DflTplDlg : public CLDialog
{
	DECLARE_DYNCREATE(DflTplDlg)

public:
	DflTplDlg(CWnd* pParent = NULL);
	~DflTplDlg(void);

	enum { IDD = IDD_clReportTemplateUI_DefaultReportTemplate };

protected: // inherit the interface
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private: // controls.
	CComboBox m_ctrlShtTpl;
	CComboBox m_ctrlNestTpl;
	CComboBox m_ctrlLabelTpl;
};

END_CUTLEADER_NAMESPACE()
