#pragma once

#include "clPartLayoutCommon.h"
#include "CommandBase.h"

DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// the command will change the prop of part placement.
class ClPartLayout_Export PartPlacementPropertyEditCommand : public CommandBase
{
public:
	PartPlacementPropertyEditCommand(EditorData editorData, PartPlacementPtr pPartPlacement, PartInstanceListPtr pPartInstances, DataPropItem* pNewPropItem, DataPropItem* pOldPropItem);
	~PartPlacementPropertyEditCommand(void);

public:
	virtual void Do();
	virtual void UnDo();

private:
	PartPlacementPtr m_pPartPlacement;

	DataPropItem* m_pNewPropItem;
	DataPropItem* m_pOldPropItem;

	PartInstanceListPtr m_pPartInstList;
};

END_CUTLEADER_NAMESPACE()