#pragma once

#include "Matrix2D.h"
#include "PartPlacementEditAction.h"
#include "PartPlacementMoveBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacementAddAction)

BEGIN_CUTLEADER_NAMESPACE()

// �����ݿ����һ���������ġ�
// ע��
//  1) ����������ӡ�
class PartPlacementAddAction : public PartPlacementEditAction, public PartPlacementMoveBase
{
public:
	PartPlacementAddAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
					 LONGLONG iPartID, LONGLONG iShtParamConfigID, DistanceDetectDlg* pDisDetectDlg);
	~PartPlacementAddAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_ADDPARTPMT; }
	virtual void UpdateForUndoRedo();

private:
    // �����ܿ����������Ի���
    DistanceDetectDlg* m_pDisDetectDlg;
};

END_CUTLEADER_NAMESPACE()
