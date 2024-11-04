#pragma once

#include "clCutFeatureCommon.h"
#include "CommandBase.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(MicroJointList)

BEGIN_CUTLEADER_NAMESPACE()

// 修改“微连接引入/引出线长度”的命令。
class ClCutFeature_Export MicroJointLeadLengthModifyCommand : public CommandBase
{
public:
	MicroJointLeadLengthModifyCommand(EditorData editorData, MicroJointListPtr pMicroJointList, BOOL bModLeadIn, double dNewLen);
	~MicroJointLeadLengthModifyCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	// 要修改的微连接特征。
	MicroJointListPtr m_pMicroJointList;

	// 修改引入还是引出。
	BOOL m_bModLeadIn;

	// 新的长度。
	double m_dNewLen;

	// 老的长度。
	std::vector<double> m_dOldLenList;
};

END_CUTLEADER_NAMESPACE()
