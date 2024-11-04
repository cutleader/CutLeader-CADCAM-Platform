#pragma once

#include "Point2D.h"
#include "MicroJointPreSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// ��ɾ��΢���ӡ�������
class ClCutFeatureUI_Export MicroJointDelAction : public MicroJointPreSelectAction
{
public:
	MicroJointDelAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort);
	~MicroJointDelAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return MicroJoint_DELETE; }

public:
	// ѡ����Щ΢����������
	// ע: 
	//   1) ��������������ѡ�еĲ�����
	void SelectMJList(MicroJointListPtr pMicroJointList);
};

END_CUTLEADER_NAMESPACE()
