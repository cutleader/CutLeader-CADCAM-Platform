#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// ���������ѡ�еĹ���ǰ������µġ���·���򡱡�
class LoopSequenceInsertAction : public SequenceEditActionBase
{
	typedef enum tagStartCutEditState
	{
		SEQUENCE_INSERT_NONE					= 0,
		SEQUENCE_INSERT_SELECT_BASE_SEQ			= 1,
		SEQUENCE_INSERT_SELECT_NEWLOOPINST		= 2,
	} SEQUENCE_INSERT_STATE;

public:
	LoopSequenceInsertAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~LoopSequenceInsertAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_INSERT_LOOPSEQ; }

private:
	SEQUENCE_INSERT_STATE m_state;

	// �»�·��������������ǰ�档
	ICutSequencePtr m_pActiveCutSeq;

	// the loop instance which will be sequenced.
	LoopInstancePtr m_pActiveLoopInstance;

	// �������еġ���·ʵ����
	LoopInstanceListPtr m_pLoopInstanceList;

	// ������������˹���ġ���·ʵ������
	LoopInstanceListPtr m_pAllSequencedLoopInstances;
};

END_CUTLEADER_NAMESPACE()
