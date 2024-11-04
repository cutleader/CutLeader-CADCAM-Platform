#pragma once

#include "SequenceCommandBase.h"
#include <vector>
#include <map>

DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// 创建“飞行切割工序”的命令。
class ClCutSequence_Export ReplaceAsFlyCutCommand : public SequenceCommandBase
{
	// "替换信息"
	struct Seq_replace_info
	{
		ICutSequencePtr m_pNewFlyCutSeq; // 新的飞行切割工序
		std::map<int, ICutSequencePtr> m_pOldCutSequences; // 旧的工序
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
	bool m_bApplyTo_other_sameParts; // 是否应用到其他相同零件。

	// do产生的中间数据，undo/redo时要用到。
	std::vector<Seq_replace_info> m_seqReplaceData;
};

END_CUTLEADER_NAMESPACE()
