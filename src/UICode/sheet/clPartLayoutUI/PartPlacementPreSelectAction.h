#pragma once

#include "PartPlacementEditAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// this action will help select part pmts in some actions.
class PartPlacementPreSelectAction : public PartPlacementEditAction
{
protected:
	enum PARTPMT_PRE_SELECT_STATE
	{
		// ��û�п�ʼѡ��
		PRE_SELECT_NO_INPUT			= 0,

		// ���������¡�
		PRE_SELECT_LEFT_BTN_DOWN	= 1,

		// ������̧��
		PRE_SELECT_LEFT_BTN_UP		= 2,
	};

public:
	PartPlacementPreSelectAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, DistanceDetectDlg* pDisDetectDlg, GlViewPortPtr pViewPort);
	~PartPlacementPreSelectAction(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();

public:
    PartPlacementListPtr GetSelectedPrtPmts() const { return m_pSelectedPartPlacements; }

protected:
	PARTPMT_PRE_SELECT_STATE m_preSelectState;

	// the data for selection.
	/*
		LeftTopPointX;
		LeftTopPointY;
		RightBottomPointX;
		RightBottomPointY;
	*/
	double m_data[4];

    // ����ƶ���ʱ�������������ö���
    PartPlacementListPtr m_pActivePartPlacements;

    // ѡ�е�������á�
    PartPlacementListPtr m_pSelectedPartPlacements;

    // �����ܿ����������Ի���
    DistanceDetectDlg* m_pDisDetectDlg;
};

END_CUTLEADER_NAMESPACE()
