#pragma once

#include "clPartLayoutCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(LoopStartCutDataList)

BEGIN_CUTLEADER_NAMESPACE()

// this command can change the cut direction of loops.
class ClPartLayout_Export CutDirectionsChangeCommand : public CommandBase
{
public:
	CutDirectionsChangeCommand(EditorData editorData, LoopStartCutDataListPtr pLoopStaEndCutList);
	virtual ~CutDirectionsChangeCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	LoopStartCutDataListPtr m_pLoopStaEndCutList;
};

END_CUTLEADER_NAMESPACE()
