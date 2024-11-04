#pragma once

#include "clExpertLibCommon.h"
#include "clCutFeatureCommon.h"
#include "baseConst.h"
#include "Vector2D.h"
#include <tuple>
#include "PatternPosition.h"


DECLARE_CUTLEADER_CLASS(IPatternLoop)
DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(CondConfigItem)
DECLARE_CUTLEADER_CLASS(PierceConfigItem)
DECLARE_CUTLEADER_CLASS(LoopStartCutData)
DECLARE_CUTLEADER_CLASS(ConditionNodeList)
DECLARE_CUTLEADER_CLASS(LoopStartCutDataList)
DECLARE_CUTLEADER_CLASS(ILead)


BEGIN_CUTLEADER_NAMESPACE()


// 起切点调整的方式。
enum MOV_START_CUT_TYPE
{
	GEOM_CORNER_2_CORNER	= 0, // 从图形或角调整到角上。
	CORNER_2_GEOM			= 1, // 从角调整到图形上。
	GEOM_2_GEOM				= 2, // 从图形调整到图形上。
	CANNOT_MOVE				= 3, // 不能调整（新的起切点位置和老的一样或者在180度的角处）。
};


// “回路起切特征”处理器。
class ClCutFeature_Export LoopStartCutProcessor
{
public:
	/************************************************************************/
	//  检查回路的引入引出线和图形的干涉。

	// 检查内部回路的引入引出线和图形的干涉。
	// 注意:
	//   1) 引入引出线不干涉的条件：
	//      a) 引入引出线的外围矩形不得大于回路外围矩形的一半。
	//      b) 刺穿点必须在回路内部。
	//      c) 引入引出线和本回路的干涉量在一定范围内。
	//   2) this function should update the cache data of loop start/end cut object.
	//   3) 调整方法:
	//        步骤1: 先把引入引出线长度控制到回路长宽的一半。
	//        步骤2: 要是还是干涉，把起切点换几个位置试一试。
	//        步骤3: 要是还是干涉，把引入引出线缩短。
	//        步骤4: 此时要是还干涉，重复步骤2,3 N次。
	static void CheckInnerLoopIntf(LoopStartCutDataPtr pLoopStartCutData, double dAllowIntfArea = GEOM_TOLERANCE);

	// 检查外部回路的引入引出线和图形的干涉。
	// 注意:
	//   1) 外部回路不可能是阵列回路。
	//   2) 引入引出线不干涉的条件：
	//      a) 引入引出线的外围矩形不得大于回路外围矩形的一半。
	//   3) this function should update the cache data of loop start/end cut object.
	static void CheckOuterLoopIntf(LoopStartCutDataPtr pLoopStartCutData, double dAllowIntfArea = GEOM_TOLERANCE);

	// 回路的引入引出线和图形是否干涉。
	// 注:
	//   1) 引入引出线干涉的条件：
	//      a) 引入引出线不能被轮廓截断，如果引入引出线的端点接触到轮廓，不认为是截断。
	static BOOL IsLoopIntf(LoopStartCutDataPtr pLoopStartCutData);
	/************************************************************************/


	/************************************************************************/
	// 关于内外切。

	// 切换回路的内外切。
	// 注：
	//   1) 只能用在闭合回路上。
	//   2) 需要更新缓存。
	static void ReverseCutSide(LoopStartCutDataPtr pLoopStartCutData);

	// 切换内外切以及cut-on，顺序为外--on--内，这样循环。
	// 注：
	//  1) leadData_4_keepIt的含义参考action中的注释
	static void SwitchCutSide(LoopStartCutDataPtr pLoopStartCutData,
		const std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>& leadData_4_keepIt, const LeadConfigItem* pLeadConfigItem);

	// 判断闭合回路在里面切还是外面切。
	static PatternLoopCutOffsetType CalculateLoopCutOffset(const LoopStartCutData* pLoopStartCutData);

	// 为封闭轮廓设置切割偏移方式。没有更新缓存。
	static void SetLoopCutOffset_4_closedLoop(LoopStartCutDataPtr pLoopStartCutData);
	/************************************************************************/


	/************************************************************************/
	// 关于起切点，这几个函数没有放在StartCutPtProcessor中。

	// 调整起切点的位置。
	/* 参数：
	*  iNodeIndex: 新的起切点所在“回路节点”。
	*  startPt: 新的起切点。
	*/
	// 注：
	//   1) 需要保留调整之前的引入引出线长度(即使是零)。但是当起切点从角上调整到图形上时，
	//      引入引出线长度来自专家库，此时可能分配的引入引出线长度是零，那这种情况就不调整到原来的长度了。
	//   2) 需要保留调整之前的切割条件。
	//   3) 需要更新缓存。
	//   4) 有些情况不能调整。
	static void ChangeStartCutPt(LoopStartCutDataPtr pLoopStartCutData, int iNodeIndex, const Point2D& startPt, const LeadConfigItem* pLeadConfigItem);

