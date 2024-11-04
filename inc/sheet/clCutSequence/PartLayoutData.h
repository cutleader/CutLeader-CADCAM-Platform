#pragma once

#include "clCutSequenceCommon.h"

DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// 零件布局数据。
class ClCutSequence_Export PartLayoutData
{
public:
	PartLayoutData(void);
	PartLayoutData(PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances);
	~PartLayoutData(void);

public: // get/set functions.
	PartPlacementListPtr GetPartPlacements() const { return m_pPartPmtList; }
	void SetPartPlacements(PartPlacementListPtr pPartPlacements) { m_pPartPmtList = pPartPlacements; }

	PartInstanceListPtr GetPartInstances() const { return m_pPartInstList; }
	void SetPartInsts(PartInstanceListPtr pPartInstances) { m_pPartInstList = pPartInstances; }

private:
	// part placements.
	PartPlacementListPtr m_pPartPmtList;

	// part instances.
	PartInstanceListPtr m_pPartInstList;
};

END_CUTLEADER_NAMESPACE()
