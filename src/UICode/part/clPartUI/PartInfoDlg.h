#pragma once

#include "clPartUICommon.h"
#include "taoResource.h"
#include "NumEdit.h"
#include "CLDialog.h"

DECLARE_CUTLEADER_CLASS(Part)

BEGIN_CUTLEADER_NAMESPACE()

// 用这个对话框创建“零件项”。
class ClPartUI_Export PartInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(PartInfoDlg)

public:
	PartInfoDlg(CWnd* pParent = NULL);
	virtual ~PartInfoDlg();

	enum { IDD = IDD_clPartUI_PartInfo };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnBnClickedOk();
	afx_msg void OnSelectScheme();

	DECLARE_MESSAGE_MAP()

public:
	// get the Part.
	PartPtr GetPart() { return m_pPart; }

private: // for controls
	CString m_strPartName;
	CString m_strExpLibItem;
	CString m_strParamConfig;

private: // data members
	// the Part which will be returned to caller.
	PartPtr m_pPart;

	LONGLONG m_iParamConfigID;
};

END_CUTLEADER_NAMESPACE()
