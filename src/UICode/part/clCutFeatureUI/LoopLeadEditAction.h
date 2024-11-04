#pragma once

#include "StartCutPtPreSelectAction.h"

DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(LeadConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

// 编辑回路引入引出线和起切点。
class LoopLeadEditAction : public StartCutPtPreSelectAction
{
public:
	LoopLeadEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList,
					   GlViewPortPtr pViewPort, LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList);
	~LoopLeadEditAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return LEAD_EDIT; }
	virtual void UpdateForUndoRedo();

private:
	// 引入引出线分配规则。
	LeadConfigItemPtr m_pLeadConfigItem;

	// 所有条件类型。
	ConditionTypeListPtr m_pCondTypeList;
};

END_CUTLEADER_NAMESPACE()
