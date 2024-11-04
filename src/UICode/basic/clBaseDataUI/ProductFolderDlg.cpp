#include "stdafx.h"
#include "ProductFolderDlg.h"

#include "baseConst.h"
#include "DateHelper.h"
#include "DataBaseManager.h"

#include "FolderItem.h"
#include "ProductItemList.h"
#include "ProductItemLoader.h"
#include "ProductItemWriter.h"
#include "FolderInfoDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ProductFolderDlg, CDialog)

BEGIN_MESSAGE_MAP(ProductFolderDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_NEW_FOLDER, OnCreateFolder)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

ProductFolderDlg::ProductFolderDlg(CWnd* pParent /*=NULL*/)
	: CLDialog(ProductFolderDlg::IDD, pParent)
{
	m_pImageList = new CImageList();
	m_pImageList->Create(IDB_PRODUCTEXPL_TREE, 16, 10, RGB(192, 192, 192));

	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	m_pWorkspace = ProductItemLoader::LoadProductTree(pMainDBCon);
}

ProductFolderDlg::~ProductFolderDlg()
{
	delete m_pImageList;
}

void ProductFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_PRODUCT, m_productTree);
}

BOOL ProductFolderDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// init the tree.
	Init();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void ProductFolderDlg::OnDestroy()
{
	// release the data which stored in the tree node.
	HTREEITEM hFirstRootItem = m_productTree.GetRootItem();
	if (hFirstRootItem)
		ReleaseNodeData(hFirstRootItem);

	CDialogEx::OnDestroy();
}

void ProductFolderDlg::OnCreateFolder()
{
	// 得到选中的目录。
	HTREEITEM hParentNode = m_productTree.GetSelectedItem();
	LONGLONG* pData = (LONGLONG*)m_productTree.GetItemData(hParentNode);
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(m_pWorkspace->SearchSubNode(*pData));

	// create a folder item.
	FolderInfoDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		CString strName, strComment;
		dlg.GetMatNameInfo(strName, strComment);

		/************************************************************************/
		// 创建目录对象并保存到数据库。

		// 创建目录对象。
		ProductItemListPtr pProductItemList(new ProductItemList);
		LONGLONG iTime = DateHelper::GetCurrentTime();
		FolderNodePtr pFolderNode(new FolderNode(pParentFolder->GetID(), strName, pProductItemList, iTime, iTime));
		pFolderNode->SetComment(strComment);
		pFolderNode->SetItemPath(pParentFolder->GetFullPath());
		pParentFolder->GetSubItemList()->push_back(pFolderNode);

		// 保存到数据库。
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		ProductItemWriter::CreateProductItem(pMainDBCon, pFolderNode);
		/************************************************************************/

		/************************************************************************/
		// 更新产品树。

		// 挂上树节点。
		TVINSERTSTRUCT hItem;
		hItem.hParent = hParentNode;
		hItem.item.pszText = pFolderNode->GetName().AllocSysString();
		hItem.item.cchTextMax = MAXLENGTH_STR;
		hItem.hInsertAfter = TVI_LAST;
		hItem.item.iImage = 0;
		hItem.item.iSelectedImage = 0;
		hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
		HTREEITEM hSubNode = m_productTree.InsertItem(&hItem);
		m_productTree.SetItemData(hSubNode, (DWORD_PTR)(new LONGLONG(pFolderNode->GetID())));

		// 选中新的目录节点。
		m_productTree.Expand(hParentNode, TVE_EXPAND);
		m_productTree.SelectItem(hSubNode);
		m_pSelFolderNode = pFolderNode;
		/************************************************************************/
	}
}

void ProductFolderDlg::OnBnClickedOk()
{
	// 得到选中的目录。
	HTREEITEM hItem = m_productTree.GetSelectedItem();
	LONGLONG* pData = (LONGLONG*)m_productTree.GetItemData(hItem);
	m_pSelFolderNode = boost::dynamic_pointer_cast<FolderNode>(m_pWorkspace->SearchSubNode(*pData));

	OnOK();
}

