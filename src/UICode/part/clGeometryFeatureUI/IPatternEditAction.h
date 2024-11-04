#pragma once

#include "clGeometryFeatureUICommon.h"
#include "IAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// this interface is the common interface for all pattern-related actions.
class IPatternEditAction : virtual public IAction
{
public:
    virtual ~IPatternEditAction() {}

public:
	// whether the action supports the "draw assist".
	// notes:
	//     1) when drawing geometry, we need "draw assist", except for the "select pattern" action.
	//     2) when modifying lead in/out or other cases, we also need "draw assist".
	virtual BOOL SupportDrawAssit() = 0;
};

END_CUTLEADER_NAMESPACE()
