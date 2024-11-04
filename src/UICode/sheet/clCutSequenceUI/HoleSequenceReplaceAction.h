#pragma once

#include "SequenceEditActionBase.h"
#include "Rect2D.h"
#include <map>

DECLARE_CUTLEADER_CLASS(PartInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ICutSequence)

BEGIN_CUTLEADER_NAMESPACE()

// �������������򡱵Ķ�����
class HoleSequenceReplaceAction : public SequenceEditActionBase
{
public:
	HoleSequenceReplaceAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~HoleSequenceReplaceAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_REPLACE_HOLE_SEQ; }
	virtual void UpdateForUndoRedo();

private:
	// ��Щ�����ǲ��ǿ���һ��ġ�
	// ע��
	//  1) map�еĵ�һ�������ǹ��������й����е���ţ��������Ӧ����������1����������Ϊ�ǿ���һ��
	BOOL IsSeqLinked(std::map<int, ICutSequencePtr> seqMap);

private:
	// ��������Ϊ��׼���и��ơ�
	PartInstancePtr m_pBasePartInst;

	// �������еġ���·ʵ������
	LoopInstanceListPtr m_pLoopInstanceList;

	// �������ʵ���и�����·ʵ������Ӿ��Ρ�
	std::map<LONGLONG, std::pair<Rect2D, std::map<LONGLONG, Rect2D>>> m_partRectMap;
};

END_CUTLEADER_NAMESPACE()
