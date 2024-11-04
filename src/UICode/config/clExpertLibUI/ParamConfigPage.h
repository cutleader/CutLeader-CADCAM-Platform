#pragma once

#include "taoresource.h"
#include "CLResizableDialog.h"
#include "IExpertLibNode.h"
#include "LeadInOutPage.h"
#include "PierceLocationPage.h"
#include "MicroJointAssignmentPage.h"
#include "CornerAssignPage.h"
#include "ToolAssignPage.h"
#include "SequenceSetPage.h"
#include "NCConfigPage.h"
#include "NestConfigItemPage.h"

BEGIN_CUTLEADER_NAMESPACE()

// ���������á�ҳ�档
class ClExpertLibUI_Export ParamConfigPage : public CLResizableDialog
{
	DECLARE_DYNAMIC(ParamConfigPage)

public:
	ParamConfigPage(CWnd* pParent = NULL);
	virtual ~ParamConfigPage();

	enum { IDD = IDD_clExpertLibUI_ParamConfig };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangedItem(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	// ��ʾ���ҳ�档
	void DisplayPage(LONGLONG iParamConfigID, int iCurrentIndex = 0);

	/************************************************************************/
	// about save the modification.

	// when select-changed event happened on the tree/listctrl node, call
	// this to check whether we should save the data on the previous page.
	// notes:
	//   1) call "SaveModification" in this function.
	//   2) display message in this function.
	void CheckModification();

	// save modifications that were made to previous page.
	/* params:
	*  bNeedConfirm: whether need user to confirm save.
	*/
	// notes:
	//   1) if no modifications made to previous page, return FALSE.
	BOOL SaveModification(BOOL bNeedConfirm = TRUE);

	// check whether some page is modified.
	BOOL DocModified();
	/************************************************************************/

	// indicate which scheme item in listctrl is selected.
	EXPLIB_NODE_TYPE GetCurNodeType() { return m_expLibNodeType; }

private:
	/************************************************************************/
	// about the listctrl.

	// init the image list which will be used in listctrl.
	void InitImageList();

	// init the listctrl according to the scheme type.
	void InitListContent();
	/************************************************************************/

	/************************************************************************/
	// about scheme item sub-pages.

	// init sub-dialogs, these dialogs will be shown
	// when user select the icons in the listctrl.
	void InitSubDialog();

	// hide all sub-dialogs.
	void HideAllSubDialog();

	// hide all sub pages and display the proper page according to 
	// "m_expLibNodeType" on the proper position.
	void RepositionSubDlg();
	/************************************************************************/

private: // controls
	// for the listctrl.
	CListCtrl m_schemeItemList;
	CImageList	m_imageList;

	// ���ҳ���е���ҳ�档
    ToolAssignPage m_toolAssignPage;
	LeadInOutPage m_leadInOutPage;
	PierceLocationPage m_pierceLocPage;
	MicroJointAssignmentPage m_microJointAssignmentPage;
	CornerAssignPage m_cornerAssignPage;
	SequenceSetPage m_seqSetPage;
	NCConfigPage m_NCConfigPage;
	NestConfigItemPage m_nestParamPage;

private: // for data.
	// ���������á�ID��
	LONGLONG m_iParamConfigID;

	// indicate which scheme item in listctrl is selected.
	// notes:
	//   1) update this when select-changed happened on listctrl.
	EXPLIB_NODE_TYPE m_expLibNodeType;
};

END_CUTLEADER_NAMESPACE()
