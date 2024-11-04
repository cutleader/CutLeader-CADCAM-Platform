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


// ���Žӹ���ת��Ϊ�и�ڵ㡣
class BridgeSequenceCutter : public SequenceCutterBase
{
public:
	BridgeSequenceCutter(BridgeSequencePtr pBridgeSeq);
	~BridgeSequenceCutter(void);

public:
	// �ѡ��Žӹ���ת��Ϊ���и�ڵ㡱���ϡ�
	// ע:
	//   1) this function also generate the trace line from "preUnPiercePt" to this loop inst.
	/* params:
	*  bAvoidToNext: whether we need avoid from this sequence to next sequence.
	*/
	CutNodeListPtr BdgeSeq2CutNodes(Point2D preUnPiercePt, LoopInstanceListPtr pCutLoopInstList, BOOL bAvoidToNext = FALSE);

private:
	/************************************************************************/
	// ���ڴ������Žӽڵ㡱��

	// �ѡ����Žӽڵ㡱ת���ɡ��и�ڵ㡱��
	// ע��
	//   1) ���Ǹ��ݹ麯����
	void RootBdgeNode2CutNodes(BridgeNodePtr pBridgeNode, CutNodeListPtr pCutNodeList);

	// �ѡ����Žӽڵ㡱��Ӧ�ġ���·ʵ���������и���ڡ������Žӵ㡱���ֶΣ�����ÿ�εġ��и�ڵ㡱�� 
	/* ������
	*  sortChildNodes: ���ӡ��Žӽڵ㡱�������������и������ġ�
	*/
	// ע��
	//   1) ��һ�εġ��и�ڵ㡱�Ǵӡ���·ʵ����������㿪ʼ�ġ�
	//   2) ��Ҫ�����ŵĿ�ȶԡ��и�ڵ㡱��������
	std::vector<CutNodeListPtr> SplitRootBdgeNode(BridgeNodePtr pRootBdgeNode, std::vector<int>& sortChildNodes);
	/************************************************************************/


	/************************************************************************/
	// ���ڴ������Žӽڵ㡱��

	// �ѡ����Žӽڵ㡱ת���ɡ��и�ڵ㡱��
	/* ������
	*  parentOutBdgePt: �������Žӽڵ㡱��Ӧ�ġ������Žӵ㡱���ꡣ
	*/
	// ע��
	//   1) ���Ǹ��ݹ麯����
	void SubBdgeNode2CutNodes(const Point2D& parentOutBdgePt, BridgeNodePtr pBridgeNode, CutNodeListPtr pCutNodeList);

	// �ѡ����Žӽڵ㡱��Ӧ�ġ���·ʵ���������и���ڡ������Žӵ㡱���ֶΣ�����ÿ�εġ��и�ڵ㡱�� 
	/* ������
	*  pInBdgePts: �á����Žӽڵ㡱�ġ�����Žӵ㡱��2�����ŵ㡱���ꡣ
	*  sortChildNodes: ���ӡ��Žӽڵ㡱�������������и������ġ�
	*/
	// ע��
	//   1) ��һ�εġ��и�ڵ㡱�Ǵӡ�����Žӵ㡱��ʼ�ġ�
	//   2) ��Ҫ�����ŵĿ�ȶԡ��и�ڵ㡱��������
	std::vector<CutNodeListPtr> SplitChildBdgeNode(Point2DListPtr pInBdgePts, BridgeNodePtr pChildBdgeNode, std::vector<int>& sortChildNodes);
	/************************************************************************/


	// �ѡ���·ʵ�����ġ��и�ڵ㡱�����и���ڡ������Žӵ㡱���ֶΣ�����ÿ�εġ��и�ڵ㡱�� 
	/* ������
	*  sortChildNodes: ���ӡ��Žӽڵ㡱�������������и������ġ�
	*/
	std::vector<CutNodeListPtr> SplitCutNodes(CutNodeListPtr pCutNodeList, Point2DListPtr pOutBdgePtPos, std::vector<int>& sortChildNodes);

	// Ϊ�ˡ��ſ�ȡ�����ÿ�����и�ڵ�Ρ���
	/* ������
	*  cutNodeSegments: ��ѭ�и��ġ��и�ڵ�Ρ����������ڡ������Žӵ㡱�ֵĶΣ�û�п����ŵĿ�ȡ�
	*  bdgePtPosList: ÿ���������Žӵ㡱��2�����ŵ㡱���ꡣ
	*/
	// ע��
	//   1) ��Ϊ���п�ȣ�������Щ���и�ڵ�Ρ���ͷ������β����Ҫ�ӳ�/����һ���֡�
	//   2) ��Ҫ���˵��̵ܶ��и�ڵ㡣
	void AdjustCutNodeSegment(std::vector<CutNodeListPtr> cutNodeSegments, std::vector<Point2DListPtr> bdgePtPosList);

	// �õ����и�ڵ�Ρ��������յ㡣
	Point2D GetCutNodeSegStartPt(CutNodeListPtr pCutNodeList);
	Point2D GetCutNodeSegEndPt(CutNodeListPtr pCutNodeList);

private:
	BridgeSequencePtr m_pBridgeSeq;
};

END_CUTLEADER_NAMESPACE()
