#pragma once

#include "PatternGripEditActionBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// this class will create and edit the line pattern.
class LinePatternEditAction : public PatternGripEditActionBase
{
protected:
	typedef enum tagDrawLineState
	{
		DRAW_LINE_NOTSTART		=0,
		DRAW_LINE_START_POINT	=1,
		DRAW_LINE_END_POINT		=2,

	} DRAW_LINE_STATE;

	typedef enum tagEditLineState
	{
		EDIT_LINE_NOTSTART		=0,
		EDIT_LINE_CENTER_POINT	=1,
		EDIT_LINE_START_POINT	=2,
		EDIT_LINE_END_POINT		=3,

	} EDIT_LINE_STATE;

public:
	LinePatternEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort);
	~LinePatternEditAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return DRAW_LINE; }
	virtual void UpdateForUndoRedo(); // if pattern is lost when grip-edit pattern, return to PatternSelectAction.

public: // implement IGripEditPatternAction interface.
	virtual void SetGripEditPattern(IPatternPtr pPattern);
	virtual void SetGripEditPt(double dPtX, double dPtY);

protected:
	double m_data[4]; // the data for creating line.

	DRAW_LINE_STATE m_drawState;
	EDIT_LINE_STATE m_editState;
};

END_CUTLEADER_NAMESPACE()
