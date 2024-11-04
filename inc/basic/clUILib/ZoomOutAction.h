#pragma once

#include "ZoomBaseAction.h"
#include "Matrix2D.h"

BEGIN_CUTLEADER_NAMESPACE()

// zoom out action.
class ClUILib_Export ZoomOutAction : public ZoomBaseAction
{
public:
	ZoomOutAction(GlViewPortPtr pViewPort);
	~ZoomOutAction(void);

public: // implement IAction interface.
	virtual int GetActionType() const override { return ACTION_ZOOM_OUT; }

public:
	virtual void GetZoomPort(double& dWorldRange, 
							 double& dScreenLength, double& dScreenHeight,
							 double& dLeftBottomX, double& dLeftBottomY);
};

END_CUTLEADER_NAMESPACE()
