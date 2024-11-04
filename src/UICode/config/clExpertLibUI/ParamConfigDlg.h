#pragma once

#include "taoresource.h"
#include "CLResizableDialog.h"
#include "ParamConfigPage.h"

BEGIN_CUTLEADER_NAMESPACE()

// ���������á����ý��档
class ClExpertLibUI_Export ParamConfigDlg : public CLResizableDialog
{
public:
	ParamConfigDlg(CWnd* pParent = NULL);
	~ParamConfigDlg();

	enum { IDD = IDD_clExpertLib_ParamConfig };

protected: // inherit the interface
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	DECLARE_MESSAGE_MAP()

public:
	void Init(LONGLONG iParamConfigID, int iCurrentIndex = 0);

private:
	void PlaceSubDlg();

private: // for UI.
	ParamConfigPage m_paramConfigPage;

private: // for data
	LONGLONG m_iParamConfigID;

	// ��ǰ�Ĳ��������
	int m_iCurrentIndex;
};

END_CUTLEADER_NAMESPACE()
