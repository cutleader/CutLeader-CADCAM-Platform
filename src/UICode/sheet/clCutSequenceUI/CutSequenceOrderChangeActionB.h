#pragma once

#include "SequenceEditActionBase.h"
#include "ApplyPartDlg.h"

DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// 依次选中工序进行排序。
// 注：
//  1) 第一次选中的工序作为基准。
class CutSequenceOrderChangeActionB : public SequenceEditActionBase
{
public:
	CutSequenceOrderChangeActionB(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg);
	~CutSequenceOrderChangeActionB(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_SORT_B; }
	virtual void UpdateForUndoRedo();

private:
	// 当前的工序。
	CutSequenceListPtr m_pCurrentCutSequences;

	// 基准工序。
	// 注：
	//  1) 点中的工序都放在这个工序后面，这个变量需要更新。
	ICutSequencePtr m_pBaseCutSequence;

	// 缓存所有添加了工序的“回路实例”。
	LoopInstanceListPtr m_pAllSequencedLoopInstances;

	// “以零件为单位”对话框。
	ApplyPartDlg* m_pApplyPartDlg;
};

END_CUTLEADER_NAMESPACE()
