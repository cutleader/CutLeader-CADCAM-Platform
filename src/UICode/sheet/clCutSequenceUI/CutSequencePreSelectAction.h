#pragma once

#include "SequenceEditActionBase.h"
#include "ApplyPartDlg.h"

DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// һЩ�����Ļ��࣬��Щ������Ҫѡ����
// ע��
//  1) ͨ��ѡ�񡰻�·ʵ������ѡ����
//  2) ���������Ϊ��λ������ѡ��ʱ���õ���ǰ�����漰���������������й��������ǰ�������ڵ�����������ţ���ô�Ź����漰��������������ᱻѡ����
class ClCutSequenceUI_Export CutSequencePreSelectAction : public SequenceEditActionBase
{
protected:
    // ��������������������״ֵ̬�����Ƿ����ø�����й���ѡ��
	enum PreSelectSequenceActionState
	{
		// the pre-select mode is inactive, mean every input cannot go into this action.
		PreSelectSequenceAction_inactive			= -1,

		// we can begin to select.
		PreSelectSequenceAction_active			    = 0,
	};

public:
	CutSequencePreSelectAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg);
	~CutSequencePreSelectAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_SELECT; }

public: // other functions.
	void SelectedSeqUnit(CutSequenceListPtr pSelCutSeqs) { m_pSelectCutSequences = pSelCutSeqs; }

protected:
	PreSelectSequenceActionState m_iPreSelectSequenceActionState;

	// ��ǰ�Ĺ���
	CutSequenceListPtr m_pCurrentCutSequences;

	// ѡ�еĹ���
	CutSequenceListPtr m_pSelectCutSequences;

	// ������������˹���ġ���·ʵ������
	LoopInstanceListPtr m_pAllSequencedLoopInstances;

	// �������Ϊ��λ���Ի���
	ApplyPartDlg* m_pApplyPartDlg;
};

END_CUTLEADER_NAMESPACE()
