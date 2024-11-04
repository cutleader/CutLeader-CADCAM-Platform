#pragma once

#include "clGeometryFeatureCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(PatternList)

BEGIN_CUTLEADER_NAMESPACE()

// this command will transform the patterns.
// notes:
//   1) we call the patterns which are from part as "original" patterns.
class ClGeometryFeature_Export PatternTransformCommand : public CommandBase
{
public:
	/* params:
	*  pNewPats: the tmp new patterns.
	*  pOldPats: the "original" patterns.
	*/
	PatternTransformCommand(EditorData editorData, CString strName, PatternLoopListPtr pPatternLoopList, PatternListPtr pNewPats, PatternListPtr pOldPats);
	~PatternTransformCommand(void);

public:
	// notes:
	//   1) after call this:
	//      a) m_pNewPats: the transformed patterns and "original".
	//      b) m_pOldPats: the not transformed patterns and tmp.
	virtual void Do();

	// notes:
	//   1) after call this:
	//      a) m_pNewPats: the transformed patterns and tmp.
	//      b) m_pOldPats: the not transformed patterns and "original".
	virtual void UnDo();

private:
	PatternLoopListPtr m_pPatternLoopList;

	// always be the patterns which have been transformed.
	PatternListPtr m_pNewPats;

	// always be the patterns which have not been transformed.
	PatternListPtr m_pOldPats;
};

END_CUTLEADER_NAMESPACE()
