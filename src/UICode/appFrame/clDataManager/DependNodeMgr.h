#pragma once

#include "clDataManagerCommon.h"
#include "DataItem.h"
#include "PartWriter.h"
#include "SheetManager.h"
#include "NestJobMgr.h"
#include "NestJobLoader.h"
#include "NestJobWriter.h"

DECLARE_CUTLEADER_CLASS(ProductItemList)
DECLARE_CUTLEADER_CLASS(DataItem)
DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(DependNode)
DECLARE_CUTLEADER_CLASS(DependNodeList)

BEGIN_CUTLEADER_NAMESPACE()

// this class will manage the dependence relationship.
class ClDataManager_Export DependNodeMgr
{
public:
	/************************************************************************/
	// about get dependence tree.

	// get the dependence tree of "pDataItem".
	// notes:
	//   1) if we reach a sheet, we do not search the nest job, because we do not
	//      think nest job depend on the sheet.
	//   2) if we reach a nest job, we should search the sheets which depend on
	//      the nest job, because we think the sheet depend on the nest job.
	/* params:
	*    1) pDataItemList: all data items under the workspace, we will use it to search something.
	*/
	static DependNodePtr GetDependTree(ProductItemListPtr pDataItemList, DataItemPtr pDataItem);

	// get the dependence tree of all data items under "pFolderNode".
	static DependNodeListPtr GetDependTree(ProductItemListPtr pAllDataItemList, FolderNodePtr pFolderNode);

	// get the dependence tree of all data items in "pDataItemList".
	static DependNodeListPtr GetDependTree(ProductItemListPtr pAllDataItemList, ProductItemListPtr pDataItemList);
	/************************************************************************/

	/************************************************************************/
	// about delete the data on dependence tree.

	// delete data in "pDependNode".
	static void DelDataOnDpdTree(DependNodePtr pDependNode, BOOL bTransaction = TRUE);

	// delete data in "pDependNodeList".
	static void DelAllDataOnDpdTree(DependNodeListPtr pDependNodeList, BOOL bTransaction = TRUE);
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
