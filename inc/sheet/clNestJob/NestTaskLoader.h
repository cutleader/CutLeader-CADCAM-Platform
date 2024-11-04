#pragma once

#include "clNestJobCommon.h"

DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(NestTask)
DECLARE_CUTLEADER_CLASS(NestPartList)
DECLARE_CUTLEADER_CLASS(NestMaterialList)

BEGIN_CUTLEADER_NAMESPACE()

// this class will load the nest task from the database.
class ClNestJob_Export NestTaskLoader
{
public:
	// 从数据库加载排版任务。
	static NestTaskPtr LoadNestTask(DBConnectPtr pMainDBCon, LONGLONG iNestTask, bool bNeedToLoadPartCamData);

private:
	// load nest parts of the nest task.
	static NestPartListPtr LoadNestPart(DBConnectPtr pMainDBCon, LONGLONG iNestTaskID, bool bNeedToLoadPartCamData);

	// load nest mats of the nest task.
	// notes:
	//   1) we only init "m_dLength", "m_dWidth", "m_bSkeleton".
	static NestMaterialListPtr LoadNestMat(DBConnectPtr pMainDBCon, LONGLONG iNestTaskID);
};

END_CUTLEADER_NAMESPACE()