void ProductFolderDlg::Init()
{
	m_productTree.SetImageList(m_pImageList, TVSIL_NORMAL);

	// 初始化根节点。
	TVINSERTSTRUCT workspaceItem;
	workspaceItem.hParent = NULL;
	workspaceItem.item.iImage = 0;
	workspaceItem.item.iSelectedImage = 0;
	workspaceItem.item.pszText = m_pWorkspace->GetName().AllocSysString();
	workspaceItem.item.cchTextMax = MAXLENGTH_STR;
	workspaceItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	workspaceItem.hInsertAfter = TVI_LAST;
	HTREEITEM hRootNode = m_productTree.InsertItem(&workspaceItem);
	m_productTree.SetItemData(hRootNode, (DWORD_PTR)(new LONGLONG(m_pWorkspace->GetID())));

	// append all sub-nodes under "m_pWorkspace".
	AppendSubNode(m_pWorkspace->GetSubItemList(), hRootNode);

	// expand the root node.
	m_productTree.Expand(hRootNode, TVE_EXPAND);

	// select the root node.
	m_productTree.SelectItem(hRootNode);
}

void ProductFolderDlg::AppendSubNode(ProductItemListPtr pProductItemList, HTREEITEM hParentNode)
{
	for (unsigned int i = 0; i < pProductItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pProductItemList->at(i);
		if (pProductItem->IsFolder())
		{
			/************************************************************************/
			// append "pProductItem" to "hParentNode".

			// create tree node.
			TVINSERTSTRUCT hItem;
			hItem.hParent = hParentNode;
			hItem.item.pszText = pProductItem->GetName().AllocSysString();
			hItem.item.cchTextMax = MAXLENGTH_STR;
			hItem.hInsertAfter = TVI_LAST;
			//
			hItem.item.iImage = 1;
			hItem.item.iSelectedImage = 1;
			if (HasFolder(pProductItem))
				hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT|TVIF_CHILDREN;
			else
				hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;

			// save folder ID in tree node.
			HTREEITEM hSubNode = m_productTree.InsertItem(&hItem);
			m_productTree.SetItemData(hSubNode, (DWORD_PTR)(new LONGLONG(pProductItem->GetID())));
			/************************************************************************/

			// check whether we will init "hSubNode"
			FolderNodePtr pFolderNode = boost::dynamic_pointer_cast<FolderNode>(pProductItem);
			AppendSubNode(pFolderNode->GetSubItemList(), hSubNode);
		}
	}
}

BOOL ProductFolderDlg::HasFolder(ProductItemPtr pFolderItem)
{
	FolderNodePtr pFolderNode = boost::dynamic_pointer_cast<FolderNode>(pFolderItem);
	ProductItemListPtr pProductItemList = pFolderNode->GetSubItemList();
	for (unsigned int i = 0; i < pProductItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pProductItemList->at(i);
		if (pProductItem->IsFolder())
			return TRUE;
	}

	return FALSE;
}

void ProductFolderDlg::ReleaseNodeData(HTREEITEM hParentItem)
{
	// release the data stored in this node.
	LONGLONG* pData = (LONGLONG*)m_productTree.GetItemData(hParentItem);
	delete pData;

	// go through all brother nodes of "hParentItem".
	HTREEITEM hBrotherNode = m_productTree.GetNextSiblingItem(hParentItem);
	if (hBrotherNode != NULL)
	{
		ReleaseNodeData(hBrotherNode);
	}

	// go through all child nodes of "hParentItem".
	HTREEITEM hChildNode = m_productTree.GetChildItem(hParentItem);
	if (hChildNode != NULL)
	{
		ReleaseNodeData(hChildNode);
	}
}

END_CUTLEADER_NAMESPACE()
