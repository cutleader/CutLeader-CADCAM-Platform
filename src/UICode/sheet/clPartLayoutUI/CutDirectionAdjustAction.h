#pragma once

#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(LoopInstance)

BEGIN_CUTLEADER_NAMESPACE()

// 调整“回路实例”的切割方向。
class CutDirectionAdjustAction : public PartPlacementEditAction
{
	typedef enum tagDirAdjustState
	{
		DIR_ADJUST_NONE				= 0,
		DIR_ADJUST_SELECT_LOOPINST	= 1,
	} DIR_ADJUST_STATE;

public:
	CutDirectionAdjustAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
		SpreadSamePartDlg* pSpreadSamePartDlg);
	virtual ~CutDirectionAdjustAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return DIRECTION_ADJUST; }

public:
    void ChangeCutDirection_OfSelectedLoopInstance(LoopInstancePtr pSelectedLoopInstance);

private:
	DIR_ADJUST_STATE m_state;

	// 所有的“回路实例”。
	LoopInstanceListPtr m_pAllLoopInstanceList;

	// 当前的“回路实例”。
	LoopInstancePtr m_pActiveLoopInstance;

	// “传播相同零件”对话框。
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
