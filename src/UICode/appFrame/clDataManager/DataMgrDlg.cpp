#include "stdafx.h"
#include "DataMgrDlg.h"

#include "DateHelper.h"
#include "IntegerList.h"
#include "clUtilityResource.h"
#include "XMenu.h"

#include "versionInfo.h"
#include "LogMessage.h"
#include "LogMgr.h"
#include "baseConst.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"
#include "ProductItemWriter.h"

#include "PatternProcessor.h"
#include "Part.h"
#include "PatternList.h"
#include "PartCadData.h"
#include "PartItem.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "PartBuilder.h"

#include "clDataImportExportResource.h"

#include "Sheet.h"
#include "SheetLoader.h"
#include "SheetWriter.h"
#include "SheetManager.h"
#include "SheetInfoDlg.h"

#include "NestJob.h"
#include "NestJobMaterialList.h"
#include "NestJobMaterial.h"
#include "NestScheduleDlg.h"
#include "NestJobLoader.h"
#include "NestJobWriter.h"
#include "NestJobMgr.h"

#include "ProductItemLoader.h"
#include "ProductItemList.h"
#include "ProductItemBase.h"
#include "FolderItem.h"
#include "DataItem.h"
#include "ProdNodeData.h"
#include "clDataManagerResource.h"
#include "FolderInfoDlg.h"
#include "ProductItemWriter.h"
#include "DependNodeMgr.h"
#include "DependMsgDlg.h"
#include "DependViewDlg.h"
#include "DependNode.h"
#include "DependNodeList.h"
#include "PartRepairDlg.h"
#include "SheetRepairDlg.h"
#include "DataItemTracker.h"

#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "clOptionResource.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(DataMgrDlg, CResizableDialog)

DataMgrDlg::DataMgrDlg(CWnd* pParent /*=NULL*/)
	: CLResizableDialog(DataMgrDlg::IDD, pParent)
{
	m_iNaviNodeID = INVALID_ID;
	m_productNodeType = PRODUCT_NODE_NOT_INIT;
}

DataMgrDlg::~DataMgrDlg()
{
}

BEGIN_MESSAGE_MAP(DataMgrDlg, CResizableDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()

	/************************************************************************/
	// the message for tree.

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PRODUCT, OnSelchangedTree)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PRODUCT, OnDblclkTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PRODUCT, OnRClickTree)

	// messages from the sub-pages.
	ON_MESSAGE(WM_DATA_ITEM_DB_CLICK, OnDataItemDbClick)

	// messages from the controls.
	ON_MESSAGE(WM_TREE_NODE_MOVED, OnTreeNodeMoved)
	ON_MESSAGE(WM_TREE_NODE_PRE_COPIED, OnTreeNodePreCopied)
	ON_MESSAGE(WM_TREE_NODE_COPIED, OnTreeNodeCopied)
	/************************************************************************/

	/************************************************************************/
	// for the popup menu.

	ON_COMMAND(IDC_CREATE_FOLDER, OnCreateFolder)
	ON_COMMAND(IDC_CREATE_PART, OnCreatePart)
	ON_COMMAND(IDC_CREATE_SHEET, OnCreateSheet)

	ON_COMMAND(IDC_EXPL_IMPORT, OnImport)
	ON_COMMAND(IDC_EXPL_SPLIT_IMPORT, OnSplitImport)
	ON_COMMAND(IDC_EXPL_EXPORT, OnExport)

	ON_COMMAND(IDC_VIEW_DEPEND, OnViewDepend)
	ON_COMMAND(IDC_DELETE_PRODUCT_ITEM, OnDeleteItem)
	/************************************************************************/

	ON_BN_CLICKED(IDC_BTN_EXPLORER_OPEN, OnExplorerOpen)
	ON_BN_CLICKED(IDC_BTN_EXPLORER_SAVE, OnExplorerSave)
	ON_BN_CLICKED(IDC_BTN_EXPLORER_CANCEL, OnExplorerCancel)
END_MESSAGE_MAP()

void DataMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_PRODUCT, m_productTree);
}

