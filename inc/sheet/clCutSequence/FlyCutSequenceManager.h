#pragma once

#include "clCutSequenceCommon.h"
#include "Point2D.h"
#include "Rect2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(ConfigData)
DECLARE_CUTLEADER_CLASS(ICutSequence)
DECLARE_CUTLEADER_CLASS(FlyCutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// �������и�򡱹�������
class ClCutSequence_Export FlyCutSequenceManager
{
public:
	// �ж���Щ��·ʵ���ܷ񴴽������и��
	static bool CheckLoopInsts_4_flyCutSeq(ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, LoopInstanceListPtr pCandidate_LoopInsts);

	// ��������и�������λ�ã���ʱ������δ�������ο��˱��滻�ĵ�һ����������е㡣
	static RECT_CORNER CalcStartCornerOfFlyCutSeq(const std::map<int, ICutSequencePtr>& cutSeqs_willBeReplaced, ConfigDataPtr pConfigData, LoopInstanceListPtr pCandidate_LoopInsts);

	// ƫ�ơ������и�򡱡�
	static FlyCutSequencePtr ProcessOffset(FlyCutSequencePtr pFlyCutSeq, PartInstanceListPtr pPartInsts_after_offset);

	// �ж�������й����ǲ�����Ч��
	static BOOL IsFlyCutSeqInvalid(FlyCutSequencePtr pFlyCutSeq, PartInstanceListPtr pAllPartInstances);

	// �����µġ����ʵ�������ϸ��¡������и���е�һЩ���ݡ�
	static void UpdateFlyCutSequence(FlyCutSequencePtr pFlyCutSeq, PartInstanceListPtr pOldPartInsts, PartInstanceListPtr pNewPartInsts);
};

END_CUTLEADER_NAMESPACE()
