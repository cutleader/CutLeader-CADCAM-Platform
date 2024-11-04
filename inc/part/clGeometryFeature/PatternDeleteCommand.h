#pragma once

#include "clGeometryFeatureCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PatternLoopList)

BEGIN_CUTLEADER_NAMESPACE()

// the command will delete patterns from the pattern list.
class ClGeometryFeature_Export PatternDeleteCommand : public CommandBase
{
public:
	PatternDeleteCommand(EditorData editorData, PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, PatternListPtr pSelectedPat);
	~PatternDeleteCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	PatternListPtr m_pPatList;
	PatternLoopListPtr m_pPatternLoopList;

	// patterns that should be deleted.
	PatternListPtr m_pSelectedPat;
};

END_CUTLEADER_NAMESPACE()