#pragma once

#include "clNestJobCommon.h"
#include "BusinessData.h"

DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(Sheet)
DECLARE_CUTLEADER_CLASS(NestJobMaterialList)
DECLARE_CUTLEADER_CLASS(NestTask)

BEGIN_CUTLEADER_NAMESPACE()

// 排版作业
class ClNestJob_Export NestJob : public BusinessData
{
public:
	NestJob(void);
	NestJob(CString strJobName);
	virtual ~NestJob(void);

public: // implement IBusinessData interface
	virtual BUSINESS_DATA_TYPE GetBusinessDataType() const override { return BUSINESS_DATA_NESTJOB; }

public: // get/set functions.
	void SetNestJobMatList(NestJobMaterialListPtr pNestJobMats) { m_pNestJobMats = pNestJobMats; }
	NestJobMaterialListPtr GetNestJobMatList() const { return m_pNestJobMats; }

	void SetNestTask(NestTaskPtr pNestTask) { m_pNestTask = pNestTask; }
	NestTaskPtr GetNestTask() const { return m_pNestTask; }

public:
	/************************************************************************/
	// about sheet within the nest job.
	
	// get the count of the sheets which are produced by the nest job.
	int GetTotalSheetCount() const;

	IntegerListPtr GetAllSheet() const;

	// 得到指定的板材。
	SheetPtr GetSheetByID(LONGLONG iSheetID) const;
	/************************************************************************/

private:
	// the materials info consumed by the nest job.
	// notes: 
	//   1) allocate memory outside the class.
	//   2) from this we can know the sheets created by the Nest Task.
	NestJobMaterialListPtr m_pNestJobMats;

	// the nest task which produced this nest job.
	NestTaskPtr m_pNestTask;
};

END_CUTLEADER_NAMESPACE()
