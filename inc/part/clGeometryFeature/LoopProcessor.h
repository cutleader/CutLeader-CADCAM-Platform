#pragma once

#include "baseConst.h"
#include "PatternPosition.h"
#include "Point2D.h"
#include <vector>
#include <set>

DECLARE_CUTLEADER_CLASS(Point2DList)
DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(IPattern)
DECLARE_CUTLEADER_CLASS(LoopTopologyItem)
DECLARE_CUTLEADER_CLASS(LoopTopologyItemList)
DECLARE_CUTLEADER_CLASS(PatternLoopNodeList)
DECLARE_CUTLEADER_CLASS(IPatternLoop)
DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(PolygonPatternLoop)
DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PolygonPattern)

BEGIN_CUTLEADER_NAMESPACE()

// “图形回路”处理器。
class ClGeometryFeature_Export LoopProcessor
{
public:
	/************************************************************************/
	// about change the pattern in the loop list.

	// adjust "pPattern" to the head of the loop.
	// notes:
	//   1) call this for close polygon loop.
	static void AdjustToHead(IPatternLoopPtr pPatternLoop, IPatternPtr pPattern);

	// add a pattern to "pPatternLoopList".
	/* params:
	*  dTol: when need to re-calculate the loops, need it.
	*/
	// notes:
	//   1) this operation may affect those open paths, so we should collect open loops
	//      which "link" to "pPattern", if we only find one loop, append the pattern to the loop,
	//      BUT if this pattern affects more than one loop, re-calculate these loops.
	//   2) function without "Ex" should not use/update the cache data of the loops.
	//   3) function with "Ex" should update the cache data and the boundary loop of the loops.
	static void AddPatToLoopList(PatternLoopListPtr pPatternLoopList, IPatternPtr pPattern, double dTol = GEOM_TOLERANCE);
	static void AddPatToLoopListEx(PatternLoopListPtr pPatternLoopList, IPatternPtr pPattern, double dTol = GEOM_TOLERANCE);

	// delete a pattern from the PatternLoopList, this will affect the loop which contains this pattern.
	/* params:
	*  dTol: when need to re-calculate the loops, need it.
	*/
	// notes:
	//   1) if the loop is a closed "polygon" path, we should move "pPattern" to 
	//      the head, and then delete "pPattern", the close path become open path.
	//   2) for open "polygon" path, if we remove the pattern at it head or tail, 
	//      just remove it. BUT if user deleted a pattern not at the head or tail, 
	//      we should delete this loop, and rebuild two loops.
	//   3) function without "Ex" should not use/update the cache data of the loops.
	//   4) function with "Ex" should update the cache data and the boundary loop of the loops.
	static void DelPatFromLoopList(PatternLoopListPtr pPatternLoopList, IPatternPtr pPattern, double dTol = GEOM_TOLERANCE);
	static void DelPatFromLoopListEx(PatternLoopListPtr pPatternLoopList, IPatternPtr pPattern, double dTol = GEOM_TOLERANCE);

	// change patterns in loop list.
	/* params:
	*  pPatList: the "after modified" pattern data.
	*/
	// notes:
	//   1) work flow:
	//		step 1: if we can find a closed loop in "pPatList", we will "protect" that loop.
	//		step 2: for other case, we can call "RemovePattern" and then call "AddPattern".
	//   2) "pPatList" may be changed in this function, so caller should be aware of this.
	//   3) this function should update the cache data and the boundary loop of the loops.
	static void ChangePatInLoopList(PatternLoopListPtr pPatternLoopList, PatternListPtr pPatList);

	// remove the tiny nodes from the loop.
	// notes:
	//   1) currently, this function only care the polygon loop.
	//   2) "dVal" will be compared with the boundary rect of the loop node.
	//   3) the function will use the cache data of the pattern.
	static void RemoveTinyLoopNode(PatternLoopListPtr pPatternLoopList, PatternListPtr pPatList, double dVal);
	static void RemoveTinyLoopNode(PatternLoopListPtr pPatternLoopList, double dVal, std::set<LONGLONG>& polyLoops_deletedNodes);
	/************************************************************************/

