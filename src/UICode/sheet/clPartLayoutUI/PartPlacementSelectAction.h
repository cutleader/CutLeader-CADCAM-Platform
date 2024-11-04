#pragma once

#include "PartPlacementEditAction.h"
#include "PartPlacementMoveBase.h"
#include "SelectPartOrLoopActionBar.h"
#include "WrongToolSizeLoopInstances.h"


DECLARE_CUTLEADER_CLASS(PartPlacementSelectAction)


BEGIN_CUTLEADER_NAMESPACE()

// ѡ������Ķ����ࡣ
class PartPlacementSelectAction : public PartPlacementEditAction, public PartPlacementMoveBase
{
	enum PARTPMT_SELECT_STATE
	{
		PARTPMT_SELECT_NONE          = 0,
		PARTPMT_SELECT_FIRST_POINT   = 1, // �������
		PARTPMT_SELECT_SECOND_POINT  = 2  // ���̧��
	};

public:
	PartPlacementSelectAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
        GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg, SelectPartOrLoopActionBar* pSelectPartOrLoopActionBar);
	~PartPlacementSelectAction(void);

public:  // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual int GetActionType() const override { return EDIT_SELECTPRTPMT; }
	virtual BOOL DrawBeforeFinish();
	virtual void UpdateForUndoRedo();

public:
	// clear current selected part pmts and select "pPartPlacements".
	// notes:
	//   1) in "PartPlacementViewBar", if we select part pmt in tree, we should call this.
	void SelectPartPlacements(PartPlacementListPtr pPartPlacements, BOOL bShowPropBar = FALSE);
    void SelectPartPlacement(PartPlacementPtr pPartPlacement, BOOL bShowPropBar = FALSE);

    PartPlacementListPtr GetSelectedPrtPmts() const { return m_pSelectedPartPlacements; }

	// �Ƿ��ڽ��п���������
	BOOL IsCopying() const { return m_bCopying; }

    WrongToolSizeLoopInstancesPtr GetWrongToolSizeLoopInstances() const { return m_pWrongToolSizeLoopInstances; }

private:
	// delete selected part pmts.
	void DeleteSelectedPartPlacements();

	// �����ƶ�ѡ�е������
	void StepMove(POSITION_INFO iDir);

	// �����ƶ�ѡ�е������
	void RapidMove(POSITION_INFO iDir);

    // ����������õ���Ӿ��Ρ�
    void UpdateOuterRects_ofAllPartPlacements();

private:
	PARTPMT_SELECT_STATE m_state;

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


	/************************************************************************/	
	// �йؿ�ݿ�����

	// ����������������Ҫ���Ƶġ�������á�����
	// ע��
	//  1) ctrl+cʱ���á�
	PartPlacementListPtr m_pBasePartPlacements;

	// ctrl+vʱ���øñ�����
	BOOL m_bCopying;		
	/************************************************************************/


	// ����������õ���Χ����
	std::map<LONGLONG, Rect2D> m_rects_of_partPmt;

    // �����ܿ����������Ի���
    DistanceDetectDlg* m_pDisDetectDlg;

    SelectPartOrLoopActionBar* m_pSelectPartOrLoopActionBar;

    // ƫ��ʧ�ܵļ���������
    // ע��
    //  1������action��ɺ�Ҫ�ص����action���ڻص����action֮ǰ�û���Ҫ����Ҫ��Ҫ���ø�ֵ��
    WrongToolSizeLoopInstancesPtr m_pWrongToolSizeLoopInstances;
};

END_CUTLEADER_NAMESPACE()
