#pragma once

#include "Matrix2D.h"
#include "PartPlacementEditAction.h"
#include "PartPlacementMoveBase.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(PartItemList)
DECLARE_CUTLEADER_CLASS(PartPlacementAddAction2)

BEGIN_CUTLEADER_NAMESPACE()

// ��dxf���һ�������������ġ�
// ע��
//   1) �����һ�����ʱ��������������
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
	// ��dxf���ص�����
	PartItemListPtr m_pPartItemList;

	// ��Щ������Ƿ񱻱��档
	BOOL m_bPartItemSaved;

	// �����ı���·����
	FolderNodePtr m_pSaveFolder;

private:
    // �����ܿ����������Ի���
    DistanceDetectDlg* m_pDisDetectDlg;
};

END_CUTLEADER_NAMESPACE()
