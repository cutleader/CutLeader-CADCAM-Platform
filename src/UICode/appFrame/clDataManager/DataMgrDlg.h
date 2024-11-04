#pragma once

#include "clDataManagerCommon.h"
#include "ProdNodeData.h"
#include "ProductTree.h"
#include "ExplorerViewHelper.h"
#include "FolderContentPage.h"
#include "PartInfoPage.h"
#include "SheetInfoPage.h"
#include "NestJobInfoPage.h"
#include "ItemListPage.h"

BEGIN_CUTLEADER_NAMESPACE()

// the resource explorer view.
class ClDataManager_Export DataMgrDlg : public CLResizableDialog
{
	DECLARE_DYNCREATE(DataMgrDlg)

public:
	DataMgrDlg(CWnd* pParent = NULL);
	virtual ~DataMgrDlg();

	enum { IDD = IDD_clDataManager_Explorer };

public: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	/************************************************************************/
	// handle the message for tree.

	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);

	// db-click to open the part/sheet.
	// notes:
	//   1) send "WM_DATA_ITEM_DB_CLICK" message to app, place "DataItem*" info in "wParam".
	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnRClickTree(NMHDR* pNMHDR, LRESULT* pResult);

	// XTreeCtrl object will move the tree node, so this class will care all other processes.
	afx_msg LRESULT OnTreeNodeMoved(WPARAM wParam, LPARAM lParam);

	// before copy tree node, this class should make sure whether we can do this. 
	// e.g. we cannot copy-drop nest job and the sheet which is created from a skeleton mat.
	// to be implemented: when copy part, should check whether it is opened and un-saved.
	afx_msg LRESULT OnTreeNodePreCopied(WPARAM wParam, LPARAM lParam);

	// XTreeCtrl object will copy the tree node, so this class will care all other processes.
	afx_msg LRESULT OnTreeNodeCopied(WPARAM wParam, LPARAM lParam);
	/************************************************************************/

	/************************************************************************/
	// handler of the menu items.

	afx_msg void OnCreateFolder();
	afx_msg void OnCreatePart();
	afx_msg void OnCreateSheet();

	afx_msg void OnImport();
	afx_msg void OnSplitImport();

	// to be implemented: when export part, should check whether it is opened and un-saved.
	afx_msg void OnExport();

	afx_msg void OnViewDepend();
	afx_msg void OnDeleteItem();
	/************************************************************************/

	/************************************************************************/
	// messages from the sub-pages.

	// FolderContentPage send WM_DATA_ITEM_DB_CLICK message to notify
	// this function that we should navigate to the DataNode.
	afx_msg LRESULT OnDataItemDbClick(WPARAM wParam, LPARAM lParam);
	/************************************************************************/

	afx_msg void OnExplorerOpen();
	afx_msg void OnExplorerSave();
	afx_msg void OnExplorerCancel();

	DECLARE_MESSAGE_MAP()

public: // get/set functions.
	void InitNaviNodeID(LONGLONG iNaviNodeID) { m_iNaviNodeID = iNaviNodeID; }

	DataItemPtr GetOpeningDataItem() { return m_pOpeningDataItem; }

public:
	// save the content of the current page.
	// notes:
	//   1) if no changes made, do not save.
	void SaveCurrentPage();

	// check whether some page is modified in resource explorer.
	BOOL DocModified();

private:
	/************************************************************************/
	// about sub-pages.

	// init sub-dialogs, these dialogs will be shown
	// when user select the proper tree node.
	void InitSubDialog();

	// hide all sub-dialogs.
	void HideAllSubDialog();

	// hide all sub pages and display the proper page according to 
	// "m_expLibNodeType" on the proper position.
	void RepositionSubDlg();
	/************************************************************************/

	// save modifications that were made to "hItem".
	/* params:
	*  bNeedConfirm: whether need user to confirm save.
	*/
	// notes:
	//   1) if no modifications made to "hItem", return FALSE.
	BOOL SaveModification(HTREEITEM hItem, BOOL bNeedConfirm = TRUE);

	// open the data item.
	void OpenDataItem(DataItemPtr pDataItem);

private: // for UI.
	/************************************************************************/
	// for the product tree.

	// the tree control.
	ProductTree m_productTree;

	// use this ID to navigate a node.
	// notes:
	//   1) if "m_iNaviNodeID" is INVALID_ID, navigate to the root node.
	LONGLONG m_iNaviNodeID;
	/************************************************************************/

	/************************************************************************/
	// the sub pages in this sheet.

	// this page display the content of the folder node.
	FolderContentPage m_folderContentPage;

	// this page display the content of the part node.
	PartInfoPage m_partInfoPage;

	// this page display the content of the sheet node.
	SheetInfoPage m_sheetInfoPage;

	// this page display the content of the nest job node.
	NestJobInfoPage m_nestJobInfoPage;

	// this page display the content of the selected nodes.
	ItemListPage m_itemListPage;
	/************************************************************************/

private: // data member
	// indicate which node has been selected.
	PRODUCT_NODE_TYPE m_productNodeType;

	// the data item which will be opened.
	DataItemPtr m_pOpeningDataItem;

	// the workspace node, we can search any node through this node.
	FolderNodePtr m_pWorkspace;

	// the helper class.
	ExplorerViewHelper m_explorerViewHelper;

	// use this brush to paint the background.
	CBrush m_brush;
};

END_CUTLEADER_NAMESPACE()
