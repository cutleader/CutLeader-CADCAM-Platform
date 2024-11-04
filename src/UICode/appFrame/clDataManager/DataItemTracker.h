#pragma once

#include "clDataManagerCommon.h"

DECLARE_CUTLEADER_CLASS(DataItem)
DECLARE_CUTLEADER_CLASS(ProductItemList)
DECLARE_CUTLEADER_CLASS(RecentDataList)

BEGIN_CUTLEADER_NAMESPACE()

// this class will keep the opened DataItems.
// notes:
//   1) when open/delete DataItems, we should query info from this class.
class ClDataManager_Export DataItemTracker
{
public:
	~DataItemTracker(void);

private:
	DataItemTracker(void);

public: // get/set functions.
	ProductItemListPtr GetOpeningData() { return m_pDataItemList; }

	void SetRecentData(RecentDataListPtr pRecentDataList) { m_pRecentDataList = pRecentDataList; }
	RecentDataListPtr GetRecentData() { return m_pRecentDataList; }

	LONGLONG GetLastSelNodeID() { return m_iLastSelNodeID; }
	void SetLastSelNodeID(LONGLONG iLastSelNodeID) { m_iLastSelNodeID = iLastSelNodeID; }

public:
	static DataItemTracker* GetInstance();

public:
	/************************************************************************/
	// check whether the data is opened.

	// check whether "pDataItem" is opened.
	BOOL IsDataItemOpened(DataItemPtr pDataItem);

	BOOL IsDataItemOpened(LONGLONG iDataID);
	/************************************************************************/

	// when open a data item, add it to this class.
	void AddDataItem(DataItemPtr pDataItem);

	// remove the opened data item.
	void RemoveDataItem(LONGLONG iDataID);

private:
	// the singleton object
	static DataItemTracker* m_pDataItemTracker;

	// the opened DataItems.
	ProductItemListPtr m_pDataItemList;

	// the data which opened recently.
	RecentDataListPtr m_pRecentDataList;

	// the last selected node.
	LONGLONG m_iLastSelNodeID;
};

END_CUTLEADER_NAMESPACE()
