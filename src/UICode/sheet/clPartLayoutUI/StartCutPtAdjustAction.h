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

// ������·���е㡣
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
	// �����е��Ƶ����ϡ�
	void StartPt2Corner(LoopStartCutDataPtr pLoopStartCutData, ILoopStartCutPtPtr pLoopStartCutPt, ILeadPtr pLeadIn, ILeadPtr pLeadOut);

	// �����е��Ƶ�ͼ���ϡ�
	void StartPt2Geom(LoopStartCutDataPtr pLoopStartCutData, LONGLONG iNewPatID, const PatternPosition& newPatPos);

    // ����ѡ�е������͵�ǰ���㣬�õ�һ���µ����е㡣
    void UpdateNewPosition(const Point2D& cursorPos);

private:
	START_PT_ADJUST_STATE m_state;

	// ���еġ���·ʵ������
	LoopInstanceListPtr m_pAllLoopInstances;

	// ��ǰ����·ʵ������
	LoopInstancePtr m_pActiveLoopInstance;

	// ѡ�еġ���·ʵ������
	LoopInstancePtr m_pSelLoopInst;

	// �µ����е�λ�á�
	Point2D m_newPt;
	int m_iPatIndex;

	// ���������߷������
	LeadConfigItemPtr m_pLeadConfigItem;

	// ��������ͬ������Ի���
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
