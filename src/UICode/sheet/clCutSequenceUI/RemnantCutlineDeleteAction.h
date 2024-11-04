#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// ɾ��ѡ�еĿ����ƶ��㡣
// ע��
//  �����ƣ�Ŀǰû���ṩundo/redo���ܡ�
class RemnantCutlineDeleteAction : public SequenceEditActionBase
{
public:
	RemnantCutlineDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~RemnantCutlineDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_DEL_REM_CUTLINE; }

private:
	ICutSequencePtr m_pActiveRemnantLineSeq;
};

END_CUTLEADER_NAMESPACE()
