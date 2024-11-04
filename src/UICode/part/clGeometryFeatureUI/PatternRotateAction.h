#pragma once

#include "PatternTransformActionBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// this action can rotate the pattern.
class PatternRotateAction : public PatternTransformActionBase
{
public:
	PatternRotateAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort, PatternListPtr pSelPats);
	~PatternRotateAction(void);

public:  // implement IAction interface.
	BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_ROTATE; }
	virtual void ConvertKBInput(double& dVal1, double& dVal2);

public: // implement IPatternTransformAction interface.
	virtual int GetMsgID();
	virtual void SetMatrixValue();
	virtual CString GetActionName();
};

END_CUTLEADER_NAMESPACE()
