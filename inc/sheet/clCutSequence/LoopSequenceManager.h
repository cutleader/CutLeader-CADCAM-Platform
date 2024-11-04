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

// ����·���򡱹�������
class ClCutSequence_Export LoopSequenceManager
{
public:
	// �������ʵ�����еġ���·ʵ�������乤��
	/* ����:
	*  lastCutPt: ���һ���г��㣬���������и���á�
	*  pPreLoopInst: ǰһ���ӹ��Ļ�·ʵ����
	*  partInstRect: ������ʵ������Χ���Σ���Ѱ������ʱ�õ���
	*  pPartInstTopItems: ��ǰ���˲㡣
	*  pPartInstTopItem: ��ǰ�����
	*  pLoopInstances: Ҫ���乤��ġ���·ʵ��������������ͬһ�������ʵ������
	*  outerLoopGeom: ��������С����ʵ������Χ��·�Ļ���ͼ�Ρ�
	*/
	static CutSequenceListPtr GenLoopSeq(Point2D& lastCutPt, LoopInstancePtr pPreLoopInst, const Rect2D& partInstRect, const PartInstanceTopologyItemList* pPartInstTopItems, const PartInstanceTopologyItem* pPartInstTopItem,
									LoopInstanceListPtr pLoopInstances, LoopInstanceListPtr pSeqLoopInsts, const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);

	// �����ʵ�����ڲ�����·ʵ�������乤��
	// ע��
	//  1) �Ե�ǰ���Ϊһ����Χ�����乤�򣬼�������������������еĹ��򣬵�������Ҫ���´�����ù�ϵ��
	static CutSequenceListPtr GenLoopSeqs_4_innerLoops(CutSequenceListPtr pCutSequences, PartInstancePtr pPartInstance, PartInstanceListPtr pPartInstances, const ConfigData* pConfigData);

	// check whether loop sequence is invalid.
	// notes:
	//   1) if cannot find the loop inst in "pAllPartInst", the sequence is invalid.
	static BOOL IsLoopSeqInvalid(LoopSequencePtr pLoopSequence, PartInstanceListPtr pAllPartInst);

	// ƫ�ơ���·���򡱡�
	// ע��
	//   1) ��������á�����ƫ��֮�󣬡���·����Ҳ��Ҫ��ƫ�ơ�
	static LoopSequencePtr ProcessOffset(LoopSequencePtr pLoopSequence, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances);

	// �����µġ����ʵ�������ϸ��¹����е�һЩ���ݡ�
	/* ������
	*  pOldPartInsts: �ɵ����ʵ�����ϡ�
	*  pNewPartInsts: ��¡���µ����ʵ�����ϡ�
	*/
	// ע��
	//  1) ��Ҫ�滻������·ʵ�����еĻ�·ID�͡����ʵ������
	static void UpdateLoopSeq(LoopSequencePtr pLoopSequence, PartInstanceListPtr pOldPartInsts, PartInstanceListPtr pNewPartInsts);

    // ����·ʵ�����乤��
    // ע��
    //  1) ���������Ҫ���������г���ͼӹ����Ļ�·���ϡ�
    static LoopSequencePtr BuildLoopSeq(Point2D& lastHitPt, const PartInstanceTopologyItemList* pPartInstTopItems, const PartInstanceTopologyItem* pPartInstTopItem, LoopInstancePtr pLoopInstance, 
        LoopInstanceListPtr pSeqLoopInsts, const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, AvoidDataPtr pAvoidData, const ConfigData* pConfigData);

    	// �ӡ���·ʵ���������еõ�һ������·���ȡ���
	/* ����:
	*  pLoopInstances: δ�ӹ��ġ���·ʵ������
	*  pPreLoopInst: ǰһ���ӹ��ġ���·ʵ������
	*  startCorner: ���ĸ���ʼ�ǿ�ʼ�����ȡ�
	*  iDir: ������ȵ����췽��
	*/
	// ע��
	//  1) �����е�ע���ԡ����½�/X���򡱷�ʽΪ����
	static LoopInstanceListPtr GetStrip(LoopInstanceListPtr pLoopInstances, LoopInstancePtr pPreLoopInst, Rect2D partInstRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

	// �õ������е�һ���ӹ��ġ���·ʵ������
	/* ������
	*  pStripLoopInsts: ���������еĻ�·ʵ����
	*  iStartCorner: ������ȵ����нǡ�
	*/
	// ע��
	//  1) �õ���һ���ӹ��ġ���·ʵ�����󣬰�����������ɾ����
	static LoopInstancePtr GetFstStripLoop(LoopInstanceListPtr pStripLoopInsts, Rect2D partInstRect, RECT_CORNER iStartCorner);

	// �õ���������һ���ӹ��ġ���·ʵ������
	/* ������
	*  pStripPartInsts: ����������δ�ӹ��ġ���·ʵ������
	*  pPreLoopInst: ��������һ�����ӹ��ġ���·ʵ������
	*  iStartCorner: ������ȵ����нǣ������ʼ�ǿ��ܻᱻ�ı䡣
	*  iDir: ������ȵ����췽��
	*/
	// ע��
	//  1) �����е�ע���ԡ����½�/X���򡱷�ʽΪ����
	//  2) ����һ������·ʵ������Ӿ��ε����ĵ��һ��ˮƽ�ߣ����������ߺ�������������ġ���·ʵ�������Ѽ�������
	//     ��Щ��·ʵ���о��������½������Ϊ��һ������·ʵ������������ˮƽ��ʲôҲû��������ӡ�pStripLoopInsts�������ң���ʱ��Ҫע��Ҫ��Ҫ�ı����ȵĿ�ʼ�ǡ�
	//  3) �õ���һ���ӹ��ġ���·ʵ�����󣬰�����������ɾ����
	static LoopInstancePtr GetNextStripLoop(LoopInstanceListPtr pStripLoopInsts, LoopInstancePtr pPreLoopInst, Rect2D partInstRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

private:
	// when use cut avoidance, check whether need to disable lead out of the loop sequence.
	static void EnableLeadOut(LoopSequencePtr pLoopSequence);

	// �ӡ�basePt����һ�����޳���ˮƽ/��ֱ�ߣ�ȡ����iDir�����������startCorner����ɨȥ����������ɨ���Ļ�·ʵ����
	static LoopInstanceListPtr GetSweepedLoops(LoopInstanceListPtr pLoopInstList, Point2D basePt, RECT_CORNER startCorner, XY_DIRECTION iDir);

	// �ı����ȵ���ʼ�ǡ�
	// ע��
	//   1) �������ȷ�ʽ���乤��ʱ�ǰ���S�����еģ�������һ�����ȵ����췽���ǰһ���෴�ġ�
	static RECT_CORNER ChgStripStartCorner(RECT_CORNER startCorner, XY_DIRECTION iDir);
};

END_CUTLEADER_NAMESPACE()
