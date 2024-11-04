#pragma once

#include "ProductItemBase.h"
#include "boost/enable_shared_from_this.hpp"

DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(ProductItemList)
DECLARE_CUTLEADER_CLASS(FolderNode)

BEGIN_CUTLEADER_NAMESPACE()

class IProductItem;
typedef boost::shared_ptr<IProductItem> ProductItemPtr;

// “目录项”对象。
// 注：
//  1) 根目录名称在数据库是英文，所以加载时要根据语言处理一下。
class ClBaseData_Export FolderNode : public ProductItemBase,
										public boost::enable_shared_from_this<FolderNode>
{
public:
	FolderNode(void);
	FolderNode(LONGLONG iParentID, CString strName, ProductItemListPtr pSubItemList, LONGLONG iCreateTime, LONGLONG iModifyTime);
	virtual ~FolderNode(void);

public: // implement interface IProductItem.
	virtual BOOL IsFolder() const override { return TRUE; }

public: // get/set functions.
	ProductItemListPtr GetSubItemList() const { return m_pProductItemList; }
	void SetSubItemList(ProductItemListPtr pProductItemList) { m_pProductItemList = pProductItemList; }

public:
	/************************************************************************/
	// about get data items.
	
	// get all DataItem under this folder.
	// notes:
	//   1) this function should be a recursive one.
	//   2) caller must allocate "pDataItemList".
	void GetAllDataItem(ProductItemListPtr pDataItemList);

	// get DataItems under this folder.
	// notes:
	//   1) this function is not a recursive one.
	ProductItemListPtr GetDataItemList();

	// use the data ID list to get data item IDs.
	IntegerListPtr GetDataItem(IntegerListPtr pDataIDList);
	/************************************************************************/

	// search a sub-node "iNodeID".
	// notes:
	//   1) this function should be a recursive one.
	ProductItemPtr SearchSubNode(LONGLONG iNodeID);

	// get all FolderNode under this folder.
	// notes:
	//   1) this function should be a recursive one.
	//   2) caller must allocate "pFolderNodeList".
	void GetAllFolderNode(ProductItemListPtr pFolderNodeList);

	/************************************************************************/
	// delete sub-nodes under the node.
	// notes:
	//   1) you know, if "iSubNodeID" is a folder node, sub-nodes under it will also be deleted.

	void DeleteSubNode(LONGLONG iSubNodeID);
	void DeleteSubNode(IntegerListPtr pSubNodeIDList);
	/************************************************************************/

private:
	// the sub-nodes under this folder.
	// notes:
	//   1) this class do not manage the memory.
	ProductItemListPtr m_pProductItemList;
};

END_CUTLEADER_NAMESPACE()
