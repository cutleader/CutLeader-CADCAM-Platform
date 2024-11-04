#pragma once

#include "clGeometryFeatureCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PatternLoopList)

BEGIN_CUTLEADER_NAMESPACE()

// the command that copy pattern(s).
class ClGeometryFeature_Export PatternCopyCommand : public CommandBase
{
public:
	PatternCopyCommand(EditorData editorData, PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, PatternListPtr pNewPats);
	~PatternCopyCommand(void);

public: // implements ICommand interface.
	virtual void Do();
	virtual void UnDo();

private:
	PatternListPtr m_pPatList;
	PatternLoopListPtr m_pPatternLoopList;
	
	// the patterns that will be copied(added) to m_pPatList.
	PatternListPtr m_pNewPats;
};

END_CUTLEADER_NAMESPACE()
