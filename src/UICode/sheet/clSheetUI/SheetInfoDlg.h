#pragma once

#include "clSheetUICommon.h"
#include "taoResource.h"
#include "NumEdit.h"

DECLARE_CUTLEADER_CLASS(Sheet)

BEGIN_CUTLEADER_NAMESPACE()

// use this dialog to create sheet.
class ClSheetUI_Export SheetInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(SheetInfoDlg)

public:
	SheetInfoDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SheetInfoDlg();

	enum { IDD = IDD_clSheetUI_SheetInfo };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnSelectMachine();
	afx_msg void OnSelectMat();

	DECLARE_MESSAGE_MAP()

public:
	// get the new sheet.
	SheetPtr GetSheet();

private: // for controls
	CString m_strSheetName;

	// Material Info group.
	CString m_strMat;
	CString m_strMatThick;
	CString m_strMatSize;

	// scheme info group.
	CString m_strExpLibItem;
	CString m_strParamConfig;

private: // data members
	// the sheet which will be edited.
	SheetPtr m_sheetPtr;

	// this variable record the sheet scheme that user select.
	// if user do not change the scheme, this will be "INVALID_ID".
	LONGLONG m_iParamConfigID;

	// this variable record the matSize that user select.
	// if user do not change the matSize, this will be "INVALID_ID".
	LONGLONG m_iMaterialSizeID;
};

END_CUTLEADER_NAMESPACE()
