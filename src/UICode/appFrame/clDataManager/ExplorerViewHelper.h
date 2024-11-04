#pragma once

#include "clDataManagerCommon.h"
#include "ProductTree.h"
#include <vector>
#include "IBusinessData.h"


DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(DataItem)
DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(DependNode)
DECLARE_CUTLEADER_CLASS(ExportDataInfo)


BEGIN_CUTLEADER_NAMESPACE()


// use this class to help ExplorerView, otherwise ExplorerView will become a huge one.
class ClDataManager_Export ExplorerViewHelper
{
public:
	ExplorerViewHelper(void);
	~ExplorerViewHelper(void);

public:
	//
	void Init(ProductTree* pProductTree);

	/************************************************************************/
	// about creating node.

	void CreateFolder(FolderNodePtr pWorkspace);
	void CreatePart(FolderNodePtr pWorkspace);
	void CreateSheet(FolderNodePtr pWorkspace);

	// 导入导出。
	void Import(FolderNodePtr pWorkspace);
	void SplitImport(FolderNodePtr pWorkspace);
	void Export(FolderNodePtr pWorkspace);
	/************************************************************************/

	/************************************************************************/
	// about deleting tree node.

	// delete the specified node(s) from "hParentItem" node.
	// notes:
	//   1) "hParentItem" is the folder node.
	//   2) these two function will care the UI and release the data in the node.
	void DeleteTreeNode(HTREEITEM hParentItem, LONGLONG iSubNodeID);
	void DeleteTreeNode(HTREEITEM hParentItem, IntegerListPtr pSubNodeIDList);
	/************************************************************************/

	/************************************************************************/
	// about deleting ProductItem node.

	// delete the part node.
	/* params:
	*    1) hItem: tree node of the part.
	*    2) pPartNode: the part node.
	*/
	// notes:
	//   1) call this if no other data depend on the part.
	//   2) this function will delete:
	//      a) tree node.
	//      b) part DataItem in db.
	//      c) part object in db.
	void DeletePartNode(FolderNodePtr pWorkspace, HTREEITEM hItem, DataItemPtr pPartNode);

	// delete the sheet node.
	// notes:
	//   1) call this if no other data depend on the sheet.
	//   2) this function will delete:
	//      a) tree node.
	//      b) sheet DataItem in db.
	//      c) sheet object in db.
	void DeleteSheetNode(FolderNodePtr pWorkspace, HTREEITEM hItem, DataItemPtr pSheetNode);

	// delete the nest job node.
	// notes:
	//   1) call this if no other job depend on this nest job.
	//   2) this function will delete:
	//      a) tree node.
	//      b) nest DataItem in db.
	//      c) nest object in db.
	void DeleteNestJobNode(FolderNodePtr pWorkspace, HTREEITEM hItem, DataItemPtr pNestJobNode);

	// delete all the node in "pDependNode".
	/* params:
	*    1) pDependNode: the dependence tree.
	*/
	// notes:
	//   1) this function will delete:
	//      a) tree node.
	//      b) DataItem in db.
	//      c) object in db.
	void DeleteDependTree(FolderNodePtr pWorkspace, DependNodePtr pDependNode, BOOL bTransaction = TRUE);
	/************************************************************************/

	/************************************************************************/
	// about move/copy tree items.

	// move "itemList" under "targetItem".
	// notes:
	//   1) this function do not update the tree. it will update "pWorkspace", and the database.
	//   2) caller should make sure "targetItem" is not the data item.
	void MoveItem(FolderNodePtr pWorkspace, std::vector<HTREEITEM> itemList, HTREEITEM targetItem);

	// copy "item" to "targetItem".
	/* params:
	*  item: this is a data node.
	*  newItem: the copied tree item.
	*/
	// notes:
	//   1) we will clone a copy into "targetItem", and caller should 
	//      make sure "targetItem" is not the data item.
	//   2) it will update "pWorkspace" and the database, we also should update item data of "newItem".
	void CopyItem(FolderNodePtr pWorkspace, HTREEITEM item, HTREEITEM newItem, HTREEITEM targetItem);
	/************************************************************************/

	// view the dependence of the selected node(s).
	void ViewDepend(FolderNodePtr pWorkspace, HWND hWnd);

	// delete selected node(s).
	// notes:
	//   1) this function will care everything about deleting the selected nodes.
	//   2) if the selected items or the dependence of the selected items have been opened, cannot delete.
	void DeleteSelItem(FolderNodePtr pWorkspace, HWND hWnd);

private:
	// get the parent node of "hItem".
	FolderNodePtr GetParentNode(FolderNodePtr pWorkspace, HTREEITEM hItem);

	// get the ProductItemList from ProductNodeData list.
	ProductItemListPtr GetProdItemList(FolderNodePtr pWorkspace, IntegerListPtr pProductDataList);

	// check whether some item in "pDataItemList" have been opened.
	BOOL CheckDataItemOpened(CString strTaskName, ProductItemListPtr pDataItemList);

    // get the export info from "ExpDataDialog".
    // notes:
    //   1) if return NULL, user cancel the export.
    ExportDataInfoPtr GetExpDataInfo(BUSINESS_DATA_TYPE businessDataType);

private:
	// the tree in ProductExplSheet.
	ProductTree* m_pProductTree;
};

END_CUTLEADER_NAMESPACE()
