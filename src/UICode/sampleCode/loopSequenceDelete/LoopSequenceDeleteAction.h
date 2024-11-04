#pragma once

#include "CutSequencePreSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// This action will select a loop sequence and delete it, 这个动作选中一个轮廓工序，然后删除之。
class LoopSequenceDeleteAction : public CutSequencePreSelectAction
{
public:
	LoopSequenceDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData,
        GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg);
	~LoopSequenceDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_Remove_LoopSequence; }
	virtual void UpdateForUndoRedo();
};

END_CUTLEADER_NAMESPACE()