void DataMgrDlg::InitSubDialog()
{
	//
	m_folderContentPage.Create(IDD_FOLDER_CONTENT_PAGE, this);
	m_folderContentPage.ShowWindow(SW_HIDE);

	//
	m_partInfoPage.Create(IDD_PARTINFO_PAGE, this);
	m_partInfoPage.ShowWindow(SW_HIDE);

	//
	m_sheetInfoPage.Create(IDD_SHEETINFO_PAGE, this);
	m_sheetInfoPage.ShowWindow(SW_HIDE);

	//
	m_nestJobInfoPage.Create(IDD_NESTJOBINFO_PAGE, this);
	m_nestJobInfoPage.ShowWindow(SW_HIDE);

	//
	m_itemListPage.Create(IDD_clDataManager_ItemList, this);
	m_itemListPage.ShowWindow(SW_HIDE);
}

void DataMgrDlg::RepositionSubDlg()
{
	// no node selected, of course no page to display.
	if (m_productNodeType == PRODUCT_NODE_NOT_INIT)
		return;

	/************************************************************************/
	// calculate the position of the child page.

	CWnd* pWndPos = GetDlgItem(IDC_STATIC_CHILD_AREA);
	CRect pagePos;
	if (pWndPos)
	{
		pWndPos->GetWindowRect(&pagePos);
		ScreenToClient(&pagePos);
	}
	/************************************************************************/

	/************************************************************************/
	// because the layout library will re-layout the controls, so we should
	// re-position the sub dialog.

	HideAllSubDialog();

	if (m_productNodeType == PRODUCT_NODE_FOLDER)
	{
		m_folderContentPage.SetWindowPos(pWndPos, pagePos.left, pagePos.top, pagePos.Width(), pagePos.Height(), 0);
		m_folderContentPage.ShowWindow(SW_SHOW);
	}
	else if (m_productNodeType == PRODUCT_NODE_PART)
	{
		m_partInfoPage.SetWindowPos(pWndPos, pagePos.left, pagePos.top, pagePos.Width(), pagePos.Height(), 0);
		m_partInfoPage.ShowWindow(SW_SHOW);
	}
	else if (m_productNodeType == PRODUCT_NODE_SHEET)
	{
		m_sheetInfoPage.SetWindowPos(pWndPos, pagePos.left, pagePos.top, pagePos.Width(), pagePos.Height(), 0);
		m_sheetInfoPage.ShowWindow(SW_SHOW);
	}
	else if (m_productNodeType == PRODUCT_NODE_NESTJOB)
	{
		m_nestJobInfoPage.SetWindowPos(pWndPos, pagePos.left, pagePos.top, pagePos.Width(), pagePos.Height(), 0);
		m_nestJobInfoPage.ShowWindow(SW_SHOW);
	}
	else if (m_productNodeType == PRODUCT_NODE_MUL_NODE)
	{
		m_itemListPage.SetWindowPos(pWndPos, pagePos.left, pagePos.top, pagePos.Width(), pagePos.Height(), 0);
		m_itemListPage.ShowWindow(SW_SHOW);
	}
	/************************************************************************/
}

BOOL DataMgrDlg::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	// layout the controls.
	AddAnchor(IDC_GROUP_TREE, TOP_LEFT, ANCHOR(30,100));
	AddAnchor(IDC_TREE_PRODUCT, TOP_LEFT, ANCHOR(30,100));
	AddAnchor(IDC_STATIC_CHILD_AREA, ANCHOR(30,0), BOTTOM_RIGHT);
	AddAnchor(IDC_BTN_EXPLORER_OPEN, BOTTOM_RIGHT);
	AddAnchor(IDC_BTN_EXPLORER_APPLY, BOTTOM_RIGHT);
	AddAnchor(IDC_BTN_EXPLORER_CANCEL, BOTTOM_RIGHT);

	// init the product tree.
	m_explorerViewHelper.Init(&m_productTree);

	// init sub dialogs.
	InitSubDialog();

	/************************************************************************/
	// init the tree.

	m_productTree.Init();

	// load the root node.
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	m_pWorkspace = ProductItemLoader::LoadProductTree(pMainDBCon);

	// display the tree.
	m_productTree.InitTree(m_pWorkspace);

	// navigate to a specified node.
	m_productTree.NavigateToNode(m_iNaviNodeID);
	/************************************************************************/

	UpdateData(FALSE);

	return TRUE;
}

