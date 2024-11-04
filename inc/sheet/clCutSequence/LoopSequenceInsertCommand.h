#pragma once

#include "SequenceCommandBase.h"

DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// °∞≤Â»Îπ§–Ú°±√¸¡Ó°£
class ClCutSequence_Export LoopSequenceInsertCommand : public SequenceCommandBase
{
public:
	LoopSequenceInsertCommand(EditorData editorData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, ICutSequencePtr pBaseSeqUnit, ICutSequencePtr pNewSeqUnit);
	~LoopSequenceInsertCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	// where the new sequence unit insert.
	ICutSequencePtr m_pBaseSeqUnit;

	// the added sequence unit.
	ICutSequencePtr m_pNewSeqUnit;
};

END_CUTLEADER_NAMESPACE()
