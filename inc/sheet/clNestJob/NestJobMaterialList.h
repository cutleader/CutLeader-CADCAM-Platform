#pragma once

#include "clNestJobCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(NestJobMaterial)
DECLARE_CUTLEADER_CLASS(NestJobMaterialList)

BEGIN_CUTLEADER_NAMESPACE()

// the list of NestJobMaterial objects.
class ClNestJob_Export NestJobMaterialList : public std::vector<NestJobMaterialPtr>
{
public:
	NestJobMaterialList(void);
	~NestJobMaterialList(void);

public:
	NestJobMaterialPtr GetNestJobMat(LONGLONG iSheetID) const;
};

END_CUTLEADER_NAMESPACE()
