#pragma once

#include "PartPlacementPreSelectAction.h"
#include "Rect2D.h"
#include "PartPlacementMoveBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacementCopyAction)

BEGIN_CUTLEADER_NAMESPACE()

class PartPlacementCopyAction : public PartPlacementPreSelectAction, public PartPlacementMoveBase
{
public:
	PartPlacementCopyAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
					  PartPlacementListPtr pSelectedPartPlacements, GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg);
	~PartPlacementCopyAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_COPYPRTPMT; }
};

END_CUTLEADER_NAMESPACE()
