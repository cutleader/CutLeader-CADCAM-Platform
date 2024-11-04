#include "StdAfx.h"
#include "DataItemTracker.h"

#include "baseConst.h"

#include "ProductItemList.h"
#include "DataItem.h"

BEGIN_CUTLEADER_NAMESPACE()

DataItemTracker* DataItemTracker::m_pDataItemTracker = NULL;

DataItemTracker::DataItemTracker(void)
{
	m_pDataItemList.reset(new ProductItemList);
	m_iLastSelNodeID = INVALID_ID;
}

DataItemTracker::~DataItemTracker(void)
{
}

DataItemTracker* DataItemTracker::GetInstance()
{
	if (m_pDataItemTracker == NULL)
		m_pDataItemTracker = new DataItemTracker();

	return m_pDataItemTracker;
}

BOOL DataItemTracker::IsDataItemOpened(DataItemPtr pDataItem)
{
	BOOL bOpened = FALSE;

	if (m_pDataItemList->GetItemByDataID(pDataItem->GetDataID()))
		bOpened = TRUE;

	return bOpened;
}

BOOL DataItemTracker::IsDataItemOpened(LONGLONG iDataID)
{
	BOOL bOpened = FALSE;

	if (m_pDataItemList->GetItemByDataID(iDataID))
		bOpened = TRUE;

	return bOpened;
}

void DataItemTracker::AddDataItem(DataItemPtr pDataItem)
{
	m_pDataItemList->push_back(pDataItem);
}

void DataItemTracker::RemoveDataItem(LONGLONG iDataID)
{
	ProductItemPtr pProductItem = m_pDataItemList->GetItemByDataID(iDataID);
	if (pProductItem)
		m_pDataItemList->RemoveItem(pProductItem->GetID());
}

END_CUTLEADER_NAMESPACE()
