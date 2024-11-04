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

	// ��һ��ѡ��һ������ʱ����¼�����������Ϣ�����л��Ĺ�������Ҫ���֣�����������л�Ϊcuton�����л�������ʱ����Ҫ��������ʱ������
    // <����ʹ�õ������з�ʽ�������ߣ������ߣ������߳��ȣ������߳���>
	std::map<LONGLONG, std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>> m_leadDataList_4_keepIt;

	// ���������߷������
	LeadConfigItemPtr m_pLeadConfigItem;
};

END_CUTLEADER_NAMESPACE()
