#pragma once

#include "SpreadSamePartDlg.h"
#include "MicroJointPreSelectAction2.h"

BEGIN_CUTLEADER_NAMESPACE()

// �ڰ����ɾ����΢�����������Ķ�����
class MicroJointDeleteAction2 : public MicroJointPreSelectAction2
{
public:
	MicroJointDeleteAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort, SpreadSamePartDlg* pSpreadSamePartDlg);
	virtual ~MicroJointDeleteAction2(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return PMT_MJ_DELETE; }

public:
	// ѡ����Щ΢����������
	// ע: 
	//   1) ��������������ѡ�еĲ�����
	void SelectMJList(MicroJointListPtr pMicroJointList);

private:
	SpreadSamePartDlg* m_pSpreadSamePartDlg;
};

END_CUTLEADER_NAMESPACE()
