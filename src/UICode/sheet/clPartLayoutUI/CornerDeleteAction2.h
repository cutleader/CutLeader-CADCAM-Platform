#pragma once

#include "CornerPreSelectAction2.h"
#include "SpreadSamePartDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

// “删除角特征”动作。
class CornerDeleteAction2 : public CornerPreSelectAction2
{
public:
	CornerDeleteAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort, SpreadSamePartDlg* pSpreadSamePartDlg);
	~CornerDeleteAction2(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return PMT_CORNER_DELETE; }

public:
	// 选中这些角特征。
	// 注: 
	//   1) 这个函数类似鼠标选中的操作。
	void SelectCornerList(CornerListPtr pCornerList);

private:
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
