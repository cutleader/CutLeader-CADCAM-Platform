#pragma once

#include "Point2D.h"
#include "CutFeatureEditBase.h"

DECLARE_CUTLEADER_CLASS(ToolAssignConfigItem)
DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ConditionTypeList)

BEGIN_CUTLEADER_NAMESPACE()

class ClCutFeatureUI_Export LoopToolEditAction : public CutFeatureEditBase
{
    enum PATLOOP_SELECT_STATE
    {
        NONE						    = 0,
        PATLOOP_SELECT_FIRST_POINT		= 1, // �������
        PATLOOP_SELECT_SECOND_POINT		= 2, // ���̧��
    };

public:
	LoopToolEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort,
                       ToolAssignConfigItemPtr pToolAssignConfigItem, LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList);
	~LoopToolEditAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
    virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return LOOPTOOL_EDIT; }
	virtual void UpdateForUndoRedo();

private:
    ToolAssignConfigItemPtr m_pToolAssignConfigItem;
	LeadConfigItemPtr m_pLeadConfigItem;
    ConditionTypeListPtr m_pCondTypeList;

    PATLOOP_SELECT_STATE m_iState;

    // ��ѡ�Ŀ��(����x������y������x������y)
	double m_boxData[4];

	// ����ƶ�ʱ�ĺ�ѡ������ֻ��һ����Ч��
	IPatternLoopPtr m_pActivePatternLoop; // ����ѡʱ�ġ���ǰ��������
	PatternLoopListPtr m_pActivePatternLoops_inRect; // ��ѡʱ�ġ���ǰ��������

    // ѡ�е����������̧��ʱ����
    PatternLoopListPtr m_pSelectedPatternLoops;
};

END_CUTLEADER_NAMESPACE()
