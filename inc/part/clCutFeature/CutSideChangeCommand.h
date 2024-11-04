#pragma once

#include "clCutFeatureCommon.h"
#include "CommandBase.h"
#include <tuple>

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(LoopStartCutData)
DECLARE_CUTLEADER_CLASS(ILead)

BEGIN_CUTLEADER_NAMESPACE()

// this command can change the cut side of the loop.
class ClCutFeature_Export CutSideChangeCommand : public CommandBase
{
public:
    CutSideChangeCommand(EditorData editorData, LoopStartCutDataPtr pLoopStartCutData,
        const std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>& leadData_4_keepIt, LeadConfigItemPtr pLeadConfigItem);
	~CutSideChangeCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	LoopStartCutDataPtr m_pLoopStartCutData;

	std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double> m_leadData_4_keepIt; // 来自SideChangeAction类

	// 记录之前的cut side和引线信息，用于undo
	PatternLoopCutSideType m_iOldCutside;
	ILeadPtr m_pOldLeadIn;
	ILeadPtr m_pOldLeadOut;

	LeadConfigItemPtr m_pLeadConfigItem;
};

END_CUTLEADER_NAMESPACE()
