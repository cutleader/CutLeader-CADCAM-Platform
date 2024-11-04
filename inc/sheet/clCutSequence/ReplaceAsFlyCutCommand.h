#pragma once

#include "SequenceCommandBase.h"
#include <vector>
#include <map>

DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// �����������и�򡱵����
class ClCutSequence_Export ReplaceAsFlyCutCommand : public SequenceCommandBase
{
	// "�滻��Ϣ"
	struct Seq_replace_info
	{
		ICutSequencePtr m_pNewFlyCutSeq; // �µķ����и��
		std::map<int, ICutSequencePtr> m_pOldCutSequences; // �ɵĹ���
	};

public:
	ReplaceAsFlyCutCommand(EditorData editorData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, PartInstanceListPtr pAllPartInstances,
					LoopInstanceListPtr pLoopInsts_4_flyCut, bool bApplyTo_other_sameParts);
	~ReplaceAsFlyCutCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	PartInstanceListPtr m_pAllPartInsts;
	LoopInstanceListPtr m_pLoopInsts_4_flyCut;
	bool m_bApplyTo_other_sameParts; // �Ƿ�Ӧ�õ�������ͬ�����

	// do�������м����ݣ�undo/redoʱҪ�õ���
	std::vector<Seq_replace_info> m_seqReplaceData;
};

END_CUTLEADER_NAMESPACE()
