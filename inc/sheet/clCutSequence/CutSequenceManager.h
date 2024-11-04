#pragma once

#include "clCutSequenceCommon.h"
#include "Rect2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(IMaterialSize)
DECLARE_CUTLEADER_CLASS(SequenceConfigItem)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItem)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItemList)
DECLARE_CUTLEADER_CLASS(PartInstance)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutSequence)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(ConfigData)
DECLARE_CUTLEADER_CLASS(AvoidData)

BEGIN_CUTLEADER_NAMESPACE()

// “切割工序”管理器。
class ClCutSequence_Export CutSequenceManager
{
public:
	// 给零件实例分配工序。
	/* 参数:
	*  partRectMap: 零件实例中各个回路实例的外接矩形。
	*  pPartInstTopItems, partTopMap: the topology of the part inst in the sheet.
	*/
	static void AutoSequence(PartPlacementListPtr pPartPmts_offset, PartInstanceListPtr pPartInsts_offset,
							 CutSequenceListPtr pAllCutSeqs, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
							 const PartInstanceTopologyItemList* pPartTopItems, const std::map<LONGLONG, PartInstanceTopologyItemPtr>& partTopMap,
							 const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);

	// 删除一个工序。
	static void DeleteCutSeq(CutSequenceListPtr pCutSequences, const ICutSequence* pSeqUnit, const IMaterialSize* pMaterialSize, const SequenceConfigItem* pSeqConfigItem);

	// 在指定位置前插入一个“切割工序”。
	/* 参数:
	*  iIndex: 为-1的话，放到最后面。
	*/
	static void InsertCutSeq(CutSequenceListPtr pCutSequences, int iIndex, ICutSequencePtr pCutSequence, const IMaterialSize* pMaterialSize, const SequenceConfigItem* pSeqConfigItem);

	// 把工序移动到指定的位置前面。
	/* 参数:
	*  iNewLoc: 为-1的话，移到最后面。
	*/
	static void MoveCutSeq(CutSequenceListPtr pCutSequences, int iNewLoc, ICutSequencePtr pCutSequence, PartInstanceListPtr pAllPartInst, const IMaterialSize* pMaterialSize, const SequenceConfigItem* pSeqConfigItem);

	// delete the invalid sequences.
	static void DeleteInvalidSeq(CutSequenceListPtr pCutSequences, PartPlacementListPtr pAllPartPmts, PartInstanceListPtr pAllPartInst);
    // 得到无效的切割工序。
    static CutSequenceListPtr GetInvalidSequences(CutSequenceListPtr pCutSequences, PartPlacementListPtr pAllPartPmts, PartInstanceListPtr pAllPartInst);

	// update the cache data in sequence.
	static void UpdateCache(CutSequenceListPtr pCutSequences);

	// 处理工序的偏移。
	// 注：
	//   1) 这个函数生成新的工序，它们和老的是独立的内存，里面图形被偏移过的替换了，其他数据一样。
	//   2) 新的“共边工序”中保存的激光宽度需要调为0.
	static CutSequenceListPtr ProcessOffset(const CutSequenceList* pCutSequences, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances);

private:
	// 根据零件拓扑结构给零件实例分配工序。
	/* 参数:
	*  lastCutPt: 最后一个切出点，用来处理切割避让。
	*  pPrePartInst: 前一个加工的“零件实例”。
    *  pOldCutSeqs: 板子上之前的工序
    *  pNewCutSequences: 新生成的工序
	*/
    // 注：
    //  1) 在处理内嵌零件时，函数1是把一个零件的所有内嵌零件先全部加工掉，然后再去切父零件。而函数2是切掉一个内孔的内嵌零件后马上去加工这个内孔。
	static void SeqPartTopStruct_1(PartPlacementListPtr pPartPmts_offset, PartInstanceListPtr pPartInsts_offset,
        Point2D& lastCutPt, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
        const PartInstanceTopologyItemList* pPartTopItems, const std::map<LONGLONG, PartInstanceTopologyItemPtr>& partTopMap,
        PartInstancePtr& pPrePartInst, LoopInstanceListPtr pSeqLoopInsts, CutSequenceListPtr pNewCutSequences, const CutSequenceList* pOldCutSeqs,
        const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);
    static void SeqPartTopStruct_2(PartPlacementListPtr pPartPmts_offset, PartInstanceListPtr pPartInsts_offset,
        Point2D& lastCutPt, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
        const PartInstanceTopologyItemList* pPartTopItems, const std::map<LONGLONG, PartInstanceTopologyItemPtr>& partTopMap,
        PartInstancePtr& pPrePartInst, LoopInstanceListPtr pSeqLoopInsts, CutSequenceListPtr pNewCutSequences, const CutSequenceList* pOldCutSeqs,
        const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);

