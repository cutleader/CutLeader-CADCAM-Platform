#pragma once

#include "clNestJobCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(Sheet)

BEGIN_CUTLEADER_NAMESPACE()

// a nesting job will consume some material, this class records these informations.
// notes:
//   1) a nesting job will contain a list of NestJobMaterial objects, so we know the materials info consumed by the job.
//   2) a NestJobMaterial object will fit a MatLayout object.
class ClNestJob_Export NestJobMaterial : public ClData
{
public:
	NestJobMaterial(void);
	NestJobMaterial(SheetPtr pSheet, int iCount);
	~NestJobMaterial(void);

public: // get/set functions.
	void SetSheet(SheetPtr pSheet) { m_pSheet = pSheet; }
	SheetPtr GetSheet() const { return m_pSheet; }

	void SetCount(int iCount) { m_iCount = iCount; }
	int GetCount() const { return m_iCount; }

private:
	// in "m_pSheet", we keep the matSize which the sheet used.
	SheetPtr m_pSheet;

	// the count of the sheet. 
	int m_iCount;
};

END_CUTLEADER_NAMESPACE()
