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

// ���и�򡱹�������
class ClCutSequence_Export CutSequenceManager
{
public:
	// �����ʵ�����乤��
	/* ����:
	*  partRectMap: ���ʵ���и�����·ʵ������Ӿ��Ρ�
	*  pPartInstTopItems, partTopMap: the topology of the part inst in the sheet.
	*/
	static void AutoSequence(PartPlacementListPtr pPartPmts_offset, PartInstanceListPtr pPartInsts_offset,
							 CutSequenceListPtr pAllCutSeqs, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
							 const PartInstanceTopologyItemList* pPartTopItems, const std::map<LONGLONG, PartInstanceTopologyItemPtr>& partTopMap,
							 const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);

	// ɾ��һ������
	static void DeleteCutSeq(CutSequenceListPtr pCutSequences, const ICutSequence* pSeqUnit, const IMaterialSize* pMaterialSize, const SequenceConfigItem* pSeqConfigItem);

	// ��ָ��λ��ǰ����һ�����и�򡱡�
	/* ����:
	*  iIndex: Ϊ-1�Ļ����ŵ�����档
	*/
	static void InsertCutSeq(CutSequenceListPtr pCutSequences, int iIndex, ICutSequencePtr pCutSequence, const IMaterialSize* pMaterialSize, const SequenceConfigItem* pSeqConfigItem);

	// �ѹ����ƶ���ָ����λ��ǰ�档
	/* ����:
	*  iNewLoc: Ϊ-1�Ļ����Ƶ�����档
	*/
	static void MoveCutSeq(CutSequenceListPtr pCutSequences, int iNewLoc, ICutSequencePtr pCutSequence, PartInstanceListPtr pAllPartInst, const IMaterialSize* pMaterialSize, const SequenceConfigItem* pSeqConfigItem);

	// delete the invalid sequences.
	static void DeleteInvalidSeq(CutSequenceListPtr pCutSequences, PartPlacementListPtr pAllPartPmts, PartInstanceListPtr pAllPartInst);
    // �õ���Ч���и��
    static CutSequenceListPtr GetInvalidSequences(CutSequenceListPtr pCutSequences, PartPlacementListPtr pAllPartPmts, PartInstanceListPtr pAllPartInst);

	// update the cache data in sequence.
	static void UpdateCache(CutSequenceListPtr pCutSequences);

	// �������ƫ�ơ�
	// ע��
	//   1) ������������µĹ������Ǻ��ϵ��Ƕ������ڴ棬����ͼ�α�ƫ�ƹ����滻�ˣ���������һ����
	//   2) �µġ����߹����б���ļ�������Ҫ��Ϊ0.
	static CutSequenceListPtr ProcessOffset(const CutSequenceList* pCutSequences, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances);

private:
	// ����������˽ṹ�����ʵ�����乤��
	/* ����:
	*  lastCutPt: ���һ���г��㣬���������и���á�
	*  pPrePartInst: ǰһ���ӹ��ġ����ʵ������
    *  pOldCutSeqs: ������֮ǰ�Ĺ���
    *  pNewCutSequences: �����ɵĹ���
	*/
    // ע��
    //  1) �ڴ�����Ƕ���ʱ������1�ǰ�һ�������������Ƕ�����ȫ���ӹ�����Ȼ����ȥ�и������������2���е�һ���ڿ׵���Ƕ���������ȥ�ӹ�����ڿס�
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

	// �õ������ʵ���������еõ�һ����������ȡ���
	/* ����:
	*  pPartInstances: ��ǰ���˲���δ�ӹ��ġ����ʵ������
	*  pPrePartInst: ǰһ���ӹ��ġ����ʵ������
	*  partRectMap: ���ʵ���и�����·ʵ������Ӿ��Ρ�
	*  partRect: ��һ���˲��������Χ���Ρ�
	*  startCorner: ���ĸ���ʼ�ǿ�ʼ�����ȡ�
	*  iDir: ������ȵ����췽��
	*/
	// ע��
	//  1) �����е�ע���ԡ����½�/X���򡱷�ʽΪ����
	static PartInstanceListPtr GetStrip(PartInstanceListPtr pPartInstances, PartInstancePtr pPrePartInst, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
									Rect2D partRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

	// �õ������е�һ���ӹ��ġ����ʵ������
	/* ������
	*  partRect: ��һ���˲��������Χ���Ρ�
	*  pStripPartInsts: ���������е����ʵ����
	*  iStartCorner: ������ȵ����нǡ�
	*/
	// ע��
	//  1) �õ���һ���ӹ��ġ����ʵ�����󣬰�����������ɾ����
	static PartInstancePtr GetFstStripPart(PartInstanceListPtr pStripPartInsts, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
									       Rect2D partRect, RECT_CORNER iStartCorner);

	// �õ���������һ���ӹ��ġ����ʵ������
	/* ������
	*  pStripPartInsts: ����������δ�ӹ��ġ����ʵ������
	*  pPrePartInst: ��������һ�����ӹ��ġ����ʵ������
	*  partRect: ��һ���˲��������Χ���Ρ�
	*  iStartCorner: ������ȵ����нǣ������ʼ�ǿ��ܻᱻ�ı䡣
	*  iDir: ������ȵ����췽��
	*/
	// ע��
	//  1) �����е�ע���ԡ����½�/X���򡱷�ʽΪ����
	//  2) ����һ�������ʵ������Ӿ��ε����ĵ��һ��ˮƽ�ߣ����������ߺ�������������ġ����ʵ�������Ѽ�������
	//     ��Щ���ʵ���о��������½������Ϊ��һ�������ʵ������������ˮƽ��ʲôҲû��������ӡ�pStripPartInsts�������ң���ʱ��Ҫע��Ҫ��Ҫ�ı����ȵĿ�ʼ�ǡ�
	//  3) �õ���һ���ӹ��ġ����ʵ�����󣬰�����������ɾ����
	static PartInstancePtr GetNextStripPart(PartInstanceListPtr pStripPartInsts, PartInstancePtr pPrePartInst, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
											Rect2D partRect, RECT_CORNER& iStartCorner, XY_DIRECTION iDir);

	// �ӡ�basePt����һ�����޳���ˮƽ/��ֱ�ߣ�ȡ����iDir�����������startCorner����ɨȥ����������ɨ�������ʵ����
	static PartInstanceListPtr GetSweepedParts(PartInstanceListPtr pPartInstances, const std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG,Rect2D>>>& partRectMap,
										   Point2D basePt, RECT_CORNER startCorner, XY_DIRECTION iDir);

	// �ı����ȵ���ʼ�ǡ�
	// ע��
	//   1) �������ȷ�ʽ���乤��ʱ�ǰ���S�����еģ�������һ�����ȵ����췽���ǰһ���෴�ġ�
	static RECT_CORNER ChgStripStartCorner(RECT_CORNER startCorner, XY_DIRECTION iDir);
};

END_CUTLEADER_NAMESPACE()
