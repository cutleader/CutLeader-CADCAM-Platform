#pragma once

#include "clBaseDataCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(HelpVideo)

BEGIN_CUTLEADER_NAMESPACE()

class ClBaseData_Export HelpVideoCategory : public std::vector<HelpVideoPtr>
{
public:
	HelpVideoCategory(void);
	~HelpVideoCategory(void);

public: // get/set functions.
	void SetName(CString strName) { m_strName = strName; }
	CString GetName() { return m_strName; }

private:
	// the video category name.
	CString m_strName;
};

END_CUTLEADER_NAMESPACE()
