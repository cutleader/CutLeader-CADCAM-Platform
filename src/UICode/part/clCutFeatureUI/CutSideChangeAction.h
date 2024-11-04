#pragma once

#include "clCutFeatureCommon.h"
#include <map>
#include <tuple>
#include "Point2D.h"
#include "CutFeatureEditBase.h"

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(ILead)

BEGIN_CUTLEADER_NAMESPACE()

// this action can change the cut side of the loop.
// notes:
//   1) the work flow:
//        a) when moving mouse, the "nearest to cursor" loop will be highlighted.
//        b) after a click, the cut side of the loop will be changed.
class ClCutFeatureUI_Export CutSideChangeAction : public CutFeatureEditBase
{
	enum CUTSIDE_CHANGE_STATE
	{
		CUTSIDE_CHANGE_NONE				= 0,
		CUTSIDE_CHANGE_SELECT_LOOPINST	= 1,
	};

public:
	CutSideChangeAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort,
		LeadConfigItemPtr pLeadConfigItem);
	~CutSideChangeAction(void);

public: // implements IInteract interface.
	virtual BOOL LButtonDown(double& dPtX, double& dPtY);
	virtual BOOL MovePoint(double& dPtX, double& dPtY);

	virtual BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

public: // implement IAction interface.
	virtual BOOL DrawBeforeFinish();
	virtual int GetActionType() const override { return CUTSIDE_CHANGE; }

private:
	CUTSIDE_CHANGE_STATE m_state;

	// 第一次选中一个轮廓时，记录上面的引线信息。在切换的过程中需要保持，比如把外切切换为cuton，再切换到内切时，需要保留外切时的引线
    // <现在使用的内外切方式，引入线，引出线，引入线长度，引出线长度>
	std::map<LONGLONG, std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>> m_leadDataList_4_keepIt;

	// 引入引出线分配规则。
	LeadConfigItemPtr m_pLeadConfigItem;
};

END_CUTLEADER_NAMESPACE()
