#pragma once

#include "PatternEditActionBase.h"

DECLARE_CUTLEADER_CLASS(IPatternLoop)

BEGIN_CUTLEADER_NAMESPACE()

class PatternLoopOffsetAction : public PatternEditActionBase
{
	typedef enum tagOffsetLoopState
	{
		OFFSET_LOOP_NOTSTART		= 0,
		OFFSET_LOOP_SELECT_LOOP		= 1,
	} OFFSET_LOOP_STATE;

public:
	PatternLoopOffsetAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort);
	~PatternLoopOffsetAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_OFFSET; }

private:
	// 动作的状态。
	OFFSET_LOOP_STATE m_state;

	// 离光标最近的回路。
	IPatternLoopPtr m_pActiveLoop;
};

END_CUTLEADER_NAMESPACE()
