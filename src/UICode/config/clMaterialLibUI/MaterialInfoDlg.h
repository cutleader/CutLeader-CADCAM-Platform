#pragma once

#include "clMaterialLibUICommon.h"
#include "taoresource.h"
#include "NumEdit.h"

DECLARE_CUTLEADER_CLASS(Material)

BEGIN_CUTLEADER_NAMESPACE()

// “材料”信息对话框。
class ClMaterialLibUI_Export MaterialInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(MaterialInfoDlg)

public:
	MaterialInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MaterialInfoDlg();

	enum { IDD = IDD_clMaterialLibUI_Material_Info };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	// get the new material size object.
	// notes:
	//   1) if click cancel, return NULL.
	MaterialPtr GetMat() { return m_pMat; }

private: // for controls.
	CString m_strName;
	CNumEdit m_dGravity;
	CString m_strComment;

private: // data member
	MaterialPtr m_pMat;
};

END_CUTLEADER_NAMESPACE()
