#pragma once

#include "clUILibCommon.h"
#include "Matrix2D.h"
#include "ZoomBaseAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// zoom in action.
class ClUILib_Export ZoomInAction : public ZoomBaseAction
{
public:
	ZoomInAction(GlViewPortPtr pViewPort);
	virtual ~ZoomInAction(void);

public: // implement IAction interface.
	virtual int GetActionType() const override { return ACTION_ZOOM_IN; }

public:
	virtual void GetZoomPort(double& dWorldRange, double& dScreenLength, double& dScreenHeight, double& dLeftBottomX, double& dLeftBottomY);
};

END_CUTLEADER_NAMESPACE()