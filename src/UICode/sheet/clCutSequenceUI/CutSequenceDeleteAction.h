#pragma once

#include "CutSequencePreSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// 删除工序的动作。
class CutSequenceDeleteAction : public CutSequencePreSelectAction
{
	typedef enum tagStartCutEditState
	{
		SEQUENCE_DELETE_NONE				= 0,
		SEQUENCE_DELETE_SELECT_LOOPINST		= 1,
	} SEQUENCE_DELETE_STATE;

public:
	CutSequenceDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg);
	~CutSequenceDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_REMOVE; }
	virtual void UpdateForUndoRedo();

public:
	// 选中这些工序。
	// 注: 
	//   1) 这个函数类似鼠标选中的操作。
	void SelectSeqUnits(CutSequenceListPtr pCutSequences);

private:
	SEQUENCE_DELETE_STATE m_state;
};

END_CUTLEADER_NAMESPACE()
