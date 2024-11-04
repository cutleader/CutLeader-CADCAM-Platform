#pragma once

#include "clPartCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(IPatternLoop)
DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(PartTopologyItemList)

BEGIN_CUTLEADER_NAMESPACE()

// 零件拓扑项。
class ClPart_Export PartTopologyItem : public ClData
{
public:
	PartTopologyItem(void);
	PartTopologyItem(IPatternLoopPtr pOuterLoop);
	~PartTopologyItem(void);

public: // get/set functions.
	void SetOuterLoop(IPatternLoopPtr pOuterLoop) { m_pOuterLoop = pOuterLoop; }
	IPatternLoopPtr GetOuterLoop() const { return m_pOuterLoop; }

	void SetInnerLoopList(PatternLoopListPtr pInnerLoopList) { m_pInnerLoopList = pInnerLoopList; }
	PatternLoopListPtr GetInnerLoopList() const { return m_pInnerLoopList; }

	void SetSubItemList(PartTopologyItemListPtr pSubItemList) { m_pSubItemList = pSubItemList; }
	PartTopologyItemListPtr GetSubItemList() const { return m_pSubItemList; }

private:
	// 零件的外围回路。
	// 注：
	//   1) 必须为闭合的。
	IPatternLoopPtr m_pOuterLoop;

	// 零件的内部回路。
	// 注：
	//   1) 可以为开放的。
	PatternLoopListPtr m_pInnerLoopList;

	// 该零件包含的“拓扑项”。
	PartTopologyItemListPtr m_pSubItemList;
};

END_CUTLEADER_NAMESPACE()