BOOL DataMgrDlg::DocModified()
{
	/************************************************************************/
	// prepare some data.

	HTREEITEM hItem = m_productTree.GetSelectedItem();
	if (!hItem)
		return FALSE;

	// get the ProductNodeData which is saved in the tree node.
	UINT dataVal = m_productTree.GetItemData(hItem);
	ProductNodeData* pData = (ProductNodeData*)dataVal;
	/************************************************************************/

	if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
	{
		// force all edit boxes lose focus.
		m_folderContentPage.SetFocus();

		// check whether we should save the folder item.
		FolderNodePtr pFolderNode = m_folderContentPage.GetFolderItem();
		if (pFolderNode && pFolderNode->IsModified())
			return TRUE;
	}
	else if (pData->GetProductNodeType() == PRODUCT_NODE_PART)
	{
		// force all edit boxes lose focus.
		m_partInfoPage.SetFocus();

		// check whether we should save the part.
		PartPtr pPart = m_partInfoPage.GetPart();
		if (pPart && pPart->IsModified())
			return TRUE;
	}
	else if (pData->GetProductNodeType() == PRODUCT_NODE_SHEET)
	{
		// force all edit boxes lose focus.
		m_sheetInfoPage.SetFocus();

		// check whether we should save the sheet.
		SheetPtr pSheet = m_sheetInfoPage.GetSheet();
		if (pSheet && pSheet->IsModified())
			return TRUE;
	}
	else if (pData->GetProductNodeType() == PRODUCT_NODE_NESTJOB)
	{
		// force all edit boxes lose focus.
		m_nestJobInfoPage.SetFocus();

		// check whether we should save the nest job.
		NestJobPtr pNestJob = m_nestJobInfoPage.GetNestJob();
		if (pNestJob && pNestJob->IsModified())
			return TRUE;
	}

	return FALSE;
}

void DataMgrDlg::SaveCurrentPage()
{
	/************************************************************************/
	// prepare some data here.

	HTREEITEM hItem = m_productTree.GetSelectedItem();
	if (!hItem)
		return;

	// get the ProductNodeData which is saved in the tree node.
	UINT dataVal = m_productTree.GetItemData(hItem);
	ProductNodeData* pData = (ProductNodeData*)dataVal;
	PRODUCT_NODE_TYPE nodeType = pData->GetProductNodeType();
	ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pData->GetProductNodeID());

	// for message.
	LogMessagePtr pLogMsg;
	CString strTaskName, strMsgContent;
	/************************************************************************/

	// check whether can save the page we will leave.
	if (SaveModification(hItem, FALSE))
	{
		// prepare the message.
		if (nodeType == PRODUCT_NODE_PART)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_PART_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_PAGE_SAVED);
		}
		else if (nodeType == PRODUCT_NODE_SHEET)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_SHEET_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_PAGE_SAVED);
		}
		else if (nodeType == PRODUCT_NODE_NESTJOB)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_NEST_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTJOB_PAGE_SAVED);
		}
		else if (nodeType == PRODUCT_NODE_FOLDER)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_FOLDER_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_FOLDER_PAGE_SAVED);
		}
		strMsgContent.Replace(_T("WILLBEREPLACED1"), pProductItem->GetName());
		pLogMsg.reset(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));
	}
	else
	{
		// prepare the message.
		if (nodeType == PRODUCT_NODE_PART)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_PART_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_PAGE_NOT_CHANGED);
		}
		else if (nodeType == PRODUCT_NODE_SHEET)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_SHEET_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_PAGE_NOT_CHANGED);
		}
		else if (nodeType == PRODUCT_NODE_NESTJOB)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_NEST_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTJOB_PAGE_NOT_CHANGED);
		}
		else if (nodeType == PRODUCT_NODE_FOLDER)
		{
			strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_FOLDER_PAGE);
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_FOLDER_PAGE_NOT_CHANGED);
		}
		strMsgContent.Replace(_T("WILLBEREPLACED1"), pProductItem->GetName());
		pLogMsg.reset(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_WARNING, strTaskName, strMsgContent));
	}

	// write the log message.
	LogMgr::WriteLog(pLogMsg);

	// prompt user.
	CString strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	MessageBox(strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
}

void DataMgrDlg::HideAllSubDialog()
{
	m_folderContentPage.ShowWindow(SW_HIDE);
	m_partInfoPage.ShowWindow(SW_HIDE);
	m_sheetInfoPage.ShowWindow(SW_HIDE);
	m_nestJobInfoPage.ShowWindow(SW_HIDE);
	m_itemListPage.ShowWindow(SW_HIDE);
}

void DataMgrDlg::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	// re-position the sub dialog.
	RepositionSubDlg();
}

