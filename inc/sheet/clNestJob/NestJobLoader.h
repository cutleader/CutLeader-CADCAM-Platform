#pragma once

#include "clNestJobCommon.h"
#include "DataProperty.h"
#include "DataPropItem.h"
#include "SheetLoader.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(NestJob)
DECLARE_CUTLEADER_CLASS(NestJobMaterialList)

BEGIN_CUTLEADER_NAMESPACE()

// this class will load the nest job from the database.
class ClNestJob_Export NestJobLoader
{
public:
	// about loading nest job.
	// notes:
	//   1) sometimes we do not load NestTask in order to improve the performance.
	//   2) the sheet will be loaded with "SHEETLOAD_BASIC" param.
	//   3) 调用这个函数时，参数bNeedToLoadPartCamData需要结合bLoadNestTask一起用。
	static NestJobPtr LoadNestJob(DBConnectPtr pMainDBCon, LONGLONG iNestJobID, BOOL bLoadNestTask, bool bNeedToLoadPartCamData, SHEETLOAD_TYPE loadType);

	/************************************************************************/
	// about getting some props of the job.

	// get the nest job which included the sheet.
	// notes:
	//   1) data in a DataProperty:
	//      1) data name
	//      2) data comment
	//      3) create date
	//      4) modify date
	static BOOL GetJobInfo(DBConnectPtr pMainDBCon, LONGLONG iSheetID, std::vector<DataProperty>& nestJobInfoList);

	// get the nesting task ID.
	static LONGLONG GetNestTaskID(DBConnectPtr pMainDBCon, LONGLONG iNestJobID);
	/************************************************************************/

	/************************************************************************/
	// about the sheet within the nest job.

	// get the relative matsize ID of the sheet.
	// notes:
	//   1) if the sheet do not relative to a matsize, return INVALID_ID;
	static LONGLONG GetMatSizeOfSheet(DBConnectPtr pMainDBCon, LONGLONG iSheetID);

	// get the count of a sheet which is within a nest job.
	// notes: if "iSheetID" is not within a nest job, return INVALID_VALUE.
	static int GetSheetCount(DBConnectPtr pMainDBCon, LONGLONG iSheetID);

	// get the usage of the mat of the nest job.
	// notes:
	//   1) if all sheets have been deleted from the job, return no item.
	static NestJobMaterialListPtr LoadNestJobMat(DBConnectPtr pMainDBCon, LONGLONG iNestJobID, SHEETLOAD_TYPE loadType = SHEETLOAD_BASIC);
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
