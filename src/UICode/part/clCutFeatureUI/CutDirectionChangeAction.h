#pragma once

#include "Point2D.h"
#include "CutFeatureEditBase.h"

DECLARE_CUTLEADER_CLASS(PatternList)

BEGIN_CUTLEADER_NAMESPACE()

// this action can change the cut direction of the loop.
// notes:
//   1) the work flow:
//        a) when moving mouse, the "nearest to cursor" loop will be highlighted.
//        b) after a click, the cut direction of the loop will be changed.
class ClCutFeatureUI_Export CutDirectionChangeAction : public CutFeatureEditBase
{
	enum DIR_CHANGE_STATE
	{
		DIR_CHANGE_NONE				= 0,
		DIR_CHANGE_SELECT_LOOPINST	= 1,
	};

public:
	CutDirectionChangeAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort);
	~CutDirectionChangeAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return DIRECTION_CHANGE; }

private:
	DIR_CHANGE_STATE m_state;
};

END_CUTLEADER_NAMESPACE()
