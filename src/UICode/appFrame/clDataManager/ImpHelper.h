#pragma once

#include "clDataManagerCommon.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)

BEGIN_CUTLEADER_NAMESPACE()

// 导入数据的帮助类。
class ClDataManager_Export ImpHelper
{
public:
	// 导入数据。
	/* 参数：
	*  pFolderNode: 导入到何处。
	*  bCanChangeFolder: 是否可以改变目录。
	*/
	// 注：
	//   1) 这个函数负责选择导入的文件，以及数据入库。
	//   2) 如果成功导入需要返回“产品项”集合。
	static ProductItemListPtr ImportData(FolderNodePtr pFolderNode, BOOL bCanChangeFolder);

	// 拆分导入零件。
	/* 参数：
	*  pFolderNode: 导入到何处。
	*  bCanChangeFolder: 是否可以改变目录。
	*/
	// 注：
	//   1) 这个函数负责选择导入的文件，以及数据入库。
	//   2) 如果文件中包含多个零件，需要拆分。
	//   3) 如果成功导入需要返回“产品项”集合。
	static ProductItemListPtr SplitImportPart(FolderNodePtr pFolderNode, BOOL bCanChangeFolder);
};

END_CUTLEADER_NAMESPACE()
