#pragma once

#include "clCutSequenceUICommon.h"
#include "Point2D.h"
#include "Rect2D.h"
#include "SequenceEditActionBase.h"
#include <map>

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// ��ӡ��Žӹ��򡱵Ķ�����
// ע��
//   1) ������������½��µ��Žӹ��򣬻������������Žӹ����������µġ��Žӽڵ㡱��
//   2) һ�������Ķ�����ѡ��2����Χ����·ʵ�������и�˳����ѭѡ��˳��
//   3) ��һ����Χ��·���С�(����)�ӳ����򡱻��ߡ����߹���ʱ�����ܶ�֮���Žӡ�
class BridgeSequenceAddAction : public SequenceEditActionBase
{
	typedef enum tagBdgeSeqAddState
	{
		NONE						= 0,
		FIRST_BDGE_PT_SELECTED		= 1,
		SECOND_BDGE_PT_SELECTED		= 2,
	} BDGESEQ_ADD_STATE;

public:
	BridgeSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~BridgeSequenceAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_BDGESEQ; }
	virtual void UpdateForUndoRedo();

private:
	// ����ܷ����������·ʵ�����ϼ��Žӡ�
	// ע��
	//   1) ��һ����Χ��·���С�����/�����ʱ�����ܶ�֮���Žӡ�
	BOOL LoopCanAddBdge(LoopInstancePtr pLoopInstance);

	// ��ѡ�е�2������·ʵ�����������Žӡ�
	void AddBdgeCut();

private:
	BDGESEQ_ADD_STATE m_iState;

	/************************************************************************/	
	// ���ڵ�һ������·ʵ��������Ϣ��

	// ��·ʵ����
	LoopInstancePtr m_pFstLoopInst;

	// �Žӵ㡣
	Point2D m_fstBdgePt;
	int m_iFstLoopNodeIdx;
	/************************************************************************/

	/************************************************************************/
	// ���ڵڶ�������·ʵ��������Ϣ��

	// ��·ʵ����
	LoopInstancePtr m_pSndLoopInst;

	// �Žӵ㡣
	Point2D m_sndBdgePt;
	int m_iSndLoopNodeIdx;
	/************************************************************************/

	// �������е���Χ����·ʵ������
	LoopInstanceListPtr m_pOuterLoopInsts;

	// �������ʵ���и�����·ʵ������Ӿ��Ρ�
	std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG, Rect2D>>> m_partRectMap;
};

END_CUTLEADER_NAMESPACE()
