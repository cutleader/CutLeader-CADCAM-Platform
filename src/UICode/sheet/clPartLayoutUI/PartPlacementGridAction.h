#pragma once

#include "PartPlacementEditAction.h"
#include "CreatePartGridDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

// 这个动作创建零件阵列。
class PartPlacementGridAction : public PartPlacementEditAction
{
	typedef enum tagDrawGridState
	{
		GRID_PARTPMT_NOTSTART			= 0,
		GRID_PARTPMT_PART_SELECTED		= 1, // 选择了零件，浮动对话框弹出了。
	} GRID_PARTPMT_STATE;

public:
	PartPlacementGridAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
					   GlViewPortPtr pViewPort, CreatePartGridDlg* pCreatePartGridDlg);
	~PartPlacementGridAction(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_GRIDPRTPMT; }
	virtual void UpdateForUndoRedo();

public:
    // 调用者确保传入的零件不是阵列零件。
    void CreateGridFromPartPlacement(PartPlacementPtr pPartPlacement);

private:
	GRID_PARTPMT_STATE m_iState;

	// 离光标最近的零件放置。
	PartPlacementPtr m_pActivePartPmt;

	// 创建出的零件阵列对象。
	// 注:
	//  1) 虽然是集合对象，里面最多只有一个元素。
	PartPlacementListPtr m_pGridPartPmts;

	// “定义零件阵列”对话框。
	CreatePartGridDlg* m_pCreatePartGridDlg;
};

END_CUTLEADER_NAMESPACE()
