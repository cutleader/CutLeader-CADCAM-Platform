#pragma once

#include "SequenceCutterBase.h"
#include <vector>


DECLARE_CUTLEADER_CLASS(Point2DList)
DECLARE_CUTLEADER_CLASS(Line2D)
DECLARE_CUTLEADER_CLASS(Arc2D)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(BridgeSequence)
DECLARE_CUTLEADER_CLASS(BridgeNode)
DECLARE_CUTLEADER_CLASS(CutNodeList)


BEGIN_CUTLEADER_NAMESPACE()


// 把桥接工序转化为切割节点。
class BridgeSequenceCutter : public SequenceCutterBase
{
public:
	BridgeSequenceCutter(BridgeSequencePtr pBridgeSeq);
	~BridgeSequenceCutter(void);

public:
	// 把“桥接工序”转化为“切割节点”集合。
	// 注:
	//   1) this function also generate the trace line from "preUnPiercePt" to this loop inst.
	/* params:
	*  bAvoidToNext: whether we need avoid from this sequence to next sequence.
	*/
	CutNodeListPtr BdgeSeq2CutNodes(Point2D preUnPiercePt, LoopInstanceListPtr pCutLoopInstList, BOOL bAvoidToNext = FALSE);

private:
	/************************************************************************/
	// 关于处理“根桥接节点”。

	// 把“根桥接节点”转化成“切割节点”。
	// 注：
	//   1) 这是个递归函数。
	void RootBdgeNode2CutNodes(BridgeNodePtr pBridgeNode, CutNodeListPtr pCutNodeList);

	// 把“根桥接节点”对应的“回路实例”按照切割方向在“出口桥接点”处分段，生成每段的“切割节点”。 
	/* 参数：
	*  sortChildNodes: 孩子“桥接节点”的索引，按照切割方向排序的。
	*/
	// 注：
	//   1) 第一段的“切割节点”是从“回路实例”的切入点开始的。
	//   2) 需要根据桥的宽度对“切割节点”做调整。
	std::vector<CutNodeListPtr> SplitRootBdgeNode(BridgeNodePtr pRootBdgeNode, std::vector<int>& sortChildNodes);
	/************************************************************************/


	/************************************************************************/
	// 关于处理“子桥接节点”。

	// 把“子桥接节点”转化成“切割节点”。
	/* 参数：
	*  parentOutBdgePt: “父亲桥接节点”对应的“出口桥接点”坐标。
	*/
	// 注：
	//   1) 这是个递归函数。
	void SubBdgeNode2CutNodes(const Point2D& parentOutBdgePt, BridgeNodePtr pBridgeNode, CutNodeListPtr pCutNodeList);

	// 把“子桥接节点”对应的“回路实例”按照切割方向在“出口桥接点”处分段，生成每段的“切割节点”。 
	/* 参数：
	*  pInBdgePts: 该“子桥接节点”的“入口桥接点”的2个“桥点”坐标。
	*  sortChildNodes: 孩子“桥接节点”的索引，按照切割方向排序的。
	*/
	// 注：
	//   1) 第一段的“切割节点”是从“入口桥接点”开始的。
	//   2) 需要根据桥的宽度对“切割节点”做调整。
	std::vector<CutNodeListPtr> SplitChildBdgeNode(Point2DListPtr pInBdgePts, BridgeNodePtr pChildBdgeNode, std::vector<int>& sortChildNodes);
	/************************************************************************/


	// 把“回路实例”的“切割节点”按照切割方向在“出口桥接点”处分段，生成每段的“切割节点”。 
	/* 参数：
	*  sortChildNodes: 孩子“桥接节点”的索引，按照切割方向排序的。
	*/
	std::vector<CutNodeListPtr> SplitCutNodes(CutNodeListPtr pCutNodeList, Point2DListPtr pOutBdgePtPos, std::vector<int>& sortChildNodes);

	// 为了“桥宽度”调整每个“切割节点段”。
	/* 参数：
	*  cutNodeSegments: 遵循切割方向的“切割节点段”，它们是在“出口桥接点”分的段，没有考虑桥的宽度。
	*  bdgePtPosList: 每个“出口桥接点”的2个“桥点”坐标。
	*/
	// 注：
	//   1) 因为桥有宽度，所以这些“切割节点段”的头部或者尾部需要延长/缩回一部分。
	//   2) 需要过滤掉很短的切割节点。
	void AdjustCutNodeSegment(std::vector<CutNodeListPtr> cutNodeSegments, std::vector<Point2DListPtr> bdgePtPosList);

	// 得到“切割节点段”的起点和终点。
	Point2D GetCutNodeSegStartPt(CutNodeListPtr pCutNodeList);
	Point2D GetCutNodeSegEndPt(CutNodeListPtr pCutNodeList);

private:
	BridgeSequencePtr m_pBridgeSeq;
};

END_CUTLEADER_NAMESPACE()
