#pragma once

#include "Rect2D.h"
#include "PartPlacementPreSelectAction.h"
#include "PartPlacementMoveBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacementMoveAction)

BEGIN_CUTLEADER_NAMESPACE()

// 移动“零件放置”的动作。
class PartPlacementMoveAction : public PartPlacementPreSelectAction, public PartPlacementMoveBase
{
public:
	PartPlacementMoveAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, PartPlacementListPtr pSelectedPartPlacements, GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg);
	~PartPlacementMoveAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_MOVEPRTPMT; }

private:
	// 选中的“零件放置”的中心点。
	// 注：
	//  1) 这个变量只在画牵引线时用到。
	Point2D m_partCenterPt;
};

END_CUTLEADER_NAMESPACE()
