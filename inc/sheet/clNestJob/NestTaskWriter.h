#pragma once

#include "clNestJobCommon.h"

DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(NestTask)
DECLARE_CUTLEADER_CLASS(NestPartList)
DECLARE_CUTLEADER_CLASS(NestMaterialList)

BEGIN_CUTLEADER_NAMESPACE()

// this class will save the nest task into the database.
class NestTaskWriter
{
public:
	static void SaveNestTask(DBConnectPtr pDBConnect, LONGLONG iNestJobID, NestTaskPtr pNestTask, BOOL bTransaction = TRUE);

private:
	static void SaveNestPart(DBConnectPtr pDBConnect, LONGLONG iNestTaskID, NestPartListPtr pNestPartList);

	// do not save all data of nest part into database.
	static void SaveNestMat(DBConnectPtr pDBConnect, LONGLONG iNestTaskID, NestMaterialListPtr pNestMatList);
};

END_CUTLEADER_NAMESPACE()