	/************************************************************************/
	// about the corner of the loop.

	// whether the point is at the loop corner.
	// notes:
	//   1) this function will check whether "pt" is at the loop corner, if yes, update "pt" with the precise corner point.
	static BOOL PtAtLoopCorner(const IPatternLoop* pPatternLoop, Point2D& pt);

	// get the angle of two adjacent base geom at point(cornerPt).
	// 注:
	//   1) do not call this for open path.
	//   2) do not call this if only one line/arc geom in the loop, e.g. a "round hole" pattern loop.
	//   3) 这个角是从回路内部看的。
	static double GetAngAtLoopCorner(const IPatternLoop* pPatternLoop, const Point2D& cornerPt);

	// get PatternPosition data of the adjacent geom.
	// notes:
	//   1) before call this, make sure that "cornerPt" is at the loop corner.
	//   2) refer to the illustration, if we have gotten the PatternPosition at geom1, this function will return the PatternPosition at geom2.
	/*     
						  *        
						  *  geom2      
						  *        
						  *        
						  *        
						  *        
			*************** corner point       
                  geom1
	*/
	static void GetNextCornerGeomPos(const IPatternLoop* pPatternLoop, const Point2D& cornerPt, LONGLONG iFirstPatID, const PatternPosition& firstPatPos,
									 LONGLONG& iSecondPatID, PatternPosition& secondPatPos);
	/************************************************************************/

	/************************************************************************/
	// about optimizing the loop.

	// merge some loop nodes as line.
	// notes:
	//   1) there are all line nodes in "pPatternLoopList".
	static void OptimizeToLine(PatternLoopListPtr pPatternLoopList, PatternListPtr pPatList, double dDelta);

	// 把一些直线节点拟合成直线。
	// 注：
	//  1) 这个函数不用也不更新缓存。
    static void OptimizeToLine_1(PolygonPatternLoopPtr pPolygonPatternLoop, double dDelta);
    static void OptimizeToLine_2(PolygonPatternPtr pPolygonPat, double dDelta); // 基于曲率判断
    static void OptimizeToLine_3(PolygonPatternPtr pPolygonPat, double dDelta); // 基于实际偏差判断

	// merge some loop nodes as arc.
	// notes:
	//   1) there are all line nodes in "pPatternLoopList".
	//   2) 这个函数不用也不更新缓存
    //   3) 这个函数目前被废弃了。
	static void OptimizeToArc(PatternLoopListPtr pPatternLoopList, PatternListPtr pPatList, double dAngleDelta, double dLengthDelta);

	// merge line nodes which are coline.
	// notes:
	//   1) 这个函数不用也不更新缓存
	static void MergeCoLineNode_1(PolygonPatternLoopPtr pPolygonPatternLoop);
	static void MergeCoLineNode_2(PolygonPatternPtr pPolygonPat);

	// merge some loop nodes as arc.
	// notes:
	//   1) if the sweep angle is smaller than GEOM_TOLERANCE, use line instead.
    //   2) 这个函数不用也不更新缓存。
	/* params:
	*  dTolerance: the allowed offset between the old curve and the new curve.
	*/
	static void OptimizeToArc_1(PolygonPatternLoopPtr pPolygonPatternLoop, double dTolerance);
    static void OptimizeToArc_2(PolygonPatternPtr pPolygonPat, double dTolerance);

	// 删除重复的回路。两两比较，只要有重复，即删掉其中一个。判断的标准为外接矩形重叠，且回路长度和节点数相等，其实此时回路不一定完全重叠。
	static bool TwoLoopsOverlapped(const IPatternLoop* pPatternLoop1, const IPatternLoop* pPatternLoop2);
	static void DeleteOverlappedLoops(PatternLoopListPtr pPatternLoopList);
	/************************************************************************/

	/************************************************************************/
	// about print debug info.

	// print the start/end points of the polygon loop's node.
	static void PrintPolyLoopPt(IPatternLoopPtr pPatternLoop);

	// print the gap size between each loop node.
	static void PrintPolyLoopGap(IPatternLoopPtr pPatternLoop);

	// display each loop segment.
	static void DisplayLoopSegment(std::vector<PatternLoopNodeListPtr> loopSegmentList);
    static void DisplayLoopSegment(std::vector<LineArc2DListPtr> loopSegmentList);
	/************************************************************************/

	/************************************************************************/
	// about update cache.

	// update the cache data of pattern loop object.
	// notes:
	//   1) before call this, make sure cache data of patterns are updated.
	static void UpdateCache(PatternLoopListPtr pPatternLoopList);
	static void UpdateCache(IPatternLoopPtr pPatternLoop);

	// update the cache data of pattern and loop object.
	// notes:
	//   1) care control pattern.
	static void UpdateCacheEx(PatternLoopListPtr pPatternLoopList);
	static void UpdateCacheEx(IPatternLoopPtr pPatternLoop);
	/************************************************************************/

	/************************************************************************/
	// 关于“偏移”。

	// 偏移回路。
	// 注：
	//   1) 偏移完需要更新“pPatList”。
	//   2) 该函数会更新“图形”和“回路”的缓存。
	static BOOL OffsetPolyLoop(PatternListPtr pPatList, IPatternLoopPtr pPatternLoop, BOOL bInside, double dOffsetVal);

	// 偏移回路。
	// 注：
	//   1) 该函数会更新“图形”和“回路”的缓存。
	static BOOL OffsetPolyLoop(IPatternLoopPtr pPatternLoop, BOOL bInside, double dOffsetVal, const std::vector<std::pair<LONGLONG, LONGLONG>>& corners);
	/************************************************************************/

	// clamp polygon loop tightly.
	/* params:
	*  dConnectTol: the connect tolerance which was used to build "pPolygonPatternLoop".
	*/
	// notes:
	//   1) sometimes, line/arc patterns within the loop are not definitely connected, you know, 
	//      if we use a small tolerance, the system may think these line/arc patterns cannot 
	//      compose the loop. so this function use "dConnectTol=0" to clamp the loop tightly.
	//   2) loop node count will greater than 1.
	//   3) when import the external data, need to call this.
	//   4) 这个函数没有用也没有更新图形对象的缓存。
	static void ClampPolygonLoop(PolygonPatternLoopPtr pPolygonPatternLoop, double dConnectTol);
	// 这个多边形回路之前是闭合的，之前可能经历了“去除小图形”，从而可能不闭合了，这个函数再让它闭合。
	// 注：如果多边形中只有两个圆弧或者两个直线，且一旦它们的端点连起来后会导致这两个图形重叠，用的时候需要知道这种情况。
	static void ClampClosePolyLoop(PolygonPatternLoopPtr pPolygonPatternLoop);

	// 找出开放回路中的闭合回路。
	// 注:
	//   1) 比如遇到一个形状像“6”的开放回路，我们可以提取出一个闭合回路，
	//      提取出闭合回路后剩下的部分将被建成新的回路。
	//   2) 这个函数没有用也没有更新图形对象的缓存。
	static void FindCloseInOpenLoop(PatternLoopListPtr pPatternLoopList, double dConnectTol);

	// get first 8-percent point of the loop.
	/* 参数:
	*  dPercentAngle: 8-multiple of PI/4.
	*  iIndex: the index of the loop node which hold the cross point.
	*/
	// notes:
	//   1) for array loop, get the point on the first inst.
	//   2) if no cross point, get the loop's closest point, refer the code.
	static void GetFst8PercentPt(const IPatternLoop* pPatternLoop, double dPercentAngle, Point2D& crossPt, int& iIndex);

