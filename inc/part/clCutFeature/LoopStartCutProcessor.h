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


// ���е�����ķ�ʽ��
enum MOV_START_CUT_TYPE
{
	GEOM_CORNER_2_CORNER	= 0, // ��ͼ�λ�ǵ��������ϡ�
	CORNER_2_GEOM			= 1, // �ӽǵ�����ͼ���ϡ�
	GEOM_2_GEOM				= 2, // ��ͼ�ε�����ͼ���ϡ�
	CANNOT_MOVE				= 3, // ���ܵ������µ����е�λ�ú��ϵ�һ��������180�ȵĽǴ�����
};


// ����·������������������
class ClCutFeature_Export LoopStartCutProcessor
{
public:
	/************************************************************************/
	//  ����·�����������ߺ�ͼ�εĸ��档

	// ����ڲ���·�����������ߺ�ͼ�εĸ��档
	// ע��:
	//   1) ���������߲������������
	//      a) ���������ߵ���Χ���β��ô��ڻ�·��Χ���ε�һ�롣
	//      b) �̴�������ڻ�·�ڲ���
	//      c) ���������ߺͱ���·�ĸ�������һ����Χ�ڡ�
	//   2) this function should update the cache data of loop start/end cut object.
	//   3) ��������:
	//        ����1: �Ȱ����������߳��ȿ��Ƶ���·�����һ�롣
	//        ����2: Ҫ�ǻ��Ǹ��棬�����е㻻����λ����һ�ԡ�
	//        ����3: Ҫ�ǻ��Ǹ��棬���������������̡�
	//        ����4: ��ʱҪ�ǻ����棬�ظ�����2,3 N�Ρ�
	static void CheckInnerLoopIntf(LoopStartCutDataPtr pLoopStartCutData, double dAllowIntfArea = GEOM_TOLERANCE);

	// ����ⲿ��·�����������ߺ�ͼ�εĸ��档
	// ע��:
	//   1) �ⲿ��·�����������л�·��
	//   2) ���������߲������������
	//      a) ���������ߵ���Χ���β��ô��ڻ�·��Χ���ε�һ�롣
	//   3) this function should update the cache data of loop start/end cut object.
	static void CheckOuterLoopIntf(LoopStartCutDataPtr pLoopStartCutData, double dAllowIntfArea = GEOM_TOLERANCE);

	// ��·�����������ߺ�ͼ���Ƿ���档
	// ע:
	//   1) ���������߸����������
	//      a) ���������߲��ܱ������ضϣ�������������ߵĶ˵�Ӵ�������������Ϊ�ǽضϡ�
	static BOOL IsLoopIntf(LoopStartCutDataPtr pLoopStartCutData);
	/************************************************************************/


	/************************************************************************/
	// ���������С�

	// �л���·�������С�
	// ע��
	//   1) ֻ�����ڱպϻ�·�ϡ�
	//   2) ��Ҫ���»��档
	static void ReverseCutSide(LoopStartCutDataPtr pLoopStartCutData);

	// �л��������Լ�cut-on��˳��Ϊ��--on--�ڣ�����ѭ����
	// ע��
	//  1) leadData_4_keepIt�ĺ���ο�action�е�ע��
	static void SwitchCutSide(LoopStartCutDataPtr pLoopStartCutData,
		const std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>& leadData_4_keepIt, const LeadConfigItem* pLeadConfigItem);

	// �жϱպϻ�·�������л��������С�
	static PatternLoopCutOffsetType CalculateLoopCutOffset(const LoopStartCutData* pLoopStartCutData);

	// Ϊ������������и�ƫ�Ʒ�ʽ��û�и��»��档
	static void SetLoopCutOffset_4_closedLoop(LoopStartCutDataPtr pLoopStartCutData);
	/************************************************************************/


	/************************************************************************/
	// �������е㣬�⼸������û�з���StartCutPtProcessor�С�

	// �������е��λ�á�
	/* ������
	*  iNodeIndex: �µ����е����ڡ���·�ڵ㡱��
	*  startPt: �µ����е㡣
	*/
	// ע��
	//   1) ��Ҫ��������֮ǰ�����������߳���(��ʹ����)�����ǵ����е�ӽ��ϵ�����ͼ����ʱ��
	//      ���������߳�������ר�ҿ⣬��ʱ���ܷ�������������߳������㣬����������Ͳ�������ԭ���ĳ����ˡ�
	//   2) ��Ҫ��������֮ǰ���и�������
	//   3) ��Ҫ���»��档
	//   4) ��Щ������ܵ�����
	static void ChangeStartCutPt(LoopStartCutDataPtr pLoopStartCutData, int iNodeIndex, const Point2D& startPt, const LeadConfigItem* pLeadConfigItem);

	// �������е�ķ�ʽ��
	// ע��
	//  1) �е�����²��ܵ�������newStartPt����
	static MOV_START_CUT_TYPE StartCutPtMovType(LoopStartCutDataPtr pLoopStartCutData, const Point2D& newStartPt);

    // get the "position" info of the start cut point.
    // notes:
    //   1) if the start cut point is at corner, we need to figure out which side will be the start one.
    static void GetStartPtInfo(const LoopStartCutData* pLoopStartCutData, LONGLONG& iStartPatID, PatternPosition& startPatPos);
	/************************************************************************/


	// ���������䡰��·������������
	// ע:
	//   1) this function also process the conditions of the pierce and lead in/out.
	//   2) this function will init start/end cut info, so do not use cache data in this function.
	//   3) this function should update the cache data after init the start/end cut object.
	static void AssignLoopStaEndCut(LoopStartCutDataPtr pLoopStartCutData, const LeadConfigItem* pLeadConfigItem, const PierceConfigItem* pPierceConfigItem, bool bUsed_cutOnTool,
        const Matrix2D& transformMatInSheet);

	// ��顰��·�����������ǲ�����Ч��
	// ע:
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
	//   4) ����������û��档
	static BOOL IsAvailable(LoopStartCutDataPtr pLoopStartCutData);

	// ����·ƫ�ƺ�ġ���·������������
	// ע��
	//  1) ֻ����պϵĶ���λ�·��
	//  2) ���ԭ�������е���ͼ���ϣ�ƫ��֮�����ڵ�ͼ�ζ�ʧ�ˣ�������е���䵽����Ľ��ϡ�
	//  3) ���ԭ�������е��ڽ��ϣ�
	//     a) ƫ��֮����ϵ�����ͼ�ζ���һ�����߶����ˣ�������е���䵽����Ľ��ϡ�
	//     b) ƫ��֮����˵��ǣ���ʱ�����е��Ƶ�����������ϡ�
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
	// ע:
	//   1) for the lead in corner, we need to exchange its lead in/out.
	//   2) ��Ҫ���»��档
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
