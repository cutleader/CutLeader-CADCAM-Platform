#pragma once

#include "clExpertLibUICommon.h"
#include "taoResource.h"

BEGIN_CUTLEADER_NAMESPACE()

// 修改“参数配置”的名称。
class ClExpertLibUI_Export ParamConfigNameDlg : public CLDialog
{
	DECLARE_DYNCREATE(ParamConfigNameDlg)

public:
	ParamConfigNameDlg();
	~ParamConfigNameDlg();

	enum { IDD = IDD_clExpertLib_ParamConfig_Name };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void InitName(CString strName) { m_strName = strName; }
	CString GetName() { return m_strName; }

private: // for controls
	CString m_strName;
};

END_CUTLEADER_NAMESPACE()
