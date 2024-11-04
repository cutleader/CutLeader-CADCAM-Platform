#pragma once

#include "clCutFeatureCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(ICorner)
DECLARE_CUTLEADER_CLASS(LoopCornerData)

BEGIN_CUTLEADER_NAMESPACE()

// 增加“角特征”的命令。
class ClCutFeature_Export CornerAddCommand : public CommandBase
{
public:
	CornerAddCommand(EditorData editorData, LoopCornerDataPtr pLoopCorner, ICornerPtr pCorner);
	~CornerAddCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	LoopCornerDataPtr m_pLoopCorner;
	ICornerPtr m_pCorner;
};

END_CUTLEADER_NAMESPACE()