	// 得到“图形回路”的16等分点和这些等分点所在图形的端点。
	/* 参数：
	*  pPercentPts, pPercentIndexs: 返回的等分点及所在的回路节点。
	*  pCornerPts, pCornerIndexs: 返回的端点及所在的回路节点。
	*  dFloatAngle: 取角点时，角上两个图形的夹角不能在180度左右这个范围内。
	*/
	// 注：
	//  1) 按“外接正方形”的边的逆时针方向等分，交点上面的等分点被抛弃。
	//  2) 重复的图形端点被去除。
	//  3) 不需要180度左右一定范围内的角点。
	//  4) 外轮廓不需要角度大于270度的角点，内轮廓不需要角度小于90度的角点。
	static void Get16PercentPt(const IPatternLoop* pPatternLoop, Point2DListPtr pPercentPts, IntegerListPtr pPercentIndexs,
							   Point2DListPtr pCornerPts, IntegerListPtr pCornerIndexs, double dFloatAngle);

	// 分析出图形回路之间的拓扑关系。
	// 注：
	//   1) 开放回路只能被闭合回路包含，而不能包含其他回路。
	//   2) 要分析的回路中不要包含阵列回路。
	static LoopTopologyItemListPtr CalcLoopTopStruct(const PatternLoopList* pPatternLoopList);

    // 根据拓扑关系判断要不要分拆成多个零件。
    static bool NeedToSeperate(const LoopTopologyItemList* pLoopTopItemList);

	// 得到轮廓的图层和颜色索引信息，取的是第一个图形上面的信息
	static CString GetLayerName_of_loop(const IPatternLoop* pPatternLoop);
	static int GetColorIndex_of_loop(const IPatternLoop* pPatternLoop);

    // 两个回路的形状是不是一样。形状一样的意思是，一个回路通过平移或旋转，能和另一个回路完全贴合起来。
	// 注：
	//  1) 目前要求传入的回路要是封闭的。
    //  2) 要是形状相同，需要返回从回路1变换到回路2的矩阵，注意，这里的变换矩阵可能不止一个。
	//  3) 有个例外，就是当轮廓是圆的时候，返回的矩阵只有平移变换，因为旋转变换有无数个。
	static bool TwoCloseLoops_sameShape(const IPatternLoop* pPatternLoop1, const IPatternLoop* pPatternLoop2, std::vector<Matrix2D>& matrixList_1to2, bool& bLoop_is_circle);

	// 判断开放回路1按照矩阵变换后是不是和开放回路2完全重叠。如果矩阵是默认值，那么就是判断这两个回路是否完全重叠。
	static bool TwoCloseLoops_totallyOverlapped(const IPatternLoop* pPatternLoop1, const IPatternLoop* pPatternLoop2, const Matrix2D& mat_1to2);
	static bool TwoOpenLoops_totallyOverlapped(const IPatternLoop* pPatternLoop1, const IPatternLoop* pPatternLoop2, const Matrix2D& mat_1to2);

private:
	// calc the dir factor of two points.
	static void CalcTwoPtDir(Point2D prePt, Point2D postPt, double& dTanX, double& dTanY);

	// separate the loop.
	/* 参数：
	*  bCutS: 是否把S型的部分切开。
	*  bLine_has_similarLength: segment中的线的长度是不是需要“差不多长”（不超过10倍），每个圆弧自己单独一个segment
	*/
	// 注:
	//   1) one arc will be one "segment".
	//   2) the returned vector will follow the loop order.
	//   3) the geometry like "v" will be separated to 2 segments because the angle of two nodes at vertex
	//      is not close to 180. so the geometry like "u" will not be separated.
	static std::vector<PatternLoopNodeListPtr> SeparateLoop_1(PolygonPatternLoopPtr pPolygonPatternLoop, BOOL bCutS, BOOL bLine_has_similarLength);
    static std::vector<LineArc2DListPtr> SeparateLoop_2(PolygonPatternPtr pPolygonPat, BOOL bCutS, BOOL bLine_has_similarLength);

	// 把“回路拓扑项”对象放到拓扑集合中。
	// 注：
	//   1) 根据拓扑关系，把“pLoopTopItem”插入到“pLoopTopItemList”中合适的位置。
	static void AddLoopToTop(LoopTopologyItemListPtr pLoopTopItemList, LoopTopologyItemPtr pLoopTopItem);
};

END_CUTLEADER_NAMESPACE()
