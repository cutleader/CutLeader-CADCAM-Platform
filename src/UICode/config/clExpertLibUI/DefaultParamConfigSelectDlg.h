#pragma once

#include "clExpertLibUICommon.h"
#include "DataProperty.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(DefaultParamConfig)

BEGIN_CUTLEADER_NAMESPACE()

// 设置默认的“参数配置”。
class ClExpertLibUI_Export DefaultParamConfigSelectDlg : public CLDialog
{
	DECLARE_DYNCREATE(DefaultParamConfigSelectDlg)

public:
	DefaultParamConfigSelectDlg();
	~DefaultParamConfigSelectDlg(void);

	enum { IDD = IDD_clExpertLib_DefaultConfig };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnSelectConfig();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void SetDflParamConfig(DefaultParamConfigPtr pDflParamConfig) { m_pDflParamConfig = pDflParamConfig; }

private:
	void DisplayDflParamConfig();

private: // controls
	CString m_strExpLibItemName;
	CString m_strConfigName;

private: // data member
	DefaultParamConfigPtr m_pDflParamConfig;
};

END_CUTLEADER_NAMESPACE()
