#pragma once

#include "Point2D.h"
#include "ObserverBase.h"
#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"
#include "SelectPartOrLoopActionBar.h"


DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ToolAssignConfigItem)
DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)


BEGIN_CUTLEADER_NAMESPACE()

class LoopInstanceSelectAction : public PartPlacementEditAction
{
    enum LOOPINST_SELECT_STATE
    {
        NONE						    = 0,
        LOOPINST_SELECT_FIRST_POINT		= 1, // �������
        LOOPINST_SELECT_SECOND_POINT	= 2, // ���̧��
    };

public:
	LoopInstanceSelectAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
        GlViewPortPtr pViewPort, SelectPartOrLoopActionBar* pSelectPartOrLoopActionBar);
	~LoopInstanceSelectAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
    virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_SelectLoopInstance; }

public:
    LoopInstanceListPtr GetSelectedLoopInstances() const { return m_pSelectedLoopInsts; }

private:
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

    SelectPartOrLoopActionBar* m_pSelectPartOrLoopActionBar;
};

END_CUTLEADER_NAMESPACE()
