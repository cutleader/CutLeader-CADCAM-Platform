#pragma once

#include "clDataManagerCommon.h"

DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(DataItem)
DECLARE_CUTLEADER_CLASS(ProductItemList)
DECLARE_CUTLEADER_CLASS(DependNodeList)

BEGIN_CUTLEADER_NAMESPACE()

// the interface for dependence node(a node in the dependence tree).
// notes:
//   1) the dependence relationship: if "a" depend on "b", so if "b" is deleted, "a" will be deleted also. 
//   2) we do not think nest job depend on sheet, if we delete a sheet within nest job, we only return the mat used by this sheet and update the modified date of the nest job.
//   3) now we think no data will depend on sheet.
class DependNode
{
public:
	DependNode(void);
	~DependNode(void);

public: // get/set functions.
	DataItemPtr GetDataItem() { return m_pDataItem; }
	void SetDataItem(DataItemPtr pDataItem) { m_pDataItem = pDataItem; }

	DependNodeListPtr GetDependNodeList() { return m_pDependNodeList; }

public: // other functions.
	/************************************************************************/
	// about the data items in the dependence tree.
	
	// get all IDs of data item.
	IntegerListPtr GetAllDataItemID();

	// get all data items in the dependence tree.
	ProductItemListPtr GetAllDataItem();
	/************************************************************************/

	// check whether there are some data items which depend on "m_pDataItem".
	BOOL HasDependence();

private:
	// you know, each node in dependence tree will stand for a data item.
	DataItemPtr m_pDataItem;

	// the nodes which depend on this node.
	// notes: allocate the memory inside the class.
	DependNodeListPtr m_pDependNodeList;
};

END_CUTLEADER_NAMESPACE()
