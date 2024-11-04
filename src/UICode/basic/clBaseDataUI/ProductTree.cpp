#include "StdAfx.h"
#include "ProductTree.h"

#include "baseConst.h"
#include "taoresource.h"
#include "IntegerList.h"

#include "ProductItemList.h"
#include "ProductItemBase.h"
#include "FolderItem.h"
#include "DataItem.h"
#include "ProdNodeData.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ProductTree, XTreeCtrl)

BEGIN_MESSAGE_MAP(ProductTree, XTreeCtrl)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

ProductTree::ProductTree(void)
		: XTreeCtrl(IDC_CURSOR_TREE_COPYDROP, IDC_CURSOR_TREE_MOVEDROP, IDC_CURSOR_TREE_NODROP)
{
	m_pImageList = new CImageList();
}

ProductTree::~ProductTree(void)
{
	delete m_pImageList;
}

BOOL ProductTree::CanBeFolderNode(HTREEITEM hItem)
{
	UINT dataVal = GetItemData(hItem);
	ProductNodeData* pData = (ProductNodeData*)dataVal;
	if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
		return TRUE;

	return FALSE;
}

void ProductTree::OnDestroy()
{
	// release the data which stored in the tree node.
	HTREEITEM hFirstRootItem = GetRootItem();
	if (hFirstRootItem)
		ReleaseAllNodeData(hFirstRootItem);

	XTreeCtrl::OnDestroy();
}

void ProductTree::AppendSubNode(ProductItemListPtr pProductItemList, HTREEITEM hParentNode, vector<HTREEITEM>& newTreeItemList)
{
	for (unsigned int i = 0; i < pProductItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pProductItemList->at(i);

		/************************************************************************/
		// append "pProductItem" to "hParentNode".

		// create tree node.
		TVINSERTSTRUCT hItem;
		hItem.hParent = hParentNode;
		hItem.item.pszText = pProductItem->GetName().AllocSysString();
		hItem.item.cchTextMax = MAXLENGTH_STR;
		hItem.hInsertAfter = TVI_LAST;

		// the data which will be saved to the tree node.
		ProductNodeData* pProductNodeData = new ProductNodeData();
		pProductNodeData->SetProductNodeID(pProductItem->GetID());

		if (pProductItem->IsFolder())
		{
			hItem.item.iImage = 1;
			hItem.item.iSelectedImage = 1;
			hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;

			// save data in tree node.
			pProductNodeData->SetProductNodeType(PRODUCT_NODE_FOLDER);
		}
		else
		{
			DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
			if (pDataItem->GetDataType() == DATANODE_PART)
			{
				hItem.item.iImage = 4;
				hItem.item.iSelectedImage = 4;

				pProductNodeData->SetProductNodeType(PRODUCT_NODE_PART);
			}
			else if (pDataItem->GetDataType() == DATANODE_SHEET)
			{
				hItem.item.iImage = 5;
				hItem.item.iSelectedImage = 5;

				pProductNodeData->SetProductNodeType(PRODUCT_NODE_SHEET);
			}
			else if (pDataItem->GetDataType() == DATANODE_NESTJOB)
			{
				hItem.item.iImage = 6;
				hItem.item.iSelectedImage = 6;

				pProductNodeData->SetProductNodeType(PRODUCT_NODE_NESTJOB);
			}

			hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
		}

		HTREEITEM hSubNode = InsertItem(&hItem);
		newTreeItemList.push_back(hSubNode);
		SetItemData(hSubNode, (int)pProductNodeData);
		/************************************************************************/

		// check whether we will init "hSubNode"
		if (pProductItem->IsFolder())
		{
			FolderNodePtr pFolderNode = boost::dynamic_pointer_cast<FolderNode>(pProductItem);
			AppendSubNode(pFolderNode->GetSubItemList(), hSubNode, newTreeItemList);
		}
	}
}

BOOL ProductTree::DeleteItem(HTREEITEM hItem)
{
	ReleaseNodeData(hItem);
	return XTreeCtrl::DeleteItem(hItem);
}

