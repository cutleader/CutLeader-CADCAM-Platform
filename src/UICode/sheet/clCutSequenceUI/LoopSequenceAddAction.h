#pragma once

#include "SequenceEditActionBase.h"
#include "Rect2D.h"
#include "ApplyPartDlg.h"
#include <map>

DECLARE_CUTLEADER_CLASS(PartInstance)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// ��ӡ��������򡱵Ķ�����
class LoopSequenceAddAction : public SequenceEditActionBase
{
public:
	LoopSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg);
	~LoopSequenceAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_LOOPSEQ; }
	virtual void UpdateForUndoRedo();

private:
	// �����ʵ�����乤��
	void AssignPartSeq(PartInstancePtr pPartInstance);

private:
	// ��ǰ�ġ���·ʵ������
	LoopInstancePtr m_pActiveLoopInstance;

	// ��ǰ�����ʵ����
	// ע��
	//  1) �����Ϊ��λʱ�õ���
	PartInstancePtr m_pActivePartInst;

	// �������еġ���·ʵ������
	LoopInstanceListPtr m_pLoopInstanceList;

	// �������Ϊ��λ���Ի���
	ApplyPartDlg* m_pApplyPartDlg;

	// �������ʵ���и�����·ʵ������Ӿ��Ρ�
	std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG, Rect2D>>> m_partRectMap;
};

END_CUTLEADER_NAMESPACE()
