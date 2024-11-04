#pragma once

#include "CutFeatureEditBase.h"

DECLARE_CUTLEADER_CLASS(ParamConfig)

BEGIN_CUTLEADER_NAMESPACE()

// 给回路添加“微连接”特征。
class ClCutFeatureUI_Export MicroJointAddAction : public CutFeatureEditBase
{
	enum ADD_MICROJOINT_ACTION_STATE
	{
		ADD_MICROJOINT_STATE_NONE			= 0,
		ADD_MICROJOINT_STATE_SELECTLOOP		= 1,
	};

public:
	MicroJointAddAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort, ParamConfigPtr pParamConfig);
	~MicroJointAddAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);
	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return MicroJoint_ADD; }

private:
    // 根据选中的轮廓和当前光标点，得到一个新的微连接点。
    void UpdateNewPosition(const Point2D& cursorPos);

private:
	ADD_MICROJOINT_ACTION_STATE m_state;

	// when we select a loop, set this variable.
	IPatternLoopPtr m_pSelectedLoop;

	// 添加的微连接在回路上的位置。
	Point2DPtr m_pMicroJointPt;
	int m_iPatIndex;

	// 参数配置。
	ParamConfigPtr m_pParamConfig;
};

END_CUTLEADER_NAMESPACE()
