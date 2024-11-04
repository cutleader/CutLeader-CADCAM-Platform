#pragma once

#include "SequenceEditActionBase.h"
#include "Point2D.h"

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ChainCutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// ��ӡ�����򡱵Ķ�����
// ע��
//  1) ����������л��½�������򡱣�Ҳ���������С�����򡱺�����ӡ�����ڵ㡱��
class ChainCutSequenceAddAction : public SequenceEditActionBase
{
	typedef enum tagBdgeSeqAddState
	{
		NONE						= 0,
		CHAIN_SEQ_CREATED			= 1,
	} CHAINSEQ_ADD_STATE;

public:
	ChainCutSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~ChainCutSequenceAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_CHAINSEQ; }
	virtual void UpdateForUndoRedo();

private:
	// ����������·ʵ�����ܷ�������
	// ע��
	//   1) �������·���С�����/�Žӹ���/���ʱ�����ܶ�֮�����
	BOOL Check4CreateChain(LoopInstancePtr pLoopInstance);

	// ����������򡱡�
	void CreateChainSeq();

	// �����С�����򡱺���ӽڵ㡣
	void AddChainNode();

private:
	CHAINSEQ_ADD_STATE m_iState;

	// ��ǰ���㡣
	Point2D m_currentPt;

	// �������ס�ġ���·ʵ������
	LoopInstancePtr m_pActiveLoopInstance;

	// �µ����е�λ�á�
	BOOL m_bOldStartPt; // �����ϵ����е㡣
	Point2D m_newPt;
	int m_iNodeIndex;

	// �����������ǰѡ�еĵ㡣
	Point2DListPtr m_pCutPts;

	// ������������������
	ChainCutSequencePtr m_pChainSeq;

	// �������е���Χ����·ʵ������
	LoopInstanceListPtr m_pOuterLoopInsts;
};

END_CUTLEADER_NAMESPACE()
