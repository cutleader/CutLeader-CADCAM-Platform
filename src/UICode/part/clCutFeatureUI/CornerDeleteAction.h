#pragma once

#include "CornerPreSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

// ��ɾ����������������
class ClCutFeatureUI_Export CornerDeleteAction : public CornerPreSelectAction
{
public:
	CornerDeleteAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort);
	~CornerDeleteAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL LButtonUp(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return CORNER_DELETE; }

public:
	// ѡ����Щ��������
	// ע: 
	//   1) ��������������ѡ�еĲ�����
	void SelectCornerList(CornerListPtr pCornerList);
};

END_CUTLEADER_NAMESPACE()
