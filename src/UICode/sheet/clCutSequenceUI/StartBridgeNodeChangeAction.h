#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// �������Žӹ��򡱵���ʼ������
class StartBridgeNodeChangeAction : public SequenceEditActionBase
{
public:
	StartBridgeNodeChangeAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~StartBridgeNodeChangeAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_BDGE_START_CHANGE; }
	virtual void UpdateForUndoRedo();

private:
	// ��ǰ�ġ���·ʵ������
	LoopInstancePtr m_pActiveLoopInstance;

	// �������С��Žӹ����еġ���·ʵ������
	LoopInstanceListPtr m_pBdgeLoopInsts;
};

END_CUTLEADER_NAMESPACE()
