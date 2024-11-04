#pragma once

#include "clDataManagerCommon.h"
#include "DataProperty.h"
#include "taoresource.h"
#include "CLResizableDialog.h"
#include "CGridListCtrlGroups.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

#define WM_DATA_ITEM_SELECT_CHANGE WM_USER + 0x4E34

//
typedef enum tagDataViewOption
{
	VIEW_DATA_ALL		= 0,
	VIEW_DATA_PART		= 1,
	VIEW_DATA_SHEET		= 2,
	VIEW_DATA_NESTJOB	= 3,

} VIEW_DATA_OPTION;

// this page will display the content of the folder.
class ClDataManager_Export FolderContentPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(FolderContentPage)

public:
	FolderContentPage();
	~FolderContentPage();

	enum { IDD = IDD_clDataManagerUI_FolderContent };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected: // message mapping.
	afx_msg void OnDestroy();

	afx_msg void OnKillName();
	afx_msg void OnKillComment();

	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);

	/************************************************************************/
	// view type

	afx_msg void OnViewAll();
	afx_msg void OnViewPart();
	afx_msg void OnViewSheet();
	afx_msg void OnViewNestJob();
	/************************************************************************/

	DECLARE_MESSAGE_MAP()

public:
	// display data node list.
	void DisplayDataItemList(FolderNodePtr pParentFolder, ProductItemListPtr productItemList);

	FolderNodePtr GetFolderItem() { return m_pParentFolder; }

	// update the modify date.
	// notes:
	//   1) get the modify date from FolderNode object.
	void UpdateModifyDate();

private:
	// init the column header of the list ctrl.
	void InitGridStyle();

	// init the content of the list ctrl.
	void InitGridData(VIEW_DATA_OPTION viewOption = VIEW_DATA_ALL);

	// release the data which stored in grid.
	void ReleaseGridItemData();

private: // controls
	/************************************************************************/
	// basic info group.

	CString m_strName;
	CString m_strComment;

	CString m_strCreateDate;
	CString m_strModifyDate;
	/************************************************************************/

	CGridListCtrlGroups m_dataItemList;
	int m_iViewType;

private: // data member.
	//
	FolderNodePtr m_pParentFolder;

	// data items under "m_pParentFolder".
	ProductItemListPtr m_productItemList;
};

END_CUTLEADER_NAMESPACE()
