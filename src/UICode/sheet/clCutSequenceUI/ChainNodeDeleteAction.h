#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(IChainNode)

BEGIN_CUTLEADER_NAMESPACE()

// ɾ��������ڵ㡱�Ķ�����
class ChainNodeDeleteAction : public SequenceEditActionBase
{
public:
	ChainNodeDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~ChainNodeDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_DEL_CHAINNODE; }
	virtual void UpdateForUndoRedo();

private:
	// ����������Ľڵ㡣
	IChainNodePtr m_pActiveChainNode;
};

END_CUTLEADER_NAMESPACE()
