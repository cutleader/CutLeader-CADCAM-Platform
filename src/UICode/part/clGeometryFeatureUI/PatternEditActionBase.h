#pragma once

#include "IPatternEditAction.h"
#include "ActionBase.h"
#include "IEditorInfo.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(IPattern)
DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PatternLoopList)

BEGIN_CUTLEADER_NAMESPACE()

// put some shared data/function of pattern-related action in this class.
// notes:
//   1) some rules:
//      a) under select action, we can escape to dummy action.
//      b) under other actions, we can escape to select action.
class PatternEditActionBase : virtual public IPatternEditAction,
						  public ActionBase,
						  public IEditorInfo
{
public:
	PatternEditActionBase(void);
	virtual ~PatternEditActionBase(void);

public: // implement interface IEditorInfo.
	virtual EditorData GetEditorData() const;

public: // implement IInteract interface.
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implements IPatternEditAction interface.
	virtual BOOL SupportDrawAssit() { return m_bSupportDrawAssit; }

protected:
	// enter the selecting pattern action and display the specified patterns.
	void EnterPatSelAction(IPatternPtr pSelPat);
	void EnterPatSelAction(PatternListPtr pSelPatList);

protected:
	// pattern list of the part.
	PatternListPtr m_pPatList;

	// the pattern loops of the part.
	PatternLoopListPtr m_pPatternLoopList;

	// whether support draw assistance.
	// notes:
	//   1) in some action, we will switch this status. e.g., when move pattern, disable draw assistance
	//      when select the pattern, enable this when move.
	BOOL m_bSupportDrawAssit;
};

END_CUTLEADER_NAMESPACE()
