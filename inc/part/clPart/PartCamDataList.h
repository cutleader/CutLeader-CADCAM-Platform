#pragma once

#include "clPartCommon.h"
#include "ClData.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(PartCamData)
DECLARE_CUTLEADER_CLASS(PartCamDataList)

BEGIN_CUTLEADER_NAMESPACE()

class ClPart_Export PartCamDataList : public std::vector<PartCamDataPtr>,
									  public ClData
{
public:
	PartCamDataList(void);
	virtual ~PartCamDataList(void);

public: // implement IData interface
	virtual IDataPtr Clone() const override;
};

END_CUTLEADER_NAMESPACE()
