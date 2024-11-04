#pragma once

#include "clOptionUICommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(ExtPropTypeList)
DECLARE_CUTLEADER_CLASS(ExtPropType)

BEGIN_CUTLEADER_NAMESPACE()

// �޸ġ��������á������ơ�
class NewExtPropDlg : public CLDialog
{
	DECLARE_DYNCREATE(NewExtPropDlg)

public:
	NewExtPropDlg();
	~NewExtPropDlg();

	enum { IDD = IDD_clOptionUI_ExtendProperty };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void Init(ExtPropTypeListPtr pExtPropTypes) { m_pExtPropTypes = pExtPropTypes; }
	ExtPropTypePtr GetExtPropType() { return m_pExtPropType; }

private: // for controls
	CString m_strName;
	CComboBox m_ctrlDataType;

private:
	// ���е���չ���ԡ�
	ExtPropTypeListPtr m_pExtPropTypes;

	// �½�����չ���ԡ�
	ExtPropTypePtr m_pExtPropType;
};

END_CUTLEADER_NAMESPACE()
