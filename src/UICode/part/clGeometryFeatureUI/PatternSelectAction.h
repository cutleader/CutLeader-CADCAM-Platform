#pragma once

#include "PatternEditActionBase.h"

DECLARE_CUTLEADER_CLASS(PatternSelectAction)

BEGIN_CUTLEADER_NAMESPACE()

// use this class to select pattern from the screen.
// notes:
//   1) about "DRAW_SELECT_STATE":
//      a) for click(db click)-select and box-select, they both generate two inputs: left button down and left button up. Both of these two ways will select the 
//         patterns within the area defined by left button down/up position, especially this area for click(db click)-select is a point.
//   2) we can also call "SelectPat" to select patterns.
//   3) if pattern(s) are selected by click-select/box-select and the pattern prop bar is visible, 
//      we should update the prop bar. If pattern(s) are selected by db-click, we should show pattern prop bar.
class PatternSelectAction : public PatternEditActionBase
{
	typedef enum tagDrawSelectState
	{
		DRAW_SELECT_NONE			= 0,
		DRAW_SELECT_FIRST_INPUT		= 1,
		DRAW_SELECT_SECOND_INPUT	= 2,
	} DRAW_SELECT_STATE;

public:
	PatternSelectAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort);
	~PatternSelectAction(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL LButtonDBClk(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_SELECTPATTERN; }
	virtual void UpdateForUndoRedo();

public: // get/set functions.
	// get patterns those are selected.
	PatternListPtr GetSelectedPats() { return m_pSelectedPats; }

public:
	// clear current selected patterns and select "pPatList".
	// notes:
	//   1) remember to set "m_state" as "DRAW_SELECT_SECOND_INPUT".
	//   2) in "PatViewBar", if we select pattern(s) in tree, we should call this.
	void SelectPats(PatternListPtr pPatList, BOOL bShowPropBar = FALSE);

private:
	// after a left button click, we should check whether user want to grip-edit pattern. 
	// if YES, we should leave this action and switch into "grip edit" action.
	BOOL CheckGripEdit(double& dPtX, double& dPtY);

	// create the action which can do "Grip Edit" for "pPattern".
	/* params:
	*  snapPt: the point where we snapped "pPattern".
	*/
	// notes:
	//   1) this function will update "m_pNextAction".
	void SwitchToGripEdit(IPatternPtr pPattern, Point2D snapPt);

	// check whether mouse is "very" close to one control point, that control point
	// is the snap pt we want.
	// notes:
	//   1) this function will update "m_pSnapPt".
	BOOL CheckSnapPt(double dPtX, double dPtY);

	// calculate which pattern are selected using "m_data".
	PatternListPtr CalcSelectPat();

	// delete selected patterns.
	void DeleteSelectedPats();

private:
	// only one pattern is active(highlighted).
	IPatternPtr m_pActivePat;

	// more than one patterns can be selected.
	PatternListPtr m_pSelectedPats;

	// when pattern(s) are selected, the mouse move handler will find the
	// control point of the pattern(s) which is in the capture range of the
	// cursor, we call that point as "snap point".
	Point2DPtr m_pSnapPt;

	DRAW_SELECT_STATE m_state;

	// the data for selection.
	/*
		LeftTopPointX;
		LeftTopPointY;
		RightBottomPointX;
		RightBottomPointY;
	*/
	double m_data[4];
};

END_CUTLEADER_NAMESPACE()
