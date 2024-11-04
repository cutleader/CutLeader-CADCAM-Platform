#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutPartSequence)
DECLARE_CUTLEADER_CLASS(CutSequenceSelectAction)
DECLARE_CUTLEADER_CLASS(SequenceSimulateShapeList)

BEGIN_CUTLEADER_NAMESPACE()

// ����ѡ������
// ע��
//  1) ͨ��ѡ�񡰻�·ʵ������ѡ����
//  2) Ŀǰ֧�ְ�סctrl��ѡ��
class ClCutSequenceUI_Export CutSequenceSelectAction : public SequenceEditActionBase
{
public:
	CutSequenceSelectAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~CutSequenceSelectAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_SELECT; }
	virtual void UpdateForUndoRedo();

public: // get/set functions.
	// get sequence units those are selected.
	CutSequenceListPtr GetSelSeqUnits() const { return m_pSelSeqUnitList; }

public: // other functions.
	void SelectedSeqUnit(CutSequenceListPtr pSelSeqUnitList) { m_pSelSeqUnitList = pSelSeqUnitList; }

private:
	// ��ǰ�Ĺ���
	ICutPartSequencePtr m_pCurCutSeq;

	// ѡ�еĹ���
	CutSequenceListPtr m_pSelSeqUnitList;

	// ������������˹���ġ���·ʵ������
	LoopInstanceListPtr m_pAllSequencedLoopInstances;

	// ���湤��ģ����Ϣ
	SequenceSimulateShapeListPtr m_pSequenceSimulateShapeList;
};

END_CUTLEADER_NAMESPACE()
