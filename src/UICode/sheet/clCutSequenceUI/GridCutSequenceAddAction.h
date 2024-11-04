#pragma once

#include "SequenceEditActionBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartPlacementList)

BEGIN_CUTLEADER_NAMESPACE()

// ��ӡ����ֹ��򡱵Ķ�����
class GridCutSequenceAddAction : public SequenceEditActionBase
{
	typedef enum tagBdgeSeqAddState
	{
		NONE						= 0,
		GRID_SEQ_FIRST_POINT		= 1, // �������
		GRID_SEQ_SECOND_POINT		= 2, // ���̧��
	} GRIDSEQ_ADD_STATE;

public:
	GridCutSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort);
	virtual ~GridCutSequenceAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return SEQUENCE_ADD_GRIDSEQ; }
	virtual void UpdateForUndoRedo();

private:
	GRIDSEQ_ADD_STATE m_iState;

	// the data for selection.
	/*
		LeftTopPointX;
		LeftTopPointY;
		RightBottomPointX;
		RightBottomPointY;
	*/
	double m_data[4];

	// ����ƶ�ʱ�ĺ�ѡpartpmt��ֻ��һ����Ч��
	PartPlacementPtr m_pActivePartPlacement; // ����ѡʱ�ġ���ǰpartpmt����
	PartPlacementListPtr m_pRectSelectPartPlacements; // ��ѡʱ�ġ���ǰpartpmt����
};

END_CUTLEADER_NAMESPACE()
