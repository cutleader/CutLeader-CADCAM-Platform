#pragma once

#include "dbWrapperCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// field data within a record.
class ClDbWrapper_Export DBField
{
public:
	DBField(void);
	~DBField(void);

public: // get/set functions.
	void SetFieldName(CString strFieldName) { m_strFieldName = strFieldName; }
	CString GetFieldName() { return m_strFieldName; }

	void SetFieldVal(VARIANT var) { m_val = var; }
	VARIANT GetFieldVal() { return m_val; }

private:
	// the name of one column.
	CString m_strFieldName;

	VARIANT m_val;
};

END_CUTLEADER_NAMESPACE()
