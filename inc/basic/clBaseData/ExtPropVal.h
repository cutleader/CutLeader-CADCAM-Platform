#pragma once

#include "ClData.h"

DECLARE_CUTLEADER_CLASS(ExtPropType)

BEGIN_CUTLEADER_NAMESPACE()

// ��չ����ֵ��
class ClBaseData_Export ExtPropVal : public ClData
{
public:
	ExtPropVal(void);
	ExtPropVal(ExtPropTypePtr pExtPropType, VARIANT val);
	~ExtPropVal(void);

public: // get/set functions.
	void SetExtPropType(ExtPropTypePtr pExtPropType) { m_pExtPropType = pExtPropType; }
	ExtPropTypePtr GetExtPropType() const { return m_pExtPropType; }

	void SetVal(VARIANT val) { m_val = val; }
	VARIANT GetVal() const { return m_val; }

public:
	void SetVal(const char* pStrVal, BOOL bForDisplay = FALSE);

	// if bForDisplay=TRUE BOOL will be converted to "Yes/No", otherwise BOOL will be converted to "1/0".
	CString GetStrValue(BOOL bForDisplay) const;

private:
	// �������͡�
	ExtPropTypePtr m_pExtPropType;

	// ֵ��
	VARIANT m_val;
};

END_CUTLEADER_NAMESPACE()