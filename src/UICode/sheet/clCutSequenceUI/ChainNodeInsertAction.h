#pragma once

#include "SequenceEditActionBase.h"
#include "Point2D.h"
#include "InsertPositionDlg.h"

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(IChainNode)

BEGIN_CUTLEADER_NAMESPACE()

// ��������п��Բ��롰����ڵ㡱��
class ChainNodeInsertAction : public SequenceEditActionBase
{
	typedef enum tagChainNodeInsertState
	{
		NONE						= 0,
		CHAIN_NODE_SELECTED			= 1,
	} CHAINNODE_INSERT_STATE;

public:
	ChainNodeInsertAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, InsertPositionDlg* pInsertPosDlg);
	~ChainNodeInsertAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_INSERT_CHAINNODE; }
	virtual void UpdateForUndoRedo();

private:
	// ����ڵ㡣
	void InsertChainNode();

private:
	CHAINNODE_INSERT_STATE m_iState;

	// ��ǰ���㡣
	Point2D m_currentPt;

	// �������ס�ġ���·ʵ������
	LoopInstancePtr m_pActiveLoopInstance;

	// �µ����е�λ�á�
	BOOL m_bOldStartPt; // �����ϵ����е㡣
	Point2D m_newPt;
	int m_iNodeIndex;

	// ��׼�ڵ㡣
	// ע��
	//  1) �²���Ľڵ�ᱻ��Ϊ�µĻ�׼�ڵ㡣
	IChainNodePtr m_pBaseChainNode;

	// �������е���Χ����·ʵ������
	LoopInstanceListPtr m_pOuterLoopInsts;

	InsertPositionDlg* m_pInsertPosDlg;
};

END_CUTLEADER_NAMESPACE()
