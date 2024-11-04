#pragma once

#include "clMaterialLibUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(MaterialThickness)

BEGIN_CUTLEADER_NAMESPACE()

// “材料厚度”信息对话框。
class ClMaterialLibUI_Export MaterialThicknessInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(MaterialThicknessInfoDlg)

public:
	MaterialThicknessInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MaterialThicknessInfoDlg();

	enum { IDD = IDD_clMaterialLibUI_MaterialThickness_Info };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnBnClickedOk();
	
	DECLARE_MESSAGE_MAP()

public:
	MaterialThicknessPtr GetMatThick() const { return m_pMatThick; }

private: // for controls
	CNumEdit m_dMatThickness;
	CString m_strComment;

private: // data members
	MaterialThicknessPtr m_pMatThick;
};

END_CUTLEADER_NAMESPACE()
