#pragma once

#include "PartPlacementEditAction.h"
#include "PartPlacementMoveBase.h"
#include "SelectPartOrLoopActionBar.h"
#include "WrongToolSizeLoopInstances.h"


DECLARE_CUTLEADER_CLASS(PartPlacementSelectAction)


BEGIN_CUTLEADER_NAMESPACE()

// 选择零件的动作类。
class PartPlacementSelectAction : public PartPlacementEditAction, public PartPlacementMoveBase
{
	enum PARTPMT_SELECT_STATE
	{
		PARTPMT_SELECT_NONE          = 0,
		PARTPMT_SELECT_FIRST_POINT   = 1, // 左键按下
		PARTPMT_SELECT_SECOND_POINT  = 2  // 左键抬起
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

	// 是否在进行拷贝动作。
	BOOL IsCopying() const { return m_bCopying; }

    WrongToolSizeLoopInstancesPtr GetWrongToolSizeLoopInstances() const { return m_pWrongToolSizeLoopInstances; }

private:
	// delete selected part pmts.
	void DeleteSelectedPartPlacements();

	// 步进移动选中的零件。
	void StepMove(POSITION_INFO iDir);

	// 快速移动选中的零件。
	void RapidMove(POSITION_INFO iDir);

    // 更新零件放置的外接矩形。
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

	// 鼠标移动的时候高亮的零件放置对象。
	PartPlacementListPtr m_pActivePartPlacements;

    // 选中的零件放置。
    PartPlacementListPtr m_pSelectedPartPlacements;


	/************************************************************************/	
	// 有关快捷拷贝。

	// 拷贝缓冲区，保存要复制的“零件放置”对象。
	// 注：
	//  1) ctrl+c时设置。
	PartPlacementListPtr m_pBasePartPlacements;

	// ctrl+v时设置该变量。
	BOOL m_bCopying;		
	/************************************************************************/


	// 缓存零件放置的外围矩形
	std::map<LONGLONG, Rect2D> m_rects_of_partPmt;

    // “智能靠碰参数”对话框。
    DistanceDetectDlg* m_pDisDetectDlg;

    SelectPartOrLoopActionBar* m_pSelectPartOrLoopActionBar;

    // 偏移失败的几何轮廓。
    // 注：
    //  1）不少action完成后都要回到这个action，在回到这个action之前用户需要决定要不要设置该值。
    WrongToolSizeLoopInstancesPtr m_pWrongToolSizeLoopInstances;
};

END_CUTLEADER_NAMESPACE()
