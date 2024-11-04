#pragma once

#include "clMaterialLibUICommon.h"
#include "DataProperty.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// “设置默认材料”对话框。
class ClMaterialLibUI_Export DefaultMaterialSelectDlg : public CLDialog
{
	DECLARE_DYNCREATE(DefaultMaterialSelectDlg)

public:
	DefaultMaterialSelectDlg();
	~DefaultMaterialSelectDlg(void);

	enum { IDD = IDD_clMaterialLibUI_DefaultMaterial };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnSelectMat();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void SetDflMatSize(LONGLONG iDflMatSizeID) { m_iDflMatSizeID = iDflMatSizeID; }
	LONGLONG GetDflMatSize() { return m_iDflMatSizeID; }

private: // controls
	CString m_strMat;
	CString m_strMatThick;
	CString m_strMatSize;

private: // data member
	LONGLONG m_iDflMatSizeID;
};

END_CUTLEADER_NAMESPACE()
