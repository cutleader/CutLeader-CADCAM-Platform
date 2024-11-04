#pragma once

#include "clGeometryFeatureCommon.h"
#include "baseConst.h"
#include "Point2D.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(IPattern)
DECLARE_CUTLEADER_CLASS(IPatternLoop)
DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PolygonPatternLoop)
DECLARE_CUTLEADER_CLASS(ArrayPatternLoop)
DECLARE_CUTLEADER_CLASS(StdPatternLoop)
DECLARE_CUTLEADER_CLASS(PolygonPattern)

BEGIN_CUTLEADER_NAMESPACE()

// the connection type of the pattern to an open loop.
enum PAT_CONNECT_TYPE
{
	PAT_CONNECT_NOT						= -1,

	PAT_CONNECT_START2HEAD				= 0,
	PAT_CONNECT_START2TAIL				= 1,
	PAT_CONNECT_END2HEAD				= 2,
	PAT_CONNECT_END2TAIL				= 3,
};

// the connection type between two open loops.
typedef enum tagLoopConnectType
{	
	LOOP_CONNECT_NOT			= -1,

	LOOP_CONNECT_HEAD2HEAD		= 0,
	LOOP_CONNECT_HEAD2TAIL		= 1,
	LOOP_CONNECT_TAIL2HEAD		= 2,
	LOOP_CONNECT_TAIL2TAIL		= 3,
} LOOP_CONNECT_TYPE;

// this class will generate loops from the pattern list.
class ClGeometryFeature_Export LoopBuilder
{
public:
	/************************************************************************/
	// build loop by patterns.
	
	// build pattern loops from "pPatList".
	// notes:
	//   1) by default we use SYSTEM_TOLERANCE as the criteria to check
	//      whether geometries are connected.
	static PatternLoopListPtr BuildPatternLoops(PatternListPtr pPatList, double dTol = SYSTEM_TOLERANCE);

	// build polygon loops with line/arc patterns.
	// notes:
	//   1) this function should not use/update the cache data of the pattern loop.
	static PatternLoopListPtr BuildPolygonPatternLoops(PatternListPtr pPatList, double dTol = SYSTEM_TOLERANCE);

	// build a polygon loop from the standard pattern.
	// notes:
	//   1) the direction of new polygon loop is CCW, just same as line/arc in "pStdPat".
	//   2) the cache data of pattern/loop should be updated.
	static PolygonPatternLoopPtr BuildPolyLoopByStdPat(const IPattern* pStdPat);
	/************************************************************************/

	/************************************************************************/
	// build polygon loop by other type loop.

	// build polygon loop by array pattern loop.
	// notes:
	//   1) we only convert the first inst of the array pattern to the polygon pattern.
	//   2) the cache data of pattern/loop should be updated.
	static PolygonPatternLoopPtr BuildPolyLoopByArrayLoop(const ArrayPatternLoop* pArrayLoop);

	// build polygon loop by std pattern loop.
	// notes:
	//   1) the cache data of pattern/loop should be updated.
	static PolygonPatternLoopPtr BuildPolyLoopByStdLoop(const StdPatternLoop* pStdLoop);

	// build a new polygon loop by pattern loop.
	// notes:
	//   1) if "pPatternLoop" is polygon pattern loop, only clone it.
	//   2) if "pPatternLoop" is std pattern loop, need to call BuildPolyLoopByStdLoop().
	//   2) if "pPatternLoop" is array pattern loop, need to call BuildPolyLoopByArrayLoop().
	static PolygonPatternLoopPtr BuildPolyLoopByPatternLoop(const IPatternLoop* pPatternLoop);

	// build the polygon loop by polygon.
	// ע:
	//   1) this function just connect patterns in "pPatList" as a loop simply.
	//   2) patterns in "pPatList" are connected head-tail, but the direction may be not same.
	//   3) ����������2����β������ֱ�߻�����ȫ�ص�������Բ�������ؿա�
	/* params:
	*  pPatList: patterns within it will connect head by tail.
	*/
	static PolygonPatternLoopPtr BuildPolyLoopByPolygon(PatternListPtr pPatList);

	// �Ӷ����pattern���ɶ���λ�·�������������Ҳ��ά�����档
	static PolygonPatternLoopPtr BuildPolyLoopByPolygonPat(PolygonPatternPtr pPolygonPat);

	// ��������Щ�����������ɱպ������������˵�ҲҪ���ء�
	// notes:
	//   1) the function will not use the cache data of the loop.
	//   2) ����Ŀ����������б��е�˳����ܻᱻ������������������û�б��ı�
	static PatternLoopListPtr BuildClosedPolyLoops_byOpenPolyLoops(PatternLoopListPtr pOpenPatternLoopList, double dTol = SYSTEM_TOLERANCE);
	static std::vector<PolygonPatternPtr> BuildClosedPolygonPats_byOpenPolygonPats(std::vector<PolygonPatternPtr> polygonPatList_open, double dTol = SYSTEM_TOLERANCE);
	/************************************************************************/

private:
	// get the connection info of the pattern to an open loop.
	/* params:
	*  headPt, tailPt: the head/tail point of the open loop.
	*  pPattern: ֱ�߻���Բ��pattern
	*  dTol: use this to check whether two patterns are connected.
	*  dMinDis: return the min distance whether they are connected or not.
	*  pPolygonPatternLoop: ���������еĶ��������
	*  bCannotOverlapWithPrePattern: pPattern�ǲ��ǲ��ܺ���һ��pattern��ȫ�ص�
	*/
	// notes:
	//   1) if start point of "pPattern" is connected to "tailPt", we return PAT_CONNECT_START2TAIL.
	//   2) if start/end point of "pPattern" are all connected to "headPt", we choose the closer one.
	static PAT_CONNECT_TYPE GetPatConnectInfo(Point2D headPt, Point2D tailPt, IPatternPtr pPattern, double dTol, double& dMinDis, const PolygonPatternLoop* pPolygonPatternLoop,
		bool bCannotOverlapWithPrePattern);

	// get the connection info between two open loops.
	/* params:
	*  headPt, tailPt: the head/tail point of the open loop.
	*  dTol: use this to check whether two patterns are connected.
	*  dMinDis: return the min distance whether they are connected or not.
	*/
	// notes:
	//   1) if start point of "pPolygonPatternLoop" is connected to "tailPt", we return LOOP_CONNECT_HEAD2TAIL.
	//   2) if start/end point of "pPolygonPatternLoop" are all connected to "headPt", we choose the closer one.
	static LOOP_CONNECT_TYPE GetLoopConnectInfo(Point2D headPt, Point2D tailPt, PolygonPatternLoopPtr pPolygonPatternLoop, double dTol, double& dMinDis);
	static LOOP_CONNECT_TYPE GetLoopConnectInfo(const Point2D& headPt, const Point2D& tailPt, const PolygonPattern* pPolygonPat, double dTol);

    // �ж�������pattern�ǲ�����ȫ�غϣ�����߾������жϣ�������pattern��ֱ�߻���Բ����
    static bool TwoPatternsOverlapped(const IPattern* pPattern1, const IPattern* pPattern2);
};

END_CUTLEADER_NAMESPACE()
