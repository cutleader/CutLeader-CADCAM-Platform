#pragma once

#include "clCutSequenceCommon.h"
#include "Point2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(SequenceConfigItem)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItem)
DECLARE_CUTLEADER_CLASS(PartInstanceTopologyItemList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(AvoidData)
DECLARE_CUTLEADER_CLASS(ComCutGroupList)
DECLARE_CUTLEADER_CLASS(ComCutGroup)
DECLARE_CUTLEADER_CLASS(CommonCutSequence)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(IEdge)
DECLARE_CUTLEADER_CLASS(EdgeLoop)
DECLARE_CUTLEADER_CLASS(EdgeLoopList)
DECLARE_CUTLEADER_CLASS(ConfigData)
DECLARE_CUTLEADER_CLASS(MicroJointDatasOfComCutGroup)

BEGIN_CUTLEADER_NAMESPACE()

// �����߹��򡱹�������
class ClCutSequence_Export CommonCutSequenceManager
{
public:
	// �ҵ�һ��������˲��еġ������顱��
	// ע��
	//  1) �������顱�����ʵ�������������治������������
	static ComCutGroupListPtr FindComCutGroups(PartInstanceListPtr pPartInstances, LoopInstanceListPtr pSeqLoopInsts, const SequenceConfigItem* pSeqConfigItem, AvoidDataPtr pAvoidData);

	// ���ɡ����߹��򡱡�
	static CommonCutSequencePtr GenerateCommonCutSequence(Point2D& lastCutPt, const PartInstanceTopologyItemList* pPartTopItems, const PartInstanceTopologyItem* pPartTopItem, ComCutGroupPtr pComCutGroup,
									 EdgeLoopListPtr pEdgeLoops, LoopInstanceListPtr pSeqLoopInsts, const std::map<LONGLONG, LineArc2DListPtr>& outerLoopGeom, const ConfigData* pConfigData);

	// ƫ�ơ����߹��򡱡�
	static CommonCutSequencePtr ProcessOffset(CommonCutSequencePtr pComCutSequenceUnit, ComCutGroupPtr pComCutGroup);

	// check whether comcut sequence is invalid.
	/* ������
	*  pValidGroups: ������Ч�Ĺ����顣
	*/
	// ע��
	//  1) Ҫ�ǹ��߹������ڵ����ʵ��û���˻��߹�������Ч�ˣ����߹���Ҳ����Ч�ˡ�
	static BOOL IsComcutInvalid(CommonCutSequencePtr pComCutSequence, PartInstanceListPtr pAllPartInsts);

	// �����µġ����ʵ�������¹����е�һЩ���ݡ�
	// ע��
	//  1) ���¹��߹����е����ʵ��ID��
	static void UpdateComcutSeq(CommonCutSequencePtr pComCutSequence, PartInstanceListPtr pOldPartInsts, PartInstanceListPtr pNewPartInsts);

	// ���㡰�߻�·������ʼ�и�ߡ�
	// ע��
	//  1) ����ǰȷ��������߻�·�������б߱��й�������ֻ�ǹ��У���
	static IEdgePtr CalcStartEdge(const Point2D& lastCutPt, EdgeLoopPtr pEdgeLoop);

    // �õ�һ�������������΢������Ϣ��
    static MicroJointDatasOfComCutGroupPtr GetMicroJointDatas_of_comCutGroup(ComCutGroupPtr pComCutGroup);
};

END_CUTLEADER_NAMESPACE()
