#pragma once

#include "PartPlacementEditAction.h"

DECLARE_CUTLEADER_CLASS(PartInstanceSelectAction)

BEGIN_CUTLEADER_NAMESPACE()

// ѡ�����ʵ����action��
class PartInstanceSelectAction : public ActionBase
{
public:
	PartInstanceSelectAction(GlViewPortPtr pViewPort);
	~PartInstanceSelectAction(void);

public:  // implement IInteract interface.
	BOOL LButtonDown(double& dPtX, double& dPtY);
	BOOL LButtonUp(double& dPtX, double& dPtY);
	BOOL MovePoint(double& dPtX, double& dPtY);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return EDIT_SELECTPRTINST; }

public:
	// clear current selected part inst and select "pPartInstances".
	// notes:
	//   1) in "PartTopologyBar", if we select part inst in tree, we should call this.
	void SelectPartInsts(PartInstanceListPtr pPartInstances) { m_pSelPartInstanceList = pPartInstances; }

private:
	PartInstanceListPtr m_pSelPartInstanceList;
};

END_CUTLEADER_NAMESPACE()
