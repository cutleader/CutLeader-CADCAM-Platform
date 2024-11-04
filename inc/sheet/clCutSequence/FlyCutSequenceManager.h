#pragma once

#include "clCutSequenceCommon.h"
#include "Point2D.h"
#include "Rect2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(ConfigData)
DECLARE_CUTLEADER_CLASS(ICutSequence)
DECLARE_CUTLEADER_CLASS(FlyCutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// “飞行切割工序”管理器。
class ClCutSequence_Export FlyCutSequenceManager
{
public:
	// 判断这些回路实例能否创建飞行切割工序。
	static bool CheckLoopInsts_4_flyCutSeq(ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, LoopInstanceListPtr pCandidate_LoopInsts);

	// 计算飞行切割工序的起切位置，此时工序尚未创建。参考了被替换的第一个工序的起切点。
	static RECT_CORNER CalcStartCornerOfFlyCutSeq(const std::map<int, ICutSequencePtr>& cutSeqs_willBeReplaced, ConfigDataPtr pConfigData, LoopInstanceListPtr pCandidate_LoopInsts);

	// 偏移“飞行切割工序”。
	static FlyCutSequencePtr ProcessOffset(FlyCutSequencePtr pFlyCutSeq, PartInstanceListPtr pPartInsts_after_offset);

	// 判断这个飞行工序是不是有效。
	static BOOL IsFlyCutSeqInvalid(FlyCutSequencePtr pFlyCutSeq, PartInstanceListPtr pAllPartInstances);

	// 根据新的“零件实例”集合更新“飞行切割工序”中的一些数据。
	static void UpdateFlyCutSequence(FlyCutSequencePtr pFlyCutSeq, PartInstanceListPtr pOldPartInsts, PartInstanceListPtr pNewPartInsts);
};

END_CUTLEADER_NAMESPACE()
