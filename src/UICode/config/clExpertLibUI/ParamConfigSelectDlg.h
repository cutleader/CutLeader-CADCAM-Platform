#pragma once

#include "clExpertLibUICommon.h"
#include "DataProperty.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// —°‘Ò°∞≤Œ ˝≈‰÷√°±°£
class ClExpertLibUI_Export ParamConfigSelectDlg : public CLDialog
{
	DECLARE_DYNCREATE(ParamConfigSelectDlg)

public:
	ParamConfigSelectDlg();
	~ParamConfigSelectDlg(void);

	enum { IDD = IDD_clExpertLib_Config_Select };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnSelchangeExpLibItem();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public: // get/set functions.
	void SetCurData(LONGLONG iExpLibItemID, LONGLONG iCurConfigID);
	LONGLONG GetExpLibItemID() { return m_iExpLibItemID; }
	LONGLONG GetParamConfigID() { return m_iCurConfigID; }

private:
	// init the three combo boxes.
	void Init();
	void InitConfigCBX(LONGLONG iExpLibItemID);

private: // controls
	CComboBox m_expLibItemCBX;
	CComboBox m_paramConfigCBX;

private: // data member
	LONGLONG m_iExpLibItemID;
	LONGLONG m_iCurConfigID;
};

END_CUTLEADER_NAMESPACE()
