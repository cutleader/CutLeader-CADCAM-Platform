#pragma once

#include "clCutSequenceCommon.h"
#include "Point2D.h"
#include "Rect2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(PartInstance)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItem)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItemList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(ICutSequence)
DECLARE_CUTLEADER_CLASS(LoopSequence)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(AvoidData)
DECLARE_CUTLEADER_CLASS(ConfigData)

BEGIN_CUTLEADER_NAMESPACE()

// “回路工序”管理器。
class ClCutSequence_Export LoopSequenceManager
{
public:
	// 给“零件实例”中的“回路实例”分配工序。
	/* 参数:
	*  lastCutPt: 最后一个切出点，用来处理切割避让。
	*  pPreLoopInst: 前一个加工的回路实例。
	*  partInstRect: 这个零件实例的外围矩形，在寻找走廊时用到。
	*  pPartInstTopItems: 当前拓扑层。
	*  pPartInstTopItem: 当前拓扑项。
	*  pLoopInstances: 要分配工序的“回路实例”，他们来自同一个“零件实例”。
	*  outerLoopGeom: 板材上所有“零件实例”外围回路的基本图形。
	*/
	static CutSequenceListPtr GenLoopSeq(Point2D& lastCutPt, LoopInstancePtr pPreLoopInst, const Rect2D& partInstRect, const PartInstanceTopologyItemList* pPartInstTopItems, const PartInstanceTopologyItem* pPartInstTopItem,
									LoopInstanceListPtr pLoopInstances, LoopInstanceListPtr pSeqLoopInsts, const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);

	// 给零件实例的内部“回路实例”分配工序。
	// 注：
	//  1) 以当前零件为一个范围来分配工序，即不考虑这个零件外的已有的工序，调用者需要重新处理避让关系。
	static CutSequenceListPtr GenLoopSeqs_4_innerLoops(CutSequenceListPtr pCutSequences, PartInstancePtr pPartInstance, PartInstanceListPtr pPartInstances, const ConfigData* pConfigData);

	// check whether loop sequence is invalid.
	// notes:
	//   1) if cannot find the loop inst in "pAllPartInst", the sequence is invalid.
	static BOOL IsLoopSeqInvalid(LoopSequencePtr pLoopSequence, PartInstanceListPtr pAllPartInst);

	// 偏移“回路工序”。
	// 注：
	//   1) “零件放置”做了偏移之后，“回路工序”也需要做偏移。
	static LoopSequencePtr ProcessOffset(LoopSequencePtr pLoopSequence, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances);

	// 根据新的“零件实例”集合更新工序中的一些数据。
	/* 参数：
	*  pOldPartInsts: 旧的零件实例集合。
	*  pNewPartInsts: 克隆的新的零件实例集合。
	*/
	// 注：
	//  1) 需要替换掉“回路实例”中的回路ID和“零件实例”。
	static void UpdateLoopSeq(LoopSequencePtr pLoopSequence, PartInstanceListPtr pOldPartInsts, PartInstanceListPtr pNewPartInsts);

    // 给回路实例分配工序。
    // 注：
    //  1) 这个函数需要更新最后的切出点和加工过的回路集合。
    static LoopSequencePtr BuildLoopSeq(Point2D& lastHitPt, const PartInstanceTopologyItemList* pPartInstTopItems, const PartInstanceTopologyItem* pPartInstTopItem, LoopInstancePtr pLoopInstance, 
        LoopInstanceListPtr pSeqLoopInsts, const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, AvoidDataPtr pAvoidData, const ConfigData* pConfigData);

    	// 从“回路实例”集合中得到一个“回路走廊”。
	/* 参数:
	*  pLoopInstances: 未加工的“回路实例”。
	*  pPreLoopInst: 前一个加工的“回路实例”。
	*  startCorner: 从哪个起始角开始找走廊。
	*  iDir: 这个走廊的延伸方向。
	*/
	// 注：
	//  1) 函数中的注释以“左下角/X方向”方式为例。
	static LoopInstanceListPtr GetStrip(LoopInstanceListPtr pLoopInstances, LoopInstancePtr pPreLoopInst, Rect2D partInstRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

	// 得到走廊中第一个加工的“回路实例”。
	/* 参数：
	*  pStripLoopInsts: 走廊中所有的回路实例。
	*  iStartCorner: 这个走廊的起切角。
	*/
	// 注：
	//  1) 得到第一个加工的“回路实例”后，把它从走廊中删除。
	static LoopInstancePtr GetFstStripLoop(LoopInstanceListPtr pStripLoopInsts, Rect2D partInstRect, RECT_CORNER iStartCorner);

	// 得到走廊中下一个加工的“回路实例”。
	/* 参数：
	*  pStripPartInsts: 走廊中所有未加工的“回路实例”。
	*  pPreLoopInst: 走廊中上一个被加工的“回路实例”。
	*  iStartCorner: 这个走廊的起切角，这个起始角可能会被改变。
	*  iDir: 这个走廊的延伸方向。
	*/
	// 注：
	//  1) 函数中的注释以“左下角/X方向”方式为例。
	//  2) 以上一个“回路实例”外接矩形的中心点打一根水平线，碰到这条线和在这条线下面的“回路实例”被搜集起来，
	//     这些回路实例中距离板材左下角最近的为下一个“回路实例”。如果这根水平线什么也没碰到，则从“pStripLoopInsts”中来找，此时需要注意要不要改变走廊的开始角。
	//  3) 得到下一个加工的“回路实例”后，把它从走廊中删除。
	static LoopInstancePtr GetNextStripLoop(LoopInstanceListPtr pStripLoopInsts, LoopInstancePtr pPreLoopInst, Rect2D partInstRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

private:
	// when use cut avoidance, check whether need to disable lead out of the loop sequence.
	static void EnableLeadOut(LoopSequencePtr pLoopSequence);

	// 从“basePt”打一条无限长的水平/垂直线（取决于iDir），这根线往startCorner方向扫去，返回所有扫到的回路实例。
	static LoopInstanceListPtr GetSweepedLoops(LoopInstanceListPtr pLoopInstList, Point2D basePt, RECT_CORNER startCorner, XY_DIRECTION iDir);

	// 改变走廊的起始角。
	// 注：
	//   1) 按照走廊方式分配工序时是按照S行运行的，所以下一个走廊的延伸方向和前一个相反的。
	static RECT_CORNER ChgStripStartCorner(RECT_CORNER startCorner, XY_DIRECTION iDir);
};

END_CUTLEADER_NAMESPACE()
