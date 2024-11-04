#pragma once

#include "PartPlacementEditAction.h"
#include "CornerConfigItem.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// �ڰ��ģʽ�����ӽ������Ķ�����
class CornerAddAction2 : public PartPlacementEditAction
{
	typedef enum tagMicroJointActionState
	{
		CORNER_STATE_NONE				= 0,
		CORNER_STATE_SELECTCORNER		= 1,
	} CORNER_ACTION_STATE;

public:
	CornerAddAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
		SpreadSamePartDlg* pSpreadSamePartDlg);
	~CornerAddAction2(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return PMT_CORNER_ADD; }

private:
	CORNER_ACTION_STATE m_state;

	// ��ǰ�Ļ�·ʵ����
	LoopInstancePtr m_pCurLoopInst;

	// ��ǰ�Ľǵ㡣
	Point2DPtr m_pCurCornerPt;

	// �������еĻ�·ʵ����
	LoopInstanceListPtr m_pAllLoopInstanceList;

	// ��������ͬ������Ի���
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
