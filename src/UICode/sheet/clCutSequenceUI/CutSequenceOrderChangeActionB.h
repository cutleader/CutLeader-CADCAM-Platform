#pragma once

#include "SequenceEditActionBase.h"
#include "ApplyPartDlg.h"

DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// ����ѡ�й����������
// ע��
//  1) ��һ��ѡ�еĹ�����Ϊ��׼��
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
	// ��ǰ�Ĺ���
	CutSequenceListPtr m_pCurrentCutSequences;

	// ��׼����
	// ע��
	//  1) ���еĹ��򶼷������������棬���������Ҫ���¡�
	ICutSequencePtr m_pBaseCutSequence;

	// ������������˹���ġ���·ʵ������
	LoopInstanceListPtr m_pAllSequencedLoopInstances;

	// �������Ϊ��λ���Ի���
	ApplyPartDlg* m_pApplyPartDlg;
};

END_CUTLEADER_NAMESPACE()
