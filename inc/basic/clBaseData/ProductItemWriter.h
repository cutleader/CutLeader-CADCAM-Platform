#pragma once

#include "clBaseDataCommon.h"

DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(FolderNode)

BEGIN_CUTLEADER_NAMESPACE()

class IProductItem;
typedef boost::shared_ptr<IProductItem> ProductItemPtr;

// this class will modify "productStruct_folder" and "productStruct_data" table.
// notes:
//   1) this class do not affect other tables.
class ClBaseData_Export ProductItemWriter
{
public:
	/************************************************************************/
	// about creating items.

	// append "pProductItem" to "pFolderNode".
	// notes:
	//   1) we can know the parent from "pProductItem::parentID".
	//   2) this function think "pProductItem" is a new item and it has no child.
	static void CreateProductItem(DBConnectPtr pDBConnect, ProductItemPtr pProductItem, BOOL bTransaction = TRUE);
	/************************************************************************/

	/************************************************************************/
	// about deleting items.
	
	// delete the folder item "pFolderNode".
	// notes:
	//   1) we need a recursive to delete all nodes under "pFolderNode".
	static void DeleteFolderItem(DBConnectPtr pDBConnect, FolderNodePtr pFolderNode, BOOL bTransaction = TRUE);

	// delete data item by item ID.
	static void DeleteDataItem(DBConnectPtr pDBConnect, IntegerListPtr pDataItemIDList, BOOL bTransaction = TRUE);

	// delete data item by data ID.
	static void DeleteDataItemEx(DBConnectPtr pDBConnect, IntegerListPtr pDataIDList, BOOL bTransaction = TRUE);
	/************************************************************************/

	/************************************************************************/
	// about modifying some data.

	// update the basic data of the data.
	// notes:
	//   1) after call this, must call function "UpdateModifyDate".
	static void UpdateBasicData(DBConnectPtr pDBConnect, LONGLONG iDataID, CString strName, CString strComment, BOOL bTransaction = TRUE);

	// update the modified date of the data.
	static void UpdateModifyDate(DBConnectPtr pDBConnect, LONGLONG iDataID, LONGLONG iModifyTimeStamp, BOOL bTransaction = TRUE);

	// save the basic data of folder into database.
	static void UpdateFolderBasicData(DBConnectPtr pDBConnect, FolderNodePtr pFolderNode);

	// change the parent node of "pProductItem".
	static void ChangeParentNode(DBConnectPtr pDBConnect, ProductItemPtr pProductItem, BOOL bTransaction = TRUE);
	/************************************************************************/

private:
	// this is a recursive function, but we do not use transaction in this function.
	static void RemoveFolderItem(DBConnectPtr pDBConnect, FolderNodePtr pFolderNode);
};

END_CUTLEADER_NAMESPACE()
