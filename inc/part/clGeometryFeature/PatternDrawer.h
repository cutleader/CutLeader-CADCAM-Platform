#pragma once

#include "clGeometryFeatureCommon.h"
#include "BasicGeometryDrawer.h"

DECLARE_CUTLEADER_CLASS(IPattern)
DECLARE_CUTLEADER_CLASS(PatternList)

BEGIN_CUTLEADER_NAMESPACE()

// this class draw the pattern.
// notes:
//   1) we do not draw the control pattern. 
//   2) we can call GetLineArcs() function of all patterns, and then draw each line and arc.
class ClGeometryFeature_Export PatternDrawer : public BasicGeometryDrawer
{
public:
	PatternDrawer(GlViewPortPtr pViewPort);
	~PatternDrawer(void);

public:
	// draw pattern.
	// notes:
	//   1) color and line width should be set in this function.
	void DrawPat(IPatternPtr pPattern);
	void DrawPatList(PatternListPtr pPatList);

	// draw active pattern.
	// notes:
	//   1) set the line width as 2.
	//   2) set the color in this function.
	void DrawActivePat(IPatternPtr pPattern);

	// draw selected pattern.
	// notes:
	//   1) set the line width as 1, use stipple mode.
	//   2) set the color in this function.
	void DrawSelPat(IPatternPtr pPattern);
	void DrawSelPatList(PatternListPtr pPatList);

private:
	// draw pattern.
	void DrawPatData(IPatternPtr pPattern);
};

END_CUTLEADER_NAMESPACE()
