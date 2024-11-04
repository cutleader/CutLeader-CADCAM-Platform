#pragma once

#include "clDataManagerCommon.h"
#include "TaskEx.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ProductItemList)
DECLARE_CUTLEADER_CLASS(ImportDataInfoList)
DECLARE_CUTLEADER_CLASS(ImpDataTask)

BEGIN_CUTLEADER_NAMESPACE()

// 这个任务对象负责导入数据到数据库。
class ClDataManager_Export ImpDataTask : public TaskEx
{
public:
	ImpDataTask(FolderNodePtr pParentFolder, ImportDataInfoListPtr pImpDataInfoList);
	~ImpDataTask(void);

public: // inherit from class TaskEx.
	virtual void Run();

public:
	ProductItemListPtr GetNewProductItemList() const { return m_pNewProductItemList; }

private:
	// whether import the data.
	FolderNodePtr m_pParentFolder;

	// the data which will be imported.
	ImportDataInfoListPtr m_pImpDataInfoList;

	// the new product item list.
	ProductItemListPtr m_pNewProductItemList;
};

END_CUTLEADER_NAMESPACE()
