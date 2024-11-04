#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// ��ӡ������и�򡱵Ķ�����
class ReplaceAsFlyCutAction : public SequenceEditActionBase
{
	enum FLYCUTSEQ_ADD_STATE
	{
		NONE						= 0,
		FLYCUT_SEQ_FIRST_POINT		= 1, // �������
		FLYCUT_SEQ_SECOND_POINT		= 2, // ���̧��
	};

public:
	ReplaceAsFlyCutAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	~ReplaceAsFlyCutAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_REPLACE_AS_FLYCUT; }
	virtual void UpdateForUndoRedo();

private:
	FLYCUTSEQ_ADD_STATE m_iState;

	// the data for selection.
	/*
		LeftTopPointX;
		LeftTopPointY;
		RightBottomPointX;
		RightBottomPointY;
	*/
	double m_data[4];

	// ����ƶ�ʱ�ĺ�ѡloop inst��ֻ��һ����Ч��
	LoopInstancePtr m_pActiveLoopInstance; // ����ѡʱ�ġ���ǰloop inst����
	LoopInstanceListPtr m_pRectSelectLoopInstances; // ��ѡʱ�ġ���ǰloop inst����

	// �������еġ���·ʵ������
	LoopInstanceListPtr m_pAllLoopInstances;
};

END_CUTLEADER_NAMESPACE()
