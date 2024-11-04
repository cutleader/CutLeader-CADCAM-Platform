#pragma once

#include "ActionBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// the dummy action.
// notes:
//   1) under this action we can do nothing.
class ClUILib_Export DummyAction : public ActionBase
{
public:
	DummyAction(GlViewPortPtr pViewPort);
	virtual ~DummyAction(void);

public: // implement IAction interface.
	// this function only draw a triangle cursor.
	virtual BOOL DrawBeforeFinish();

	// there are many actions in the system, only this action's type is -1.
	virtual int GetActionType() const override { return ACTION_NONE; }
};

END_CUTLEADER_NAMESPACE()
