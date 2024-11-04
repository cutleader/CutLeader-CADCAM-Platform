#pragma once

#include "PatternTransformActionBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// this action can move the pattern.
class PatternMoveAction : public PatternTransformActionBase
{
public:
	PatternMoveAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort, PatternListPtr pSelPats);
	~PatternMoveAction(void);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_MOVE; }
	virtual void ConvertKBInput(double& dVal1, double& dVal2);

public: // implement IPatternTransformAction interface.
	virtual int GetMsgID();

	virtual void SetMatrixValue();
	virtual CString GetActionName();
};

END_CUTLEADER_NAMESPACE()
