#pragma once

#include "clPartLayoutUICommon.h"
#include "PartPlacementTransformAction.h"
#include "Matrix2D.h"


DECLARE_CUTLEADER_CLASS(PartPlacementRotateAction)


BEGIN_CUTLEADER_NAMESPACE()

class PartPlacementRotateAction : public PartPlacementTransformAction
{
public:
	PartPlacementRotateAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
						PartPlacementListPtr pSelectedPartPlacements, GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg);
	~PartPlacementRotateAction(void);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_ROTATEPARTPMT; }
	virtual void ConvertKBInput(double& dVal1, double& dVal2);

private: // implement IPartPlacementTransformAction interface.
	virtual void SetMatrixValue();
	virtual CString GetActionName();
	virtual int GetMsgID();
};

END_CUTLEADER_NAMESPACE()
