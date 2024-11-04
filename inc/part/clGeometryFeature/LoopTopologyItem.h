#pragma once

#include "clGeometryFeatureCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(LoopTopologyItemList)
DECLARE_CUTLEADER_CLASS(IPatternLoop)

BEGIN_CUTLEADER_NAMESPACE()

// “图形回路”拓扑项。
class ClGeometryFeature_Export LoopTopologyItem : public ClData
{
public:
	LoopTopologyItem(IPatternLoopPtr pPatternLoop);
	~LoopTopologyItem(void);

public: // get/set functions.
	void SetPatternLoop(IPatternLoopPtr pPatternLoop) { m_pPatternLoop = pPatternLoop;}
	IPatternLoopPtr GetPatternLoop() const { return m_pPatternLoop; }

	void SetSubItemList(LoopTopologyItemListPtr pSubItemList) { m_pSubItemList = pSubItemList; }
	LoopTopologyItemListPtr GetSubItemList() const { return m_pSubItemList; }

private:
	// 当前图形回路。
	IPatternLoopPtr m_pPatternLoop;

	// 该回路包含的“拓扑项”。
	LoopTopologyItemListPtr m_pSubItemList;
};

END_CUTLEADER_NAMESPACE()