BOOL DataMgrDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH DataMgrDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CResizableDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void DataMgrDlg::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_productTree.CanIgnore_selectChange_event())
	{
		return;
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TVITEM newItem = pNMTreeView->itemNew;
	if (newItem.hItem == 0)
		return;
	TVITEM oldItem = pNMTreeView->itemOld;

	// disable the "open" button.
	((CWnd*)GetDlgItem(IDC_BTN_EXPLORER_OPEN))->EnableWindow(FALSE);

	/************************************************************************/
	// check the modifications in the previous page.

	// if "oldItem.hItem" is NULL, we know this is the first time to select a node.
	if (oldItem.hItem)
	{
		// we should check whether we should save the data on the previous page.
		if (SaveModification(oldItem.hItem))
		{
			// get the ProductNodeData which is saved in the tree node.
			UINT iOldDataVal = m_productTree.GetItemData(oldItem.hItem);
			ProductNodeData* pOldData = (ProductNodeData*)iOldDataVal;
			PRODUCT_NODE_TYPE iOldNodeType = pOldData->GetProductNodeType();
			ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pOldData->GetProductNodeID());

			/************************************************************************/
			// display the message on the message bar.

			// prepare the message.
			CString strTaskName, strMsgContent;
			if (iOldNodeType == PRODUCT_NODE_PART)
			{
				strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_PART_PAGE);
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_PAGE_SAVED);
			}
			else if (iOldNodeType == PRODUCT_NODE_SHEET)
			{
				strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_SHEET_PAGE);
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_PAGE_SAVED);
			}
			else if (iOldNodeType == PRODUCT_NODE_NESTJOB)
			{
				strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_NEST_PAGE);
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTJOB_PAGE_SAVED);
			}
			else if (iOldNodeType == PRODUCT_NODE_FOLDER)
			{
				strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_FOLDER_PAGE);
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_FOLDER_PAGE_SAVED);
			}
			strMsgContent.Replace(_T("WILLBEREPLACED1"), pProductItem->GetName());
			LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));

			// write the log message.
			LogMgr::WriteLog(pLogMsg);
			/************************************************************************/
		}
	}
	/************************************************************************/

	// the new node.
	BOOL bNodeTypeChanged = FALSE;
	ProductNodeData* pNewData = (ProductNodeData*)m_productTree.GetItemData(newItem.hItem);
	if (m_productNodeType != pNewData->GetProductNodeType())
	{
		m_productNodeType = pNewData->GetProductNodeType();
		bNodeTypeChanged = TRUE;
	}

	/************************************************************************/
	// process the case that more than one items are selected or no items are selected.

	int iSelNodeCount = m_productTree.GetSelectedCount();
	if (iSelNodeCount > 1)
	{
		m_productNodeType = PRODUCT_NODE_MUL_NODE;

		// get the selected node.
		vector<HTREEITEM> selNodeList = m_productTree.GetSelectedList();

		// collect all selected product nodes.
		ProductItemListPtr pProductItemList(new ProductItemList);
		for (unsigned int i = 0; i < selNodeList.size(); i++)
		{
			// get the product item.
			HTREEITEM hItem = selNodeList.at(i);
			ProductNodeData* pTmpData = (ProductNodeData*)m_productTree.GetItemData(hItem);
			ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pTmpData->GetProductNodeID());

			pProductItemList->push_back(pProductItem);
		}

		// display ItemListPage.
		RepositionSubDlg();
		m_itemListPage.DisplayItemList(pProductItemList);

		return;
	}
	else if (iSelNodeCount == 0)
		return;
	/************************************************************************/

	// if reach here, one node is selected.

	// update the last selected node.
	DataItemTracker::GetInstance()->SetLastSelNodeID(pNewData->GetProductNodeID());

	// re-position the sub dialog.
	if (bNodeTypeChanged)
		RepositionSubDlg();

	// display the page according to the node selected.
	if (pNewData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
	{
		// get the selected folder item.
		ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pNewData->GetProductNodeID());
		FolderNodePtr pTmpFolderNode = boost::dynamic_pointer_cast<FolderNode>(pProductItem);

		// get all data nodes under this folder item.
		ProductItemListPtr pDataItemList(new ProductItemList);
		pTmpFolderNode->GetAllDataItem(pDataItemList);

		// display FolderContentPage.
		m_folderContentPage.DisplayDataItemList(pTmpFolderNode, pDataItemList);
	}
	else if (pNewData->GetProductNodeType() == PRODUCT_NODE_PART)
	{
		// get the selected part node.
		ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pNewData->GetProductNodeID());
		DataItemPtr pPartNode = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		// load the part.
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(pPartNode->GetDataID());
		PartPtr pPart = PartLoader::LoadPart(pPartDBCon, pPartNode->GetDataID(), PARTLOAD_PREVIEW);

		// display PartInfoPage.
		m_partInfoPage.DisplayPartInfo(pPart);

		((CWnd*)GetDlgItem(IDC_BTN_EXPLORER_OPEN))->EnableWindow(TRUE);
	}
	else if (pNewData->GetProductNodeType() == PRODUCT_NODE_SHEET)
	{
		// get the selected sheet node.
		ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pNewData->GetProductNodeID());

		// load the sheet.
		DataItemPtr pSheetNode = boost::dynamic_pointer_cast<DataItem>(pProductItem);
		DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(pSheetNode->GetDataID());
		SheetPtr pSheet = SheetLoader::LoadSheet(pShtDBCon, pSheetNode->GetDataID(), SHEETLOAD_PREVIEW);

		// display SheetInfoPage.
		m_sheetInfoPage.DisplaySheetInfo(pSheet);

		((CWnd*)GetDlgItem(IDC_BTN_EXPLORER_OPEN))->EnableWindow(TRUE);
	}
	else if (pNewData->GetProductNodeType() == PRODUCT_NODE_NESTJOB)
	{
		// get the selected nest job node.
		ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pNewData->GetProductNodeID());
		DataItemPtr pNestJobNode = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		// load the nest job.
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		NestJobPtr pNestJob = NestJobLoader::LoadNestJob(pMainDBCon, pNestJobNode->GetDataID(), FALSE, false, SHEETLOAD_BASIC);

		// display NestJobInfoPage.
		m_nestJobInfoPage.DisplayNestJobInfo(pNestJob);
	}
}