	// 得到“零件实例”集合中得到一个“零件走廊”。
	/* 参数:
	*  pPartInstances: 当前拓扑层中未加工的“零件实例”。
	*  pPrePartInst: 前一个加工的“零件实例”。
	*  partRectMap: 零件实例中各个回路实例的外接矩形。
	*  partRect: 这一拓扑层零件的外围矩形。
	*  startCorner: 从哪个起始角开始找走廊。
	*  iDir: 这个走廊的延伸方向。
	*/
	// 注：
	//  1) 函数中的注释以“左下角/X方向”方式为例。
	static PartInstanceListPtr GetStrip(PartInstanceListPtr pPartInstances, PartInstancePtr pPrePartInst, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
									Rect2D partRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

	// 得到走廊中第一个加工的“零件实例”。
	/* 参数：
	*  partRect: 这一拓扑层零件的外围矩形。
	*  pStripPartInsts: 走廊中所有的零件实例。
	*  iStartCorner: 这个走廊的起切角。
	*/
	// 注：
	//  1) 得到第一个加工的“零件实例”后，把它从走廊中删除。
	static PartInstancePtr GetFstStripPart(PartInstanceListPtr pStripPartInsts, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
									       Rect2D partRect, RECT_CORNER iStartCorner);

	// 得到走廊中下一个加工的“零件实例”。
	/* 参数：
	*  pStripPartInsts: 走廊中所有未加工的“零件实例”。
	*  pPrePartInst: 走廊中上一个被加工的“零件实例”。
	*  partRect: 这一拓扑层零件的外围矩形。
	*  iStartCorner: 这个走廊的起切角，这个起始角可能会被改变。
	*  iDir: 这个走廊的延伸方向。
	*/
	// 注：
	//  1) 函数中的注释以“左下角/X方向”方式为例。
	//  2) 以上一个“零件实例”外接矩形的中心点打一根水平线，碰到这条线和在这条线下面的“零件实例”被搜集起来，
	//     这些零件实例中距离板材左下角最近的为下一个“零件实例”。如果这根水平线什么也没碰到，则从“pStripPartInsts”中来找，此时需要注意要不要改变走廊的开始角。
	//  3) 得到下一个加工的“零件实例”后，把它从走廊中删除。
	static PartInstancePtr GetNextStripPart(PartInstanceListPtr pStripPartInsts, PartInstancePtr pPrePartInst, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
											Rect2D partRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

	// 从“basePt”打一条无限长的水平/垂直线（取决于iDir），这根线往startCorner方向扫去，返回所有扫到的零件实例。
	static PartInstanceListPtr GetSweepedParts(PartInstanceListPtr pPartInstances, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
										   Point2D basePt, RECT_CORNER startCorner, XY_DIRECTION iDir);

	// 改变走廊的起始角。
	// 注：
	//   1) 按照走廊方式分配工序时是按照S行运行的，所以下一个走廊的延伸方向和前一个相反的。
	static RECT_CORNER ChgStripStartCorner(RECT_CORNER startCorner, XY_DIRECTION iDir);
};

END_CUTLEADER_NAMESPACE()
