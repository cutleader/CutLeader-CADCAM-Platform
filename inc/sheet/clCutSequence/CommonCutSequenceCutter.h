#pragma once

#include "SequenceCutterBase.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(Line2D)
DECLARE_CUTLEADER_CLASS(LineArc2DLoop)
DECLARE_CUTLEADER_CLASS(MicroJoint)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(IEdge)
DECLARE_CUTLEADER_CLASS(EdgeLoop)
DECLARE_CUTLEADER_CLASS(EdgeLoopList)
DECLARE_CUTLEADER_CLASS(EdgeCutNode)
DECLARE_CUTLEADER_CLASS(EdgeCutNodeList)
DECLARE_CUTLEADER_CLASS(CommonCutSequence)
DECLARE_CUTLEADER_CLASS(MicroJointDatasOfComCutGroup)

BEGIN_CUTLEADER_NAMESPACE()

// this class will convert common cut sequence to cut nodes.
// 注：
//   1) 当过切所在图形的长度的一半比“过切值”还短时，在图形1/3处过切。
class ClCutSequence_Export CommonCutSequenceCutter : public SequenceCutterBase
{
public:
	CommonCutSequenceCutter(CommonCutSequencePtr pComCutSequenceUnit);
	~CommonCutSequenceCutter(void);

public:
	// 把“共边工序”转化为“切割节点”集合。
	/* 参数:
	*  lastCutPt: 当从过切点起切时，返回最后的切割点给调用者。
	*  pCutLoopInstList: 已切割的“回路实例”，这个函数会更新它。
	*  bAvoidToNext: whether we need avoid from this sequence to next sequence.
	*/
	CutNodeListPtr ComSeq2CutNodes(Point2D& lastCutPt, const LoopInstanceList* pCutLoopInsts, EdgeLoopListPtr edgeLoops, MicroJointDatasOfComCutGroupPtr pMicroJointDatasOfComCutGroup,
                                   BOOL bAvoidToNext = FALSE);

	// 从“边回路”生成“边切割节点”集合。
	// 注:
	//   1) cut "pEdgeLoop" from "pStartEdge", we will stop unless we reach "pStartEdge" again.
	//   2) when cutting a scrap edge, we will jump to the adjacent loop to cut the "scrap loop", BUT we can jump back.
	static EdgeCutNodeListPtr EdgeLoop2EdgeCutNodes(EdgeLoopPtr pEdgeLoop, IEdgePtr pStartEdge);

private:
	/************************************************************************/
	// 关于生成“边切割节点”。

	// cut the outer edge.
	// notes:
	//   1) in this function, we only set "pOuterEdge" as "HasBeCut" although we will pre-cut other edge.
	static EdgeCutNodeListPtr CutOuterEdge(EdgeLoopPtr pEdgeLoop, IEdgePtr pOuterEdge);

	// cut the com edge.
	// notes:
	//   1) in this function, we only set "pComEdge" and its adjacent
	//      edge as "HasBeCut" although we will pre-cut other edge.
	static EdgeCutNodeListPtr CutComEdge(EdgeLoopPtr pEdgeLoop, IEdgePtr pComEdge);

	// cut the scrap edge.
	static EdgeCutNodeListPtr CutScrapEdge(EdgeLoopPtr& pEdgeLoop, IEdgePtr& pScrapEdge);
	/************************************************************************/

	/************************************************************************/
	// 关于生成“切割节点”。

	// 从“边切割节点”集合生成“切割节点”集合。
	/* 参数:
	*  pEdgeCutNodes: 从一个“共边轮廓”生成的“边切割节点”。
	*  pCutLoopInstList: 已切割的“回路实例”，这个函数会更新它。
	*/
	// 注:
	//   1) 需要处理这个“共边轮廓”内部的避让，这个“共边轮廓”和前一个工序的避让这里不处理。
	CutNodeListPtr CutEdgeCutNodes(Point2D& lastHitPt, EdgeCutNodeListPtr pEdgeCutNodes, const LoopInstanceList* pCutLoopInsts, MicroJointDatasOfComCutGroupPtr pMicroJointDatasOfComCutGroup);

	// convert EdgeCutNode to CutNodeList.
	// notes:
	//   1) these functions should update the last hit point.
	CutNodeListPtr ConvertEdgeCutStart(EdgeCutNodePtr pEdgeCutNode, Point2D& lastHitPt);
	CutNodeListPtr ConvertEdgeCutEnd(EdgeCutNodePtr pEdgeCutNode, Point2D& lastHitPt);

	// this function always one cut node(line/arc).
	/* params:
	*  startPt: the start cut point of the cut node returned.
	*/
	CutNodeListPtr ConvertEdgeCutRegular(EdgeCutNodePtr pEdgeCutNode, Point2D& startPt, Point2D& lastHitPt, MicroJointDatasOfComCutGroupPtr pMicroJointDatasOfComCutGroup);

    // 得到一个line2d上面的排好序的微连接，这些微连接对象就是实际要切割的。
    MicroJointDatasOfComCutGroupPtr GetSortedMicroJoints_on_line2d(MicroJointDatasOfComCutGroupPtr pMicroJointDatasOfComCutGroup, Line2DPtr pLine2D);
	/************************************************************************/

private:
	CommonCutSequencePtr m_pComCutSequence;
};

END_CUTLEADER_NAMESPACE()