void DataMgrDlg::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// get the selected tree item.
	HTREEITEM hItem = m_productTree.GetSelectedItem();
	if (!hItem)
		return;

	// get the selected ProductItem object.
	UINT dataVal = m_productTree.GetItemData(hItem);
	ProductNodeData* pData = (ProductNodeData*)dataVal;
	ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pData->GetProductNodeID());

	// if we db-click data item.
	if (!pProductItem->IsFolder())
	{
		DataItemPtr pSelDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
		OpenDataItem(pSelDataItem);
	}

	*pResult = 0;
}

void DataMgrDlg::OnRClickTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	/************************************************************************/
	// check whether click on the item.

	// convert the coordinate from screen to client.
	POINT pos;
	if (!GetCursorPos(&pos))
		return;
	m_productTree.ScreenToClient(&pos);

	// check the hit item.
	UINT uFlags;
	HTREEITEM hItem = m_productTree.HitTest(pos, &uFlags);
	if ((hItem == NULL) || !(uFlags & TVHT_ONITEM))
		return;
	else
	{
		if (!m_productTree.IsSelected(hItem))
		{
			m_productTree.SelectAll(FALSE);
			m_productTree.SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
		}
	}
	/************************************************************************/

	/************************************************************************/
	// display the popup menu.
	// notes:
	//   1) if license do not include nesting module, disable the menu item.

	// figure out the position where the menu will pop up.
	m_productTree.ClientToScreen(&pos);

	// display different popup.
	CString str;
	XMenu xMenu;
	xMenu.CreatePopupMenu();
	int iSelNodeCount = m_productTree.GetSelectedCount();
	if (iSelNodeCount > 1)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DELETE);
		xMenu.AppendMenu(MF_STRING, IDC_DELETE_PRODUCT_ITEM, str.AllocSysString());
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_VIEW_DEPEND);
		xMenu.AppendMenu(MF_STRING, IDC_VIEW_DEPEND, str.AllocSysString());
	}
	else if (iSelNodeCount == 1)
	{
		// get the select node.
		vector<HTREEITEM> selItemList = m_productTree.GetSelectedList();
		HTREEITEM hItem = selItemList.at(0);
		UINT dataVal = m_productTree.GetItemData(hItem);
		ProductNodeData* pData = (ProductNodeData*)dataVal;

		// the folder node will show the different popup with others.
		if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CREATE_PART);
			xMenu.AppendMenu(MF_STRING, IDC_CREATE_PART, str.AllocSysString());
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CREATE_SHEET);
			xMenu.AppendMenu(MF_STRING, IDC_CREATE_SHEET, str.AllocSysString());

			// folder
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CREATE_FOLDER);
			xMenu.AppendMenu(MF_STRING, IDC_CREATE_FOLDER, str.AllocSysString());

			// import.
			xMenu.AppendMenu(MF_SEPARATOR);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT);
			xMenu.AppendMenu(MF_STRING, IDC_EXPL_IMPORT, str.AllocSysString());
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SPLIT_IMPORT_DATA);
			xMenu.AppendMenu(MF_STRING, IDC_EXPL_SPLIT_IMPORT, str.AllocSysString());

			//
			xMenu.AppendMenu(MF_SEPARATOR);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DELETE);
			xMenu.AppendMenu(MF_STRING, IDC_DELETE_PRODUCT_ITEM, str.AllocSysString());
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_VIEW_DEPEND);
			xMenu.AppendMenu(MF_STRING, IDC_VIEW_DEPEND, str.AllocSysString());
		}
		else if (pData->GetProductNodeType() == PRODUCT_NODE_PART ||
				 pData->GetProductNodeType() == PRODUCT_NODE_SHEET ||
				 pData->GetProductNodeType() == PRODUCT_NODE_NESTJOB)
		{
			// export.
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXPORT);
			xMenu.AppendMenu(MF_STRING, IDC_EXPL_EXPORT, str.AllocSysString());
			xMenu.AppendMenu(MF_SEPARATOR);

			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DELETE);
			xMenu.AppendMenu(MF_STRING, IDC_DELETE_PRODUCT_ITEM, str.AllocSysString());
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_VIEW_DEPEND);
			xMenu.AppendMenu(MF_STRING, IDC_VIEW_DEPEND, str.AllocSysString());
		}
	}
	xMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pos.x, pos.y, this);
	/************************************************************************/

	*pResult = 0;
}

