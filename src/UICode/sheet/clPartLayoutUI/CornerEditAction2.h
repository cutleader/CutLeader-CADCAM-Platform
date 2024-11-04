#pragma once

#include "ObserverBase.h"
#include "CornerPreSelectAction2.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(CornerConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

// “编辑角特征”动作。
class CornerEditAction2 : public CornerPreSelectAction2, public ObserverBase
{
public:
	CornerEditAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
        SpreadSamePartDlg* pSpreadSamePartDlg, CornerConfigItemPtr pCornerConfigItem);
	~CornerEditAction2(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return PMT_CORNER_EDIT; }
	virtual void UpdateForUndoRedo();

public: // implement IObserver interface.
	virtual void Update(int iEventID, VARIANT varEventContent) override;

private:
	void DisplayCornerData();

private:
	SpreadSamePartDlg* m_pSpreadSamePartDlg;

    // 角特征分配规则
    CornerConfigItemPtr m_pCornerConfigItem;
};

END_CUTLEADER_NAMESPACE()