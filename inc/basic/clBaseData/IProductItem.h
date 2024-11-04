#pragma once

#include "clBaseDataCommon.h"
#include "IData.h"

BEGIN_CUTLEADER_NAMESPACE()

// “产品项”接口。
// 注意：
//  1) we should init parent ID of the node.
class ClBaseData_Export IProductItem : virtual public IData
{
public:
	virtual CString GetName() const = 0;
	virtual void SetName(CString strName) = 0;

	virtual void SetComment(CString strComment) = 0;
	virtual CString GetComment() const = 0;

	// whether this ProductItem is a folder.
	virtual BOOL IsFolder() const = 0;

	// get the path of the item.
	virtual CString GetItemPath() const = 0;
	virtual void SetItemPath(CString strPath) = 0;

	virtual void SetCreateTime(LONGLONG iCreateTimeStamp) = 0;
	virtual LONGLONG GetCreateTime() const = 0;

	virtual void SetModifyTime(LONGLONG iModifyTimeStamp) = 0;
	virtual LONGLONG GetModifyTime() const = 0;
};

END_CUTLEADER_NAMESPACE()
