#pragma once

#include "clGeometryFeatureCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(Point2D)
DECLARE_CUTLEADER_CLASS(Point2DList)
DECLARE_CUTLEADER_CLASS(Line2D)
DECLARE_CUTLEADER_CLASS(Line2DList)
DECLARE_CUTLEADER_CLASS(PatternList)

BEGIN_CUTLEADER_NAMESPACE()

// the assistor for draw.
// notes:
//   1) a case which will use this assistor: draw a line.
//      a) in "mouse move handler", calculate the assist line/point and snap point.
//      b) in "mouse click handler", if a snap pt exist, adjust the mouse position to that snap point, do not adjust the cursor.
class ClGeometryFeature_Export DrawAssistor
{
public:
	DrawAssistor(GlViewPortPtr pViewPort);
	~DrawAssistor(void);

public:
	// init the assistor.
	// notes:
	//   1) we should transfer following information to assistor:
	//      a) existing Patterns.
	//      b) the first input(if exist) of the action.
	void Init(PatternListPtr pPatList, Point2DPtr pFirstPt);

	// update the assist line/point and snap point in this class.
	// notes:
	//   1) we should provide the current cursor position, and this function
	//      will calculate the snap pt relative to the cursor, of course we may not find snap point.
	void RefreshAssistor(double dPtX, double dPtY);

	// draw the assist information on the screen.
	// notes:
	//   1) when moving/clicking the mouse, we should draw these:
	//      a) snap point. (none or only one)
	//      b) assist lines.
	//      c) assist points.
	void DrawAssistInfo();

	// reset assist information.
	// notes:
	//   1) call this when:
	//      a) zoom in/out the view.
	//      b) perform some actions that do not support "draw assist".
	void Reset();

	// get the snap point, may be NULL.
	Point2DPtr GetSnapPt() { return m_pSnapPt; }

private:
	/************************************************************************/
	// about get the snap point.

	// get pattern's control point that is "close" to cursor.
	// notes:
	//   1) the returned point will be the snap point, of course we may not find such control point.
	Point2DPtr CheckPatCtrlPt();

	// check the cross points of patterns, and get the one that is "close" to cursor.
	// notes:
	//   1) the returned point will be the snap point, of course we may not find such control point.
	Point2DPtr CheckPatCrossPt();

	// check the cross point associated with the assist lines.
	// notes:
	//   1) if the assist line cross with the pattern, we should check whether it can be snap point.
	//   2) if two assist lines have the cross point, we also should check whether it can be snap point.
	//   3) the returned point will be the snap point, of course we may not find such control point.
	Point2DPtr CheckAssistLineCrossPt();
	/************************************************************************/

	// calculate assist lines.
	// notes: 
	//    1) we only get the horizontal and vertical assist lines.
	//    2) only need one horizontal and(or) vertical assist line.
	//    3) put the result to "m_pAssistLineList". 
	void CalcAssistLines();

private:
	GlViewPortPtr m_pViewPort;

	/************************************************************************/
	// input data.

	// existing Patterns.
	PatternListPtr m_pPatList;
	
	// the first input(if exist) of the action.
	Point2DPtr m_pFirstPt;

	// the current cursor position.
	Point2DPtr m_pCursorPt;
	/************************************************************************/

	/************************************************************************/
	// output data.

	// the figured out snap point.
	// notes:
	//   1) snap point is an assist point which stay in the capture range of the current cursor.
	Point2DPtr m_pSnapPt;

	// the figured out assist points.
	// notes:
	//   1) "assist point": the points which can be the reference.
	Point2DListPtr m_pAssistPtList;

	// the figured out assist lines.
	// notes:
	//   1) "assist line": the lines which can be the reference.
	Line2DListPtr m_pAssistLineList;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