LRESULT DataMgrDlg::OnTreeNodePreCopied(WPARAM wParam, LPARAM lParam)
{
	PRE_COPY_MSG* pPreCopyMsg = (PRE_COPY_MSG*)wParam;

	// get the DataItem which will be copied.
	ProductNodeData* pData = (ProductNodeData*)m_productTree.GetItemData(pPreCopyMsg->hTreeNode);
	ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pData->GetProductNodeID());

	// check whether can copy it.
	DataItemPtr pSrcDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
	if (pSrcDataItem->GetDataType() == DATANODE_NESTJOB)
	{
		pPreCopyMsg->bCanCopy = FALSE;
		pPreCopyMsg->strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CANNOT_COPY_NEST);
	}

	return true;
}

LRESULT DataMgrDlg::OnTreeNodeCopied(WPARAM wParam, LPARAM lParam)
{
	// get the copied tree node.
	HTREEITEM newTreeItem = (HTREEITEM)wParam;

	// get the items which will be moved.
	vector<HTREEITEM> dragItemList = m_productTree.GetDragItems();
	HTREEITEM dropTargetItem = m_productTree.GetDropTargetItem();

	// ExplorerViewHelper will help me to finish this.
	m_explorerViewHelper.CopyItem(m_pWorkspace, dragItemList[0], newTreeItem, dropTargetItem);

	return true;
}

LRESULT DataMgrDlg::OnDataItemDbClick(WPARAM wParam, LPARAM lParam)
{
	ProductNodeData* pProductNodeData = reinterpret_cast<ProductNodeData*>(wParam);

	// navigate to the node.
	m_iNaviNodeID = pProductNodeData->GetProductNodeID();
	m_productTree.NavigateToNode(m_iNaviNodeID);

	return true;
}

void DataMgrDlg::OnCreateFolder()
{
	m_explorerViewHelper.CreateFolder(m_pWorkspace);
}

void DataMgrDlg::OnCreatePart()
{
	m_explorerViewHelper.CreatePart(m_pWorkspace);
}

void DataMgrDlg::OnCreateSheet()
{
	m_explorerViewHelper.CreateSheet(m_pWorkspace);
}

void DataMgrDlg::OnImport()
{
	m_explorerViewHelper.Import(m_pWorkspace);
}

void DataMgrDlg::OnSplitImport()
{
	m_explorerViewHelper.SplitImport(m_pWorkspace);
}

void DataMgrDlg::OnExport()
{
	m_explorerViewHelper.Export(m_pWorkspace);
}

void DataMgrDlg::OnViewDepend()
{
	m_explorerViewHelper.ViewDepend(m_pWorkspace, m_hWnd);
}

