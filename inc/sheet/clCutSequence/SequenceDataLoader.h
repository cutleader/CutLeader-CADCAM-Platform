#pragma once

#include "clCutSequenceCommon.h"
#include <map>

DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(Point2DList)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(LoopSequence)
DECLARE_CUTLEADER_CLASS(GridLoopSequence)
DECLARE_CUTLEADER_CLASS(CommonCutSequence)
DECLARE_CUTLEADER_CLASS(BridgeNode)
DECLARE_CUTLEADER_CLASS(BridgeSequence)
DECLARE_CUTLEADER_CLASS(ChainCutSequence)
DECLARE_CUTLEADER_CLASS(GridCutSequence)
DECLARE_CUTLEADER_CLASS(FlyCutSequence)
DECLARE_CUTLEADER_CLASS(ComCutGroupList)
DECLARE_CUTLEADER_CLASS(RemnantLineSeq)

BEGIN_CUTLEADER_NAMESPACE()

// use this class to save sequence unit data into database.
class ClCutSequence_Export SequenceDataLoader
{
public:
	// load sequence units of the sheet.
	/* params:
	*  pAllPartPmt: all part pmts.
	*  pAllPartInst: all part insts.
	*/
	// notes:
	//   1) after loading sequence, cache data in loop inst will be updated.
	static CutSequenceListPtr LoadSeqData(DBConnectPtr pDBConnect, PartPlacementListPtr pAllPartPmt, PartInstanceListPtr pAllPartInst);

private:
	/************************************************************************/
	// about loading loop sequence.

	// load loop sequence.
	/* params:
	*  pPartInstances: all loaded part inst list.
	*/
	// notes:
	//   1) we should check the loop inst exist, if the loop inst lost, return NULL.
	static LoopSequencePtr LoadLoopSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, PartInstanceListPtr pPartInstances);
	static std::map<LONGLONG, LoopSequencePtr> LoadAllLoopSeq(DBConnectPtr pDBConnect, PartInstanceListPtr pPartInstances);

	// load loop grid sequence.
	/* params:
	*  pPartInstances: all loaded part inst list.
	*/
	// notes:
	//   1) we should check the loop inst exist, if the loop inst lost, return NULL.
	static GridLoopSequencePtr LoadLoopGridSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, PartInstanceListPtr pPartInstances);
	/************************************************************************/

	/************************************************************************/
	// ���ڹ��߹���

	// ���ع��߹���
	static CommonCutSequencePtr LoadComCutSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, PartInstanceListPtr pAllPartInst, ComCutGroupListPtr pAllComCutGroup);

	// �������й����顣
	// ע��
	//  1) ����������е����ʵ����û���ˣ�����������ʧЧ�ˡ�
	//  2) �Ѷ�ʧ�����ʵ���ӹ�������ɾ����
	static ComCutGroupListPtr LoadAllComCutGroup(DBConnectPtr pDBConnect, PartInstanceListPtr pAllPartInst);
	/************************************************************************/

	// �����Žӹ���
	// ע��
	//  1) ����ʱ��Ҫ��֤������Ч�ԡ�
	static BridgeSequencePtr LoadBdgeCutSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, PartInstanceListPtr pAllPartInst);
	static BOOL LoadSubBdgeNode(DBConnectPtr pDBConnect, BridgeNodePtr pBridgeNode, PartInstanceListPtr pAllPartInst);

	// ���������
	static ChainCutSequencePtr LoadChainSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, PartInstanceListPtr pAllPartInst);

	// �������ֹ���
	static GridCutSequencePtr LoadGridSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, PartInstanceListPtr pAllPartInst);

	// ���ط����и��
	static FlyCutSequencePtr LoadFlyCutSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, PartInstanceListPtr pAllPartInst);

	// load the trace points.
	static Point2DListPtr LoadTracePt(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID);
	static std::map<LONGLONG, Point2DListPtr> LoadAllTracePt(DBConnectPtr pDBConnect);

	// ���ء������и��ߡ�
	static RemnantLineSeqPtr LoadRemnantLineSeq(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID);
};

END_CUTLEADER_NAMESPACE()
