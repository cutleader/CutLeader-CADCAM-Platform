#pragma once

#include "clDataImportExportCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// this class defines the info of the export action.
// notes:
//   1) this data come from "ExpDataDialog".
class ClDataImportExport_Export ExportDataInfo
{
public:
	ExportDataInfo(void);
	ExportDataInfo(CString strExpFilePath, IMPEXP_DATA_TYPE expDataType);
	~ExportDataInfo(void);

public: // get/set functions.
	void SetExpFilePath(CString strExpFilePath) { m_strExpFilePath = strExpFilePath; }
	CString GetExpFilePath() const { return m_strExpFilePath; }

	void SetExpDataType(IMPEXP_DATA_TYPE expDataType) { m_expDataType = expDataType; }
	IMPEXP_DATA_TYPE GetExpDataType() const { return m_expDataType; }

public:
	// the path where the data will be exported.
	CString m_strExpFilePath;

	// the type of the data which will be exported. 
	IMPEXP_DATA_TYPE m_expDataType;
};

END_CUTLEADER_NAMESPACE()
