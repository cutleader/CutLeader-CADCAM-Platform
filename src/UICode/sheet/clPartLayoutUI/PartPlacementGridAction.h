#pragma once

#include "PartPlacementEditAction.h"
#include "CreatePartGridDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

// �����������������С�
class PartPlacementGridAction : public PartPlacementEditAction
{
	typedef enum tagDrawGridState
	{
		GRID_PARTPMT_NOTSTART			= 0,
		GRID_PARTPMT_PART_SELECTED		= 1, // ѡ��������������Ի��򵯳��ˡ�
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
    // ������ȷ�����������������������
    void CreateGridFromPartPlacement(PartPlacementPtr pPartPlacement);

private:
	GRID_PARTPMT_STATE m_iState;

	// ���������������á�
	PartPlacementPtr m_pActivePartPmt;

	// ��������������ж���
	// ע:
	//  1) ��Ȼ�Ǽ��϶����������ֻ��һ��Ԫ�ء�
	PartPlacementListPtr m_pGridPartPmts;

	// ������������С��Ի���
	CreatePartGridDlg* m_pCreatePartGridDlg;
};

END_CUTLEADER_NAMESPACE()
