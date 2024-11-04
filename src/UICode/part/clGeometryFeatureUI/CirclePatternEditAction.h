#pragma once

#include "PatternGripEditActionBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// this class will create/edit circle pattern.
class CirclePatternEditAction : public PatternGripEditActionBase
{
	typedef enum tagDrawCircleState
	{
		DRAW_CIRCLE_NOTSTART	     = 0,
		DRAW_CIRCLE_CENTER_POINT	 = 1,
		DRAW_CIRCLE_END_POINT		 = 2,

	} DRAW_CIRCLE_STATE;

	typedef enum tagEditCircleState
	{
		EDIT_CIRCLE_NOTSTART         = 0,
		EDIT_CIRCLE_CENTER_POINT	 = 1,
		EDIT_CIRCLE_TOP_POINT		 = 2,
		EDIT_CIRCLE_LEFT_POINT		 = 3,
		EDIT_CIRCLE_BOTTOM_POINT     = 4,
		EDIT_CIRCLE_RIGHT_POINT		 = 5,

	} EDIT_CIRCLE_STATE;

public:
	CirclePatternEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort);
	~CirclePatternEditAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return DRAW_ROUNDHOLE; }
	virtual void ConvertKBInput(double& dVal1, double& dVal2);
	virtual void UpdateForUndoRedo(); // if pattern is lost when grip-edit pattern, return to PatternSelectAction.

public: // implement IGripEditPatternAction interface.
	virtual void SetGripEditPattern(IPatternPtr pPattern);
	virtual void SetGripEditPt(double dPtX, double dPtY);

private:
	DRAW_CIRCLE_STATE m_drawState;
	EDIT_CIRCLE_STATE m_editState;

	// the data for creating circle.
	/* 
		CenterPointX 
		CenterPointY 
		Radius
	*/
	double m_data[3];
};

END_CUTLEADER_NAMESPACE()
