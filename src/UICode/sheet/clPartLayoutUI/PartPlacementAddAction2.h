#pragma once

#include "Matrix2D.h"
#include "PartPlacementEditAction.h"
#include "PartPlacementMoveBase.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(PartItemList)
DECLARE_CUTLEADER_CLASS(PartPlacementAddAction2)

BEGIN_CUTLEADER_NAMESPACE()

// 从dxf添加一个或多个零件到板材。
// 注：
//   1) 当添加一个零件时可以连续操作。
class PartPlacementAddAction2 : public PartPlacementEditAction, public PartPlacementMoveBase
{
public:
	PartPlacementAddAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
					   PartItemListPtr pPartItemList, const std::map<LONGLONG, std::vector<Matrix2D>>& matrixData_of_sameShapePart, FolderNodePtr pSaveFolder, DistanceDetectDlg* pDisDetectDlg);
	~PartPlacementAddAction2(void);

public: // implement IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_ADDPARTPMT_EX; }
	virtual void UpdateForUndoRedo();

private:
	// 从dxf加载的零件项。
	PartItemListPtr m_pPartItemList;

	// 这些零件项是否被保存。
	BOOL m_bPartItemSaved;

	// 零件项的保存路径。
	FolderNodePtr m_pSaveFolder;

private:
    // “智能靠碰参数”对话框。
    DistanceDetectDlg* m_pDisDetectDlg;
};

END_CUTLEADER_NAMESPACE()
