#pragma once

#include "IData.h"

DECLARE_CUTLEADER_CLASS(ClData)

BEGIN_CUTLEADER_NAMESPACE()

// the basic class of all type data, cutleader���ݻ��ࡣ
// ע��
//  1) ������������Ӧ����DataBase��Ϊ�˱�������ݿ��������������ΪClData��
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

    // �����޸�ʱ�䡣
    // ע��
    // 1�����������BusinessData�еĲ�һ��������������ǵ�����ClData��ÿ���޸ġ�ֻҪ����ClData�ġ����޸ġ����ʱ����Ҫ�������ֵ��
    //    �Ӵ��ĵ����ر��ĵ����ʱ�䷶Χ�ڣ�m_bModified���ܻ᲻�ϱ�����Ϊtrue��false��ֻ����Ϊtrueʱ�Ÿ������ʱ�����
    // 2) ���ʱ�����һֱ��һ���������ֵ��
    // 3) Ϊ��֧��ʱ�����ܽ����޸ģ����ʱ��������Ǻ��뼶�ġ�
    LONGLONG m_iLatestModifyTimeStamp;
};

END_CUTLEADER_NAMESPACE()
