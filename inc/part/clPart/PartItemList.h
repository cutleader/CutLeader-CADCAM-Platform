#pragma once

#include "clPartCommon.h"
#include "Rect2D.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(PartItem)
DECLARE_CUTLEADER_CLASS(PartItemList)

BEGIN_CUTLEADER_NAMESPACE()

// the PartItem list.
class ClPart_Export PartItemList : public std::vector<PartItemPtr>
{
public:
	PartItemList(void);
	~PartItemList(void);

public:
	// get index of the PartItem.
	int GetPartItemIndex(const PartItem* pPartItem) const;

	// get the part item by part ID and cam data ID.
	PartItemPtr GetPartItem(LONGLONG iPartId, LONGLONG iPartCamDataId) const;
};

END_CUTLEADER_NAMESPACE()