	// 调整起切点的方式。
	// 注：
	//  1) 有的情况下不能调整到“newStartPt”。
	static MOV_START_CUT_TYPE StartCutPtMovType(LoopStartCutDataPtr pLoopStartCutData, const Point2D& newStartPt);

    // get the "position" info of the start cut point.
    // notes:
    //   1) if the start cut point is at corner, we need to figure out which side will be the start one.
    static void GetStartPtInfo(const LoopStartCutData* pLoopStartCutData, LONGLONG& iStartPatID, PatternPosition& startPatPos);
	/************************************************************************/


	// 给轮廓分配“回路起切特征”。
	// 注:
	//   1) this function also process the conditions of the pierce and lead in/out.
	//   2) this function will init start/end cut info, so do not use cache data in this function.
	//   3) this function should update the cache data after init the start/end cut object.
	static void AssignLoopStaEndCut(LoopStartCutDataPtr pLoopStartCutData, const LeadConfigItem* pLeadConfigItem, const PierceConfigItem* pPierceConfigItem, bool bUsed_cutOnTool,
        const Matrix2D& transformMatInSheet);

	// 检查“回路起切特征”是不是有效。
	// 注:
	//   1) if the start/end cut point is at geom before process, it will be not available when:
	//         a) the "start" pattern is lost.
	//         b) the loop has been open.
	//   2) if the start/end cut point is at the ends of open path before process, it will be not available when:
	//         a) the "start" pattern is lost.
	//         b) the "start" pattern is not at the ends of the loop.
	//         c) the loop has been close.
	//   3) if the start/end cut point is at corner before process, it will be not available when:
	//         a) two adjacent patterns are NOT connected as they did before.
	//		   b) the loop has been open.
	//      in this case, even if the a), b) items did not happened, we still should refine the start/end cut info for some cases, e.g. the corner angle was changed.
	//   4) 这个函数不用缓存。
	static BOOL IsAvailable(LoopStartCutDataPtr pLoopStartCutData);

	// 检查回路偏移后的“回路起切特征”。
	// 注：
	//  1) 只需检查闭合的多边形回路。
	//  2) 如果原来的起切点在图形上，偏移之后所在的图形丢失了，则把起切点分配到最近的角上。
	//  3) 如果原来的起切点在角上：
	//     a) 偏移之后角上的两个图形丢了一个或者都丢了，则把起切点分配到最近的角上。
	//     b) 偏移之后加了倒角，此时把起切点移到这个倒角线上。
	static void Check4Offset(LoopStartCutDataPtr pLoopStartCutData);

	// calc the un-pierce point.
	// notes:
	//   1) if array loop and bArrayLastInst is TRUE, just for last inst.
	//   2) this function will use the cache of start/end cut data.
	static Point2D CalcUnPiercePt(const LoopStartCutData* pLoopStartCutData, BOOL bArrayLastInst = FALSE);

	// get the loop that it's pierce point is closest to "pt".
	// notes:
	//   1) this function will use the cache of start/end cut data.
	static IPatternLoopPtr GetClosestLoop(LoopStartCutDataListPtr pLoopStaEndCutList, Point2D pt);

	// reverse the cut direction of the loop.
	// 注:
	//   1) for the lead in corner, we need to exchange its lead in/out.
	//   2) 需要更新缓存。
	static void ReverseCutDir(LoopStartCutDataPtr pLoopStartCutData);

	// update the cache data of loop start/end cut object.
	static void UpdateCache(LoopStartCutDataPtr pLoopStartCutData);
	static void UpdateCache(LoopStartCutDataListPtr pLoopStaEndCutList);

private:
	/************************************************************************/
	// get pierce/un-pierce pt for all type lead.

	// calc the pierce point.
	// notes:
	//   1) if array loop, just for first inst.
	//   2) call this function in LoopStartCutProcessor::UpdateCache and do not call this at other place.
	static Point2D CalcPiercePt(const LoopStartCutData* pLoopStartCutData);

	// get pierce/un-pierce pt for "line" type lead.
	static Point2D GetLineLeadPiercePt(const LoopStartCutData* pLoopStartCutData, LeadInOutType leadtype);

	// get pierce/un-pierce pt for "two line" type lead.
	// notes:
	//   1) we know that the do-pierce point of this lead is vertical to the cut direction, that means 
	//      the vector from the start point to do-pierce point is vertical to the cut direction vector.
	static Point2D GetTwoLineLeadPiercePt(const LoopStartCutData* pLoopStartCutData, LeadInOutType leadtype);

	// get pierce/un-pierce pt for "arc" type lead.
	static Point2D GetArcLeadPiercePt(const LoopStartCutData* pLoopStartCutData, LeadInOutType leadtype);

	// get pierce/un-pierce pt for "line arc" type lead.
	static Point2D GetLineArcLeadPiercePt(const LoopStartCutData* pLoopStartCutData, LeadInOutType leadtype);

	// get pierce/un-pierce pt for "hook" type lead.
	static Point2D GetHookLeadPiercePt(const LoopStartCutData* pLoopStartCutData, LeadInOutType leadtype);	
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