BOOL ProductTree::LocateNode(HTREEITEM hItem, LONGLONG iNaviNodeID)
{
	// get the item data.
	UINT dataVal = GetItemData(hItem);
	ProductNodeData* pData = (ProductNodeData*)dataVal;

	// check whether this item is the item we want.
	if (pData->GetProductNodeID() == iNaviNodeID)
	{
		// select the node.
		SelectAll(FALSE);
		SetItemState(hItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		SetFocus();

		return TRUE;
	}

	if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
	{
		// expand this "folder" node.
		Expand(hItem, TVE_EXPAND);

		// go through each "child" node.
		BOOL bFound = FALSE;

		HTREEITEM hChildItem = GetChildItem(hItem);
		while (hChildItem) 
		{
			if (LocateNode(hChildItem, iNaviNodeID))
			{
				bFound = TRUE;
				return TRUE;
			}
			else
				hChildItem = GetNextSiblingItem(hChildItem);
		}

		// IMPORTANT! if do not found it in this "folder" node, collapse it.
		if (!bFound)
			Expand(hItem, TVE_COLLAPSE);
	}

	return FALSE;
}

void ProductTree::NavigateToNode(LONGLONG iNaviNodeID)
{
	HTREEITEM hRootItem = GetRootItem();

	if (iNaviNodeID == INVALID_ID)
	{
		// select the root node.
		SelectAll(FALSE);
		SetItemState(hRootItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		SetFocus();

		return;
	}

	// go through the children.
	LocateNode(hRootItem, iNaviNodeID);
}

void ProductTree::Init()
{
	// enable multiple selection.
	SetMultiSelect(TRUE);

	// init the image list.
	m_pImageList->Create(IDB_PRODUCTEXPL_TREE, 16, 10, RGB(192, 192, 192));
	SetImageList(m_pImageList, TVSIL_NORMAL);
}

void ProductTree::InitTree(FolderNodePtr pWorkspace)
{
	m_pWorkspace = pWorkspace;

	/************************************************************************/
	// append the root node.

	// create tree node.
	TVINSERTSTRUCT workspaceItem;
	workspaceItem.hParent = NULL;
	workspaceItem.item.iImage = 0;
	workspaceItem.item.iSelectedImage = 0;
	workspaceItem.item.pszText = pWorkspace->GetName().AllocSysString();
	workspaceItem.item.cchTextMax = MAXLENGTH_STR;
	workspaceItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;
	workspaceItem.hInsertAfter = TVI_LAST;

	HTREEITEM hRootNode = InsertItem(&workspaceItem);

	// save data in tree node.
	ProductNodeData* pProductNodeData = new ProductNodeData();
	pProductNodeData->SetProductNodeType(PRODUCT_NODE_FOLDER);
	pProductNodeData->SetProductNodeID(pWorkspace->GetID());
	SetItemData(hRootNode, (int)pProductNodeData);
	/************************************************************************/

	// append all sub-nodes under "pWorkspace".
	vector<HTREEITEM> newTreeItemList;
	AppendSubNode(pWorkspace->GetSubItemList(), hRootNode, newTreeItemList);

	// expand the root node.
	Expand(hRootNode, TVE_EXPAND);
}

IntegerListPtr ProductTree::GetSelTreeItemData()
{
	IntegerListPtr pTreeItemList(new IntegerList);

	// first, get all selected tree node.
	vector<HTREEITEM> tmpTreeItemList;
	tmpTreeItemList = GetSelectedList();

	/************************************************************************/
	// check each tree node.

	for (unsigned int i = 0; i < tmpTreeItemList.size(); i++)
	{
		BOOL bNeedThis = TRUE;
		HTREEITEM hTmpItem = tmpTreeItemList.at(i);  

		// check the parent/grand-parent/.... nodes.
		HTREEITEM hParentItem = GetParentItem(hTmpItem);
		while (hParentItem)
		{
			if (IncludeItem(tmpTreeItemList, hParentItem))
			{
				bNeedThis = FALSE;
				break;
			}

			// get next "parent" node.
			hParentItem = GetParentItem(hParentItem);
		}

		if (bNeedThis)
			pTreeItemList->AddInt(GetItemData(hTmpItem));
	}
	/************************************************************************/

	return pTreeItemList;
}

void ProductTree::ReleaseAllNodeData(HTREEITEM hParentItem)
{
	// release the data stored in this node.
	int iData = GetItemData(hParentItem);
	ProductNodeData* pProductNodeData = (ProductNodeData*)iData;
	delete pProductNodeData;

	// go through all brother nodes of "hParentItem".
	HTREEITEM hBrotherNode = GetNextSiblingItem(hParentItem);
	if (hBrotherNode != NULL)
		ReleaseAllNodeData(hBrotherNode);

	// go through all child nodes of "hParentItem".
	HTREEITEM hChildNode = GetChildItem(hParentItem);
	if (hChildNode != NULL)
		ReleaseAllNodeData(hChildNode);
}

void ProductTree::ReleaseNodeData(HTREEITEM hParentItem)
{
	// release the data stored in this node.
	int iData = GetItemData(hParentItem);
	ProductNodeData* pProductNodeData = (ProductNodeData*)iData;
	delete pProductNodeData;

	HTREEITEM hChild = GetChildItem(hParentItem);
	while (hChild != NULL)
	{
		ReleaseNodeData(hChild);
		hChild = GetNextSiblingItem(hChild);
	}
}

END_CUTLEADER_NAMESPACE()
