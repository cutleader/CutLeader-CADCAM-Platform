#pragma once

#include "Rect2D.h"
#include "PartPlacementPreSelectAction.h"
#include "PartPlacementMoveBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacementMoveAction)

BEGIN_CUTLEADER_NAMESPACE()

// �ƶ���������á��Ķ�����
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
	// ѡ�еġ�������á������ĵ㡣
	// ע��
	//  1) �������ֻ�ڻ�ǣ����ʱ�õ���
	Point2D m_partCenterPt;
};

END_CUTLEADER_NAMESPACE()