void DataMgrDlg::OnDeleteItem()
{
	m_explorerViewHelper.DeleteSelItem(m_pWorkspace, m_hWnd);
}

void DataMgrDlg::OnExplorerOpen()
{
	// get the selected data object.
	HTREEITEM hItem = m_productTree.GetFirstSelectedItem();
	ProductNodeData* pData = (ProductNodeData*)m_productTree.GetItemData(hItem);
	ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pData->GetProductNodeID());

	// open it.
	DataItemPtr pSelDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
	OpenDataItem(pSelDataItem);
}

void DataMgrDlg::OnExplorerSave()
{
	CString strProductName, str;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	HTREEITEM hItem = m_productTree.GetSelectedItem();
	if (SaveModification(hItem, FALSE))
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CHANGE_SAVED);
		MessageBox(str, strProductName, MB_ICONINFORMATION);
	}
	else
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_CHANGE);
		MessageBox(str, strProductName, MB_ICONWARNING);
	}
}

void DataMgrDlg::OnExplorerCancel()
{
	EndDialog(IDC_BTN_EXPLORER_CANCEL);
}

BOOL DataMgrDlg::SaveModification(HTREEITEM hItem, BOOL bNeedConfirm)
{
	BOOL bModified = FALSE;

	CString strProductName, str;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();

	// get the ProductNodeData which is saved in the tree node.
	UINT dataVal = m_productTree.GetItemData(hItem);
	ProductNodeData* pData = (ProductNodeData*)dataVal;

	if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
	{
		// force all edit boxes lose focus.
		m_folderContentPage.SetFocus();

		// check whether we should save the folder item.
		FolderNodePtr pFolderNode = m_folderContentPage.GetFolderItem();
		if (pFolderNode && pFolderNode->IsModified())
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SAVE_FOLDER);
			if ((bNeedConfirm && MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES) || !bNeedConfirm)
			{
				bModified = TRUE;

				/************************************************************************/
				// save the basic data of folder.

				// since we modified the folder, we should update the modify date.
				pFolderNode->SetModifyTime(DateHelper::GetCurrentTime());

				// save the "basic info" of the part.
				ProductItemWriter::UpdateFolderBasicData(pMainDBCon, pFolderNode);
				/************************************************************************/

				/************************************************************************/
				// update the UI.

				// update the name of the tree node.
				m_productTree.SetItemText(hItem, pFolderNode->GetName());

				// update the page.
				m_folderContentPage.UpdateModifyDate();
				/************************************************************************/
			}
		}
	}
	else if (pData->GetProductNodeType() == PRODUCT_NODE_PART)
	{
		// force all edit boxes lose focus.
		m_partInfoPage.SetFocus();

		// check whether we should save the part.
		PartPtr pPart = m_partInfoPage.GetPart();
		if (pPart && pPart->IsModified())
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SAVE_PART);
			if ((bNeedConfirm && MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES) || !bNeedConfirm)
			{
				bModified = TRUE;

				/************************************************************************/
				// save the part.

				// since we modified the part, we should update the modify date.
				pPart->SetModifyTime(DateHelper::GetCurrentTime());

				// save the "basic info" of the part.
				ProductItemWriter::UpdateBasicData(pMainDBCon, pPart->GetID(), pPart->GetName(), pPart->GetComment());
				ProductItemWriter::UpdateModifyDate(pMainDBCon, pPart->GetID(), pPart->GetModifyTime());

				pPart->SetModified(FALSE);
				/************************************************************************/

				/************************************************************************/
				// update UI.

				// update the name of the tree node.
				m_productTree.SetItemText(hItem, pPart->GetName());

				// update the name of this DataItem.
				ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pData->GetProductNodeID());
				DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
				pDataItem->SetName(pPart->GetName());

				// update the page.
				m_partInfoPage.UpdateModifyDate();
				/************************************************************************/
			}
		}
	}
	else if (pData->GetProductNodeType() == PRODUCT_NODE_SHEET)
	{
		// force all edit boxes lose focus.
		m_sheetInfoPage.SetFocus();

		// check whether we should save the sheet.
		SheetPtr pSheet = m_sheetInfoPage.GetSheet();
		if (pSheet && pSheet->IsModified())
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SAVE_SHEET);
			if ((bNeedConfirm && MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES) || !bNeedConfirm)
			{
				bModified = TRUE;

				/************************************************************************/
				// save the sheet.

				// since we modified the part, we should update the modify date.
				pSheet->SetModifyTime(DateHelper::GetCurrentTime());

				// save the "basic info" of the sheet.
				ProductItemWriter::UpdateBasicData(pMainDBCon, pSheet->GetID(), pSheet->GetName(), pSheet->GetComment());
				ProductItemWriter::UpdateModifyDate(pMainDBCon, pSheet->GetID(), pSheet->GetModifyTime());

				pSheet->SetModified(FALSE);
				/************************************************************************/

				/************************************************************************/
				// update UI.

				// update the name of the tree node.
				m_productTree.SetItemText(hItem, pSheet->GetName());

				// update the name of this DataItem.
				ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pData->GetProductNodeID());
				DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
				pDataItem->SetName(pSheet->GetName());

				// update the page.
				m_sheetInfoPage.UpdateModifyDate();
				/************************************************************************/
			}
		}
	}
	else if (pData->GetProductNodeType() == PRODUCT_NODE_NESTJOB)
	{
		// force all edit boxes lose focus.
		m_nestJobInfoPage.SetFocus();

		// check whether we should save the nest job.
		NestJobPtr pNestJob = m_nestJobInfoPage.GetNestJob();
		if (pNestJob && pNestJob->IsModified())
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SAVE_NESTJOB);
			if ((bNeedConfirm && MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES) || !bNeedConfirm)
			{
				bModified = TRUE;

				/************************************************************************/
				// save the nest job.

				// since we modified the part, we should update the modify date.
				pNestJob->SetModifyTime(DateHelper::GetCurrentTime());

				// save the "basic info" of the nest job.
				ProductItemWriter::UpdateBasicData(pMainDBCon, pNestJob->GetID(), pNestJob->GetName(), pNestJob->GetComment());
				ProductItemWriter::UpdateModifyDate(pMainDBCon, pNestJob->GetID(), pNestJob->GetModifyTime());

				pNestJob->SetModified(FALSE);
				/************************************************************************/

				/************************************************************************/
				// update UI.

				// update the name of the tree node.
				m_productTree.SetItemText(hItem, pNestJob->GetName());

				// update the name of this DataItem.
				ProductItemPtr pProductItem = m_pWorkspace->SearchSubNode(pData->GetProductNodeID());
				DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
				pDataItem->SetName(pNestJob->GetName());

				// update the page.
				m_nestJobInfoPage.UpdateModifyDate();
				/************************************************************************/
			}
		}
	}

	return bModified;
}

