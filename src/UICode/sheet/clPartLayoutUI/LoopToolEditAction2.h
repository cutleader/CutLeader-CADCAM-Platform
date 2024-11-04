#pragma once

#include "Point2D.h"
#include "ObserverBase.h"
#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"
#include "WrongToolSizeLoopInstances.h"


DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ToolAssignConfigItem)
DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)


BEGIN_CUTLEADER_NAMESPACE()

class LoopToolEditAction2 : public PartPlacementEditAction, public ObserverBase
{
    enum LOOPINST_SELECT_STATE
    {
        NONE						    = 0,
        LOOPINST_SELECT_FIRST_POINT		= 1, // �������
        LOOPINST_SELECT_SECOND_POINT	= 2, // ���̧��
    };

public:
	LoopToolEditAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
                       ToolAssignConfigItemPtr pToolAssignConfigItem, LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList, SpreadSamePartDlg* pSpreadSamePartDlg);
	~LoopToolEditAction2(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
    virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return LOOPTOOL_ADJUST; }
	virtual void UpdateForUndoRedo();

public: // implement IObserver interface.
    virtual void Update(int iEventID, VARIANT varEventContent) override;

public:
    void EditToolsOfSelectedLoopInstances(LoopInstanceListPtr pSelectedLoopInstances);

private:
    void DisplayLoopToolInfo();

private:
    ToolAssignConfigItemPtr m_pToolAssignConfigItem;
	LeadConfigItemPtr m_pLeadConfigItem;
    ConditionTypeListPtr m_pCondTypeList;

    LOOPINST_SELECT_STATE m_iState;

    // ��ѡ�Ŀ��(����x������y������x������y)
	double m_boxData[4];

	// ����ƶ�ʱ�ĺ�ѡ������ֻ��һ����Ч��
	LoopInstancePtr m_pActiveLoopInstance; // ����ѡʱ�ġ���ǰ��������
	LoopInstanceListPtr m_pActiveLoopInsts_inRect; // ��ѡʱ�ġ���ǰ��������

    // ѡ�е����������̧��ʱ����
    LoopInstanceListPtr m_pSelectedLoopInsts;

	// �������еġ���·ʵ������
	LoopInstanceListPtr m_pAllLoopInstances;

    // ��������ͬ������Ի���
    SpreadSamePartDlg* m_pSpreadSamePartDlg;

    // ��ÿ�����������ĵ��߳ߴ�ʱ����ƫ���������������¼�����
    // ע��
    //  1) �ڸ�action�£�ÿ����һ�ε��߳ߴ綼Ҫ����������ϡ��뿪�����action����Ϣ�ͱ�����ˡ�
    WrongToolSizeLoopInstancesPtr m_pWrongToolSizeLoopInstances;
};

END_CUTLEADER_NAMESPACE()
