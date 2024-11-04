#pragma once

#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"
#include "PatternPosition.h"

DECLARE_CUTLEADER_CLASS(CondConfigItem)
DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ILoopStartCutPt)
DECLARE_CUTLEADER_CLASS(ILead)
DECLARE_CUTLEADER_CLASS(LoopStartCutData)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// 调整回路起切点。
class StartCutPtAdjustAction : public PartPlacementEditAction
{
	enum START_PT_ADJUST_STATE
	{
		START_PT_ADJUST_NONE			= 0,
		START_PT_ADJUST_SELECTLOOP		= 1,
	};

public:
	StartCutPtAdjustAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
						LeadConfigItemPtr pLeadConfigItem, SpreadSamePartDlg* pSpreadSamePartDlg);
	~StartCutPtAdjustAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return START_PT_ADJUST; }

public:
    void EditStartCutPoint_OfSelectedLoopInstance(LoopInstancePtr pLoopInstance);

private:
	// 把起切点移到角上。
	void StartPt2Corner(LoopStartCutDataPtr pLoopStartCutData, ILoopStartCutPtPtr pLoopStartCutPt, ILeadPtr pLeadIn, ILeadPtr pLeadOut);

	// 把起切点移到图形上。
	void StartPt2Geom(LoopStartCutDataPtr pLoopStartCutData, LONGLONG iNewPatID, const PatternPosition& newPatPos);

    // 根据选中的轮廓和当前光标点，得到一个新的起切点。
    void UpdateNewPosition(const Point2D& cursorPos);

private:
	START_PT_ADJUST_STATE m_state;

	// 所有的“回路实例”。
	LoopInstanceListPtr m_pAllLoopInstances;

	// 当前“回路实例”。
	LoopInstancePtr m_pActiveLoopInstance;

	// 选中的“回路实例”。
	LoopInstancePtr m_pSelLoopInst;

	// 新的起切点位置。
	Point2D m_newPt;
	int m_iPatIndex;

	// 引入引出线分配规则。
	LeadConfigItemPtr m_pLeadConfigItem;

	// “传播相同零件”对话框。
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