LRESULT DataMgrDlg::OnTreeNodeMoved(WPARAM wParam, LPARAM lParam)
{
	// get the items which will be moved.
	vector<HTREEITEM> dragItemList = m_productTree.GetDragItems();
	HTREEITEM dropTargetItem = m_productTree.GetDropTargetItem();

	// ExplorerViewHelper will help me to finish this.
	m_explorerViewHelper.MoveItem(m_pWorkspace, dragItemList, dropTargetItem);

	return true;
}

void DataMgrDlg::OpenDataItem(DataItemPtr pDataItem)
{
	// 打开前检查要不要修复零件。
	if (pDataItem->GetDataType() == DATANODE_PART)
	{
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(pDataItem->GetDataID());
		vector<DataProperty> camDataList = PartLoader::GetPartCamDataProp(pPartDBCon);
		if (camDataList.size() == 0)
		{
			PartRepairDlg dlg;
			dlg.SetPart(pDataItem->GetDataID());
			if (dlg.DoModal() == IDOK)
				m_partInfoPage.UpdateCamDataGrid();
			else
				return;
		}
	}

	// 打开前检查要不要修复板材。
	if (pDataItem->GetDataType() == DATANODE_SHEET)
	{
		if (!SheetManager::VerifySheet(pDataItem->GetDataID()))
		{
			SheetRepairDlg dlg;
			dlg.SetSheet(pDataItem->GetDataID());
			dlg.DoModal();

			// check again after display the repair dialog.
			if (!SheetManager::VerifySheet(pDataItem->GetDataID()))
				return;
		}
	}

	if (pDataItem->GetDataType() == DATANODE_PART || pDataItem->GetDataType() == DATANODE_SHEET)
	{
		m_pOpeningDataItem = pDataItem;
		EndDialog(IDC_BTN_EXPLORER_OPEN);
	}
}

END_CUTLEADER_NAMESPACE()
