#pragma once

#include "clBaseDataCommon.h"
#include "DataItem.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

class IProductItem;
typedef boost::shared_ptr<IProductItem> ProductItemPtr;

// the list of ProductItem.
class ClBaseData_Export ProductItemList : public std::vector<ProductItemPtr>
{
public:
	ProductItemList(void);
	~ProductItemList(void);

public:
	/************************************************************************/
	// about getting data item.

	// get all DataItem.
	// notes:
	//   1) this function should be a recursive one.
	//   2) caller must allocate "pDataItemList".
	void GetAllDataItem(ProductItemListPtr pDataItemList);

	// get the data items by the type.
	// notes:
	//   1) this is not a recursive function.
	ProductItemListPtr GetItemByType(DATANODE_TYPE dataNodeType);

	// get data item by data ID.
	ProductItemPtr GetItemByDataID(LONGLONG iDataID);
	/************************************************************************/

	/************************************************************************/
	// about getting folder item.

	// get all FolderNode.
	// notes:
	//   1) this function should be a recursive one.
	//   2) caller must allocate "pFolderNodeList".
	void GetAllFolderNode(ProductItemListPtr pFolderNodeList);

	// get FolderNode.
	// notes:
	//   1) this function is not a recursive one.
	ProductItemListPtr GetFolderNodeList();
	/************************************************************************/

	// get item by item ID.
	ProductItemPtr GetItemByID(LONGLONG iItemID);

	void RemoveItem(LONGLONG iItemID);
};

END_CUTLEADER_NAMESPACE()
