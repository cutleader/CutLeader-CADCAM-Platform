#pragma once

#include "Point2D.h"
#include "CornerPreSelectAction.h"

DECLARE_CUTLEADER_CLASS(CornerConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

// ���༭��������������
class ClCutFeatureUI_Export CornerEditAction : public CornerPreSelectAction
{
public:
	CornerEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort,
        CornerConfigItemPtr pCornerConfigItem);
	~CornerEditAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return CORNER_EDIT; }

	// update the action for undo/redo operation.
	// notes:
	//   1) if the current edit corner is lost, rest the status of action.
	//   2) hide CornerPropertyBar.
	virtual void UpdateForUndoRedo();

private:
    // �������������
    CornerConfigItemPtr m_pCornerConfigItem;
};

END_CUTLEADER_NAMESPACE()