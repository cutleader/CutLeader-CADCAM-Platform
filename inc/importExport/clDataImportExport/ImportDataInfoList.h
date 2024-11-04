#pragma once

#include "clDataImportExportCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(ImportDataInfo)

BEGIN_CUTLEADER_NAMESPACE()

// all the data which will be imported.
class ClDataImportExport_Export ImportDataInfoList : public std::vector<ImportDataInfoPtr>
{
public:
	ImportDataInfoList(void);
	~ImportDataInfoList(void);
};

END_CUTLEADER_NAMESPACE()
