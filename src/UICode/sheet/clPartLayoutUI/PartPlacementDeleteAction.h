#pragma once

#include "PartPlacementPreSelectAction.h"

DECLARE_CUTLEADER_CLASS(PartPlacementDeleteAction)

BEGIN_CUTLEADER_NAMESPACE()

class PartPlacementDeleteAction : public PartPlacementPreSelectAction
{
public:
	PartPlacementDeleteAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, PartPlacementListPtr pSelectedPartPlacements, GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg);
	~PartPlacementDeleteAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:  // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_DELETEPARTPMT; }
	virtual void UpdateForUndoRedo();

public:
	// delete selected part pmts.
	void DeleteSelectedPartPlacements();
};

END_CUTLEADER_NAMESPACE()
