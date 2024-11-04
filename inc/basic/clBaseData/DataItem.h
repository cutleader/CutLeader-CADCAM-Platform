#pragma once

#include "ProductItemBase.h"

DECLARE_CUTLEADER_CLASS(DataItem)

BEGIN_CUTLEADER_NAMESPACE()

// the type of data node.
typedef enum tagDataNodeType
{
	DATANODE_PART		= 0,
	DATANODE_SHEET		= 1,
	DATANODE_NESTJOB	= 2,

} DATANODE_TYPE;

// “数据项”对象。
class ClBaseData_Export DataItem : public ProductItemBase
{
public:
	DataItem(void);
	DataItem(LONGLONG iDataID, LONGLONG iParentID, DATANODE_TYPE dataType, CString strName, LONGLONG iCreateTime, LONGLONG iModifyTime);
	virtual ~DataItem();

public: // implement interface IProductItem.
	virtual BOOL IsFolder() const override { return FALSE; }

public: // get/set functions.
	void SetDataID(LONGLONG iDataID) { m_iDataID = iDataID; }
	LONGLONG GetDataID() const { return m_iDataID; }

	void SetDataType(DATANODE_TYPE dataType) { m_dataType = dataType; }
	DATANODE_TYPE GetDataType() const { return m_dataType; }

private:
	// the data ID. e.g. the ID of part/sheet/...
	LONGLONG m_iDataID;

	// the type of the data.
	DATANODE_TYPE m_dataType;
};

END_CUTLEADER_NAMESPACE()
