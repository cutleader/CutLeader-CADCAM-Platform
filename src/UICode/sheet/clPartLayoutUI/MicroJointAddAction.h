#pragma once

#include "clCutFeatureCommon.h"
#include "PartPlacementEditAction.h"
#include "SpreadSamePartDlg.h"

DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(LoopInstance)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// ����·��ӡ�΢���ӡ�������
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
    // ����ѡ�е������͵�ǰ���㣬�õ�һ���µ�΢���ӵ㡣
    void UpdateNewPosition(const Point2D& cursorPos);

private:
	ADD_MICROJOINT_ACTION_STATE m_state;

	// ���еġ���·ʵ������
	LoopInstanceListPtr m_pAllLoopInstanceList;

	// ��ǰ����·ʵ������
	LoopInstancePtr m_pActiveLoopInstance;

	// ѡ�еġ���·ʵ������
	LoopInstancePtr m_pSelectedLoopInst;

	// ��ӵ�΢�����ڻ�·�ϵ�λ�á�
	Point2DPtr m_pMicroJointPt; // ����������ģʽ�µ����ꡣ
	int m_iPatIndex;

	// �������á�
	ParamConfigPtr m_pParamConfig;

	// ��������ͬ������Ի���
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
