#pragma once

#include "Point2D.h"
#include "CutFeatureEditBase.h"

DECLARE_CUTLEADER_CLASS(LeadConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

// ������·���е㡣
class StartCutPtChangeAction : public CutFeatureEditBase
{
	enum START_PT_CHANGE_STATE
	{
		START_PT_CHANGE_NONE			= 0,
		START_PT_CHANGE_SELECTLOOP		= 1,
	};

public:
	StartCutPtChangeAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList,
						GlViewPortPtr pViewPort, LeadConfigItemPtr pLeadConfigItem);
	~StartCutPtChangeAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return START_PT_CHANGE; }

private:
    // ����ѡ�е������͵�ǰ���㣬�õ�һ���µ����е㡣
    void UpdateNewPosition(const Point2D& cursorPos);

private:
	START_PT_CHANGE_STATE m_state;

	// ��ǰѡ�е�������
	IPatternLoopPtr m_pSelectedLoop;

	// �µ����е�λ�á�
	Point2D m_newPt;
	int m_iPatIndex;

	// ���������߷������
	LeadConfigItemPtr m_pLeadConfigItem;
};

END_CUTLEADER_NAMESPACE()
