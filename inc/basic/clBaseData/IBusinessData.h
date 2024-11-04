#pragma once

#include "clBaseDataCommon.h"
#include "IData.h"

BEGIN_CUTLEADER_NAMESPACE()


typedef enum tagBusinessDataType
{
	BUSINESS_DATA_PART			= 200,
	BUSINESS_DATA_SHEET			= 201,
	BUSINESS_DATA_NESTJOB		= 202,
	BUSINESS_DATA_MAC_ITEM		= 203,
	BUSINESS_DATA_EXPLIB_ITEM	= 204,
	BUSINESS_DATA_MATLIB_TYPE	= 205,
	BUSINESS_DATA_REPORT_TPL	= 206,
} BUSINESS_DATA_TYPE;


// the interface for business data, 业务对象的接口。
class ClBaseData_Export IBusinessData : virtual public IData
{
public:
	virtual ~IBusinessData() {}

public:
	virtual BUSINESS_DATA_TYPE GetBusinessDataType() const = 0;

	virtual void SetName(CString strName) = 0;
	virtual CString GetName() const = 0;

	virtual void SetComment(CString strComment) = 0;
	virtual CString GetComment() const = 0;

	virtual void SetCreateTime(LONGLONG iCreateTimeStamp) = 0;
	virtual LONGLONG GetCreateTime() const = 0;

	virtual void SetModifyTime(LONGLONG iModifyTimeStamp) = 0;
	virtual LONGLONG GetModifyTime() const = 0;
};

END_CUTLEADER_NAMESPACE()
