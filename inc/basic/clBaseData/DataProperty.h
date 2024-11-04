#pragma once

#include "clBaseDataCommon.h"
#include "DataPropItem.h"
#include <vector>

BEGIN_CUTLEADER_NAMESPACE()

class ClBaseData_Export DataProperty
{
public:
	DataProperty(void);
	~DataProperty(void);

public:
	/************************************************************************/
	// ���ڵõ����������

	DataPropItem GetItem(int index) { return m_propItems[index]; }

	// get the property items by the category type of the property item.
	void GetItemsByType(int iCategoryType, std::vector<DataPropItem>& propItems);

	// get the property item by item name.
	DataPropItem GetItemByName(int iItemName);
	/************************************************************************/

	int Count() { return m_propItems.size(); }
	void AddProperty(DataPropItem& propItem) { m_propItems.push_back(propItem); }

	// ���ǵ�ָ���ġ����������
	void SetItem(int iIndex, const DataPropItem& propItem);

private:
	// the property items.
	std::vector<DataPropItem> m_propItems;
};

END_CUTLEADER_NAMESPACE()
