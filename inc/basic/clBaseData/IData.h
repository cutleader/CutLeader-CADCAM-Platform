#pragma once

#include "clBaseDataCommon.h"
#include "DataProperty.h"
#include "DataPropItem.h"

DECLARE_CUTLEADER_CLASS(IData)

BEGIN_CUTLEADER_NAMESPACE()


// this is the interface for all cutleader data, cutleader数据接口。
// notes:
//   1) about "IsModified":
//      a) if new an object, we should remember to call "SetModified(TRUE)".
//      b) after load object from database or save the object to db, remember to call "SetModified(FALSE)".
//      c) for other cases, if user call Set***()/Update***()/... functions, user should call "SetModified(TRUE)", so user should determine when we should call this.
//      d) you cannot check the modified status only from "m_bModified", e.g.
//           i) for PartItem object, if a Line Pattern is added, "PartItem::m_bModified" will not changed, so we also need to check the involved object.
//           ii) for PatternLoopNode object, we do not check "PatternLoopNode::m_patPtr", because if we changed the color of "PatternLoopNode::m_patPtr", we
//               do not think PatternLoopNode is modified.
//         so we should check the "composed" object, NOT "referenced" object. or you can have another criteria, the "composed" object will be saved with
//         "parent" object, BUT the "referenced" object will not.
//   2) which data will inherit from "IData"?
//      a) the data will be saved into database.
//      b) ID is useful for the data.
class ClBaseData_Export IData
{
public:
	virtual ~IData() {}

public:
	// all data object have a ID.
	virtual LONGLONG GetID() const = 0;
	virtual void SetID(LONGLONG iID) = 0;

	// notes:
	//   1) parent ID is not alway the ID of the super class.
	virtual void SetParentID(LONGLONG iParentID) = 0;
	virtual LONGLONG GetParentID() const = 0;

	virtual BOOL IsModified() const = 0;
	virtual void SetModified(BOOL bModified) = 0; // 设置为“已修改”，并记录修改时间。
    virtual LONGLONG GetLatestModifyTimeStamp() const = 0; // 得到“最新修改时间”

	// compare the object ID.
	virtual BOOL IDEqual(LONGLONG iID) const = 0;

	// the properties of the data.
	virtual DataProperty GetProperty() const = 0;
	virtual void UpdateProperty(DataPropItem& propItem) = 0;

	// 克隆数据对象。
	// 注：
	//   1) 有时数据对象的克隆函数只能完成部分工作，其他处理需要在外面完成。
	virtual IDataPtr Clone() const = 0;
};

END_CUTLEADER_NAMESPACE()
