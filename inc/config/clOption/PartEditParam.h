#pragma once

#include "clOptionCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(ExtPropTypeList)

BEGIN_CUTLEADER_NAMESPACE()

class ClOption_Export PartEditParam : public ClData
{
public:
	PartEditParam(void);
	~PartEditParam(void);

public: // get/set functions.
	void SetExtPropTypeList(ExtPropTypeListPtr pExtPropTypes) { m_pExtPropTypes = pExtPropTypes; }
	ExtPropTypeListPtr GetExtPropTypeList() const { return m_pExtPropTypes; }

private:
	// 扩展的属性类型。
	ExtPropTypeListPtr m_pExtPropTypes;
};

END_CUTLEADER_NAMESPACE()
