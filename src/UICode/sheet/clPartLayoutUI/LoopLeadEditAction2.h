#pragma once

#include "ObserverBase.h"
#include "StartCutPtPreSelectAction2.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(LoopInstance)

BEGIN_CUTLEADER_NAMESPACE()

// 板材模式下编辑“回路实例”的引入引出线和起切点。
class LoopLeadEditAction2 : public StartCutPtPreSelectAction2, public ObserverBase
{
public:
	LoopLeadEditAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort, 
						LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList, SpreadSamePartDlg* pSpreadSamePartDlg);
	~LoopLeadEditAction2(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return LEAD_ADJUST; }
	virtual void UpdateForUndoRedo();
	virtual void UpdateActionBuffer();

public: // implement IObserver interface.
	virtual void Update(int iEventID, VARIANT varEventContent) override;

public:
    void EditLeadsOfSelectedLoopInstances(LoopInstanceListPtr pSelectedLoopInstances);

private:
	void DisplayLoopLead();

private:
	// 引入引出线分配规则。
	LeadConfigItemPtr m_pLeadConfigItem;

	// 所有条件类型。
	ConditionTypeListPtr m_pCondTypeList;

	// “传播相同零件”对话框。
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
