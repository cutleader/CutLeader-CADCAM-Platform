#pragma once

#include "PatternPreSelectAction.h"

DECLARE_CUTLEADER_CLASS(PatternDeleteAction)

BEGIN_CUTLEADER_NAMESPACE()

// this action can delete the pattern.
// notes:
//   1) if patterns have been selected before enter this action, delete selected
//      patterns and set PatternSelectAction as the next action.
class PatternDeleteAction : public PatternPreSelectAction
{
public:
	PatternDeleteAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort, PatternListPtr pSelPats);
	~PatternDeleteAction(void);

public: // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL RButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);

	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

public:  // implement IAction interface.
	BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_DELETE; }
	virtual void UpdateForUndoRedo();

public:
	// delete selected patterns.
	// notes:
	//   1) if no pattern selected, do nothing.
	//   2) if patterns selected, delete them and set PatternSelectAction as the next action.
	void DeleteSelectedPats();
};

END_CUTLEADER_NAMESPACE()
