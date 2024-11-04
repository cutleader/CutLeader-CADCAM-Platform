#pragma once

#include "IData.h"

DECLARE_CUTLEADER_CLASS(ClData)

BEGIN_CUTLEADER_NAMESPACE()

// the basic class of all type data, cutleader数据基类。
// 注：
//  1) 按照命名规则应该是DataBase，为了避免和数据库混淆，所以命名为ClData。
class ClBaseData_Export ClData : virtual public IData
{
public:
	ClData(void);
	virtual ~ClData(void);

public: // implements IData interface.
	virtual LONGLONG GetID() const override { return m_iID; }
	virtual void SetID(LONGLONG iID) override { m_iID = iID; }

	virtual void SetParentID(LONGLONG iParentID) override { m_iParentID = iParentID; }
	virtual LONGLONG GetParentID() const override { return m_iParentID; }

	virtual BOOL IsModified() const override { return m_bModified; }
	virtual void SetModified(BOOL bModified) override;
    virtual LONGLONG GetLatestModifyTimeStamp() const override { return m_iLatestModifyTimeStamp; }

	virtual BOOL IDEqual(LONGLONG iID) const override { return m_iID == iID ? TRUE : FALSE; }

	// implement this two functions here, maybe some class do not need to implement them.
	virtual DataProperty GetProperty() const override;
	virtual void UpdateProperty(DataPropItem& propItem) override;

	// provide a default implement here, actually it's meaningless.
	virtual IDataPtr Clone() const override;

protected:
	// the unique ID of the object.
	// notes:
	//   1) the legal ID range is in [0, 2^63-1], this is the position section of long long type.
	//   2) for invalid ID, -1 will be saved.
	LONGLONG m_iID;

	// the parent ID of the object.
	// notes:
	//   1) actually not all data inherit from this class should set this variable. BUT we should set this variable in such cases,
	//      e.g., Pattern/PartCadData, PartCadData/Part...
	//   2) programmer MUST comment in the class if the class will set this variable.
	LONGLONG m_iParentID;

	// whether this object has been modified.
	BOOL m_bModified;

    // 最新修改时间。
    // 注：
    // 1）这个变量和BusinessData中的不一样，这个变量覆盖到所有ClData的每次修改。只要设置ClData的“已修改”标记时，就要更新这个值。
    //    从打开文档到关闭文档这个时间范围内，m_bModified可能会不断被设置为true和false，只有设为true时才更新这个时间戳。
    // 2) 这个时间戳会一直是一个有意义的值。
    // 3) 为了支持时间间隔很近的修改，这个时间戳必须是毫秒级的。
    LONGLONG m_iLatestModifyTimeStamp;
};

END_CUTLEADER_NAMESPACE()
