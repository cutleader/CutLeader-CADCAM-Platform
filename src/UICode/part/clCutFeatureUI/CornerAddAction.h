#pragma once

#include "CutFeatureEditBase.h"
#include "CornerConfigItem.h"

BEGIN_CUTLEADER_NAMESPACE()

// ���ӽ������Ķ�����
class ClCutFeatureUI_Export CornerAddAction : public CutFeatureEditBase
{
	enum CORNER_ACTION_STATE
	{
		CORNER_STATE_NONE				= 0,
		CORNER_STATE_SELECTCORNER		= 1,
	};

public:
	CornerAddAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort);
	~CornerAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return CORNER_ADD; }

private:
	CORNER_ACTION_STATE m_state;

	// the corner point.
	// notes:
	//   1) this point can only on the polygon loop.
	Point2DPtr m_pCornerPt;
};

END_CUTLEADER_NAMESPACE()
