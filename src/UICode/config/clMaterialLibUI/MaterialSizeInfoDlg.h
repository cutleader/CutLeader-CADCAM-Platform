#pragma once

#include "clMaterialLibUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(IMaterialSize)

BEGIN_CUTLEADER_NAMESPACE()

// �����ϳߴ硱��Ϣ�Ի���
class ClMaterialLibUI_Export MaterialSizeInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(MaterialSizeInfoDlg)

public:
	MaterialSizeInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MaterialSizeInfoDlg();

	enum { IDD = IDD_clMaterialLibUI_MaterialSize_Info };

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
	IMaterialSizePtr GetMatSize() { return m_pMatSize; }

private: // for control
	CNumEdit m_ctrlMatWidth;
	CNumEdit m_ctrlMatHeight;
	CNumEdit m_ctrlMatCount;
	CString m_strComment;

private: // data members
	IMaterialSizePtr m_pMatSize;
};

END_CUTLEADER_NAMESPACE()
