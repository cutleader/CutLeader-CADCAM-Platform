#pragma once

#include "clPartLayoutCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// the command will add a grid type part placement, the behavior is like "modify pattern" command.
// notes:
//   1) we should not delete the old not-grid partpmt, you know, we may already have some CAM data(pierce info/...) for old partpmt.
//   2) we should also update the part instance list in sheet, so the existed sequence data(loop instance order) will be lost.
class ClPartLayout_Export PartPlacementGridCommand : public CommandBase
{
public:
	PartPlacementGridCommand(EditorData editorData, PartPlacementListPtr partPmtListPtr, PartInstanceListPtr partInstListPtr, PartPlacementPtr oldPmtPtr, PartPlacementPtr gridPmtPtr);
	~PartPlacementGridCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	PartPlacementListPtr m_partPmtListPtr;
	PartInstanceListPtr m_partInstListPtr;

	PartPlacementPtr m_oldPmtPtr;
	PartPlacementPtr m_newPmtPtr;
};

END_CUTLEADER_NAMESPACE()
