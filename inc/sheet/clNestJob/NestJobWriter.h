#pragma once

#include "clNestJobCommon.h"

DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(NestJob)

BEGIN_CUTLEADER_NAMESPACE()

// this class will save the nest job into the database.
class ClNestJob_Export NestJobWriter
{
public:
	// save nest job to db.
	// notes:
	//   1) this function save the Nest job into database. 
	//   2) this function:
	//       a) DO NOT update skeleton and sheet info, "NestJobMgr::SaveNestJob" will do this.
	//       c) DO update "nestJob" and "nestJobMat" table.
	static void SaveNestJob(DBConnectPtr pDBConnect, NestJobPtr pNestJob, BOOL bTransaction = TRUE);

	// delete nest job from db.
	// notes:
	//   1) this function should be caller by "NestJobMgr::DeleteNestJob", you know, we should do something else except for this function.
	//   2) the triggers in the tables will help us to finish something.
	//   3) this function do not submit the transaction.
	static void DeleteNestJob(DBConnectPtr pDBConnect, LONGLONG iNestJobID, BOOL bTransaction = TRUE);

	// delete a sheet layout from the nest job.
	static void DelSheetFromJob(DBConnectPtr pDBConnect, LONGLONG iSheetID, BOOL bTransaction = TRUE);
};

END_CUTLEADER_NAMESPACE()
