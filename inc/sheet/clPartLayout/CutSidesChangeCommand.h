#pragma once

#include <map>
#include <tuple>
#include "clCutFeatureCommon.h"
#include "clPartLayoutCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(LeadConfigItem)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(ILead)

BEGIN_CUTLEADER_NAMESPACE()

// this command can change the cut side of loops.
class ClPartLayout_Export CutSidesChangeCommand : public CommandBase
{
public:
    CutSidesChangeCommand(EditorData editorData, LoopInstanceListPtr pLoopInstList,
        const std::vector<std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>>& leadDataList_4_keepIt, LeadConfigItemPtr pLeadConfigItem);
	~CutSidesChangeCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	LoopInstanceListPtr m_pLoopInstList;

    std::vector<std::tr1::tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>> m_leadDataList_4_keepIt; // 来自SideAdjustAction类

	// 记录之前的cut side和引线信息，用于undo。<LONGLONG, LONGLONG>为<零件实例ID，轮廓ID>。
	std::map<std::pair<LONGLONG,LONGLONG>, PatternLoopCutSideType> m_oldCutsideData;
	std::map<std::pair<LONGLONG,LONGLONG>, ILeadPtr> m_oldLeadInData;
	std::map<std::pair<LONGLONG,LONGLONG>, ILeadPtr> m_oldLeadOutData;

	LeadConfigItemPtr m_pLeadConfigItem;
};

END_CUTLEADER_NAMESPACE()
