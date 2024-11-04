#pragma once

#include "clCutFeatureCommon.h"
#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// 给回路添加“微连接”特征。
class MicroJointAddAction : public PartPlacementEditAction
{
	enum ADD_MICROJOINT_ACTION_STATE
	{
		ADD_MICROJOINT_STATE_NONE			= 0,
		ADD_MICROJOINT_STATE_SELECTLOOP		= 1,
	};

public:
	MicroJointAddAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort, ParamConfigPtr pParamConfig,
		SpreadSamePartDlg* pSpreadSamePartDlg);
	~MicroJointAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return PMT_MJ_ADD; }

public:
    void AddMicroJointOnSelectedLoopInstance(LoopInstancePtr pSelectedLoopInstance);

private:
    // 根据选中的轮廓和当前光标点，得到一个新的微连接点。
    void UpdateNewPosition(const Point2D& cursorPos);

private:
	ADD_MICROJOINT_ACTION_STATE m_state;

	// 所有的“回路实例”。
	LoopInstanceListPtr m_pAllLoopInstanceList;

	// 当前“回路实例”。
	LoopInstancePtr m_pActiveLoopInstance;

	// 选中的“回路实例”。
	LoopInstancePtr m_pSelectedLoopInst;

	// 添加的微连接在回路上的位置。
	Point2DPtr m_pMicroJointPt; // 这个点是零件模式下的坐标。
	int m_iPatIndex;

	// 参数配置。
	ParamConfigPtr m_pParamConfig;

	// “传播相同零件”对话框。
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
