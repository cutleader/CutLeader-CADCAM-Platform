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
// ע��
//   1) ����������ͼ�εĳ��ȵ�һ��ȡ�����ֵ������ʱ����ͼ��1/3�����С�
class ClCutSequence_Export CommonCutSequenceCutter : public SequenceCutterBase
{
public:
	CommonCutSequenceCutter(CommonCutSequencePtr pComCutSequenceUnit);
	~CommonCutSequenceCutter(void);

public:
	// �ѡ����߹���ת��Ϊ���и�ڵ㡱���ϡ�
	/* ����:
	*  lastCutPt: ���ӹ��е�����ʱ�����������и��������ߡ�
	*  pCutLoopInstList: ���и�ġ���·ʵ����������������������
	*  bAvoidToNext: whether we need avoid from this sequence to next sequence.
	*/
	CutNodeListPtr ComSeq2CutNodes(Point2D& lastCutPt, const LoopInstanceList* pCutLoopInsts, EdgeLoopListPtr edgeLoops, MicroJointDatasOfComCutGroupPtr pMicroJointDatasOfComCutGroup,
                                   BOOL bAvoidToNext = FALSE);

	// �ӡ��߻�·�����ɡ����и�ڵ㡱���ϡ�
	// ע:
	//   1) cut "pEdgeLoop" from "pStartEdge", we will stop unless we reach "pStartEdge" again.
	//   2) when cutting a scrap edge, we will jump to the adjacent loop to cut the "scrap loop", BUT we can jump back.
	static EdgeCutNodeListPtr EdgeLoop2EdgeCutNodes(EdgeLoopPtr pEdgeLoop, IEdgePtr pStartEdge);

private:
	/************************************************************************/
	// �������ɡ����и�ڵ㡱��

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
	// �������ɡ��и�ڵ㡱��

	// �ӡ����и�ڵ㡱�������ɡ��и�ڵ㡱���ϡ�
	/* ����:
	*  pEdgeCutNodes: ��һ�����������������ɵġ����и�ڵ㡱��
	*  pCutLoopInstList: ���и�ġ���·ʵ����������������������
	*/
	// ע:
	//   1) ��Ҫ��������������������ڲ��ı��ã������������������ǰһ������ı������ﲻ����
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

    // �õ�һ��line2d������ź����΢���ӣ���Щ΢���Ӷ������ʵ��Ҫ�и�ġ�
    MicroJointDatasOfComCutGroupPtr GetSortedMicroJoints_on_line2d(MicroJointDatasOfComCutGroupPtr pMicroJointDatasOfComCutGroup, Line2DPtr pLine2D);
	/************************************************************************/

private:
	CommonCutSequencePtr m_pComCutSequence;
};

END_CUTLEADER_NAMESPACE()
