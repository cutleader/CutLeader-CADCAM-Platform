#pragma once

#include "StartCutPtPreSelectAction.h"

DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(LeadConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

// �༭��·���������ߺ����е㡣
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
	// ���������߷������
	LeadConfigItemPtr m_pLeadConfigItem;

	// �����������͡�
	ConditionTypeListPtr m_pCondTypeList;
};

END_CUTLEADER_NAMESPACE()
