#pragma once

#include "clCutFeatureCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(LoopMicroJoint)
DECLARE_CUTLEADER_CLASS(MicroJoint)

BEGIN_CUTLEADER_NAMESPACE()

// 增加“微连接特征”的命令。
class ClCutFeature_Export MicroJointAddCommand : public CommandBase
{
public:
	MicroJointAddCommand(EditorData editorData, LoopMicroJointPtr pLoopMicroJoint, MicroJointPtr pMicroJoint);
	~MicroJointAddCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	LoopMicroJointPtr m_pLoopMicroJoint;
	MicroJointPtr m_pMicroJoint;
};

END_CUTLEADER_NAMESPACE()
