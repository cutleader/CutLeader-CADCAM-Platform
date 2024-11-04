#pragma once

#include "clBaseDataCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// the type of product node.
typedef enum tagProductNodeType
{
	PRODUCT_NODE_NOT_INIT	= 0,

	PRODUCT_NODE_PART		= 1,
	PRODUCT_NODE_SHEET		= 2,
	PRODUCT_NODE_NESTJOB	= 3,

	PRODUCT_NODE_FOLDER		= 4,

	// more than one node selected.
	PRODUCT_NODE_MUL_NODE	= 5,
} PRODUCT_NODE_TYPE;

// we keep this in the tree/list node, so we can "recognize" this tree/list node.
class ClBaseData_Export ProductNodeData
{
public:
	ProductNodeData();
	ProductNodeData(PRODUCT_NODE_TYPE nodeType, LONGLONG iNodeID);

	~ProductNodeData();

public: // get/set functions.
	void SetProductNodeType(PRODUCT_NODE_TYPE nodeType) { m_nodeType = nodeType; }
	PRODUCT_NODE_TYPE GetProductNodeType() { return m_nodeType; }

	void SetProductNodeID(LONGLONG iNodeID) { m_iNodeID = iNodeID; }
	LONGLONG GetProductNodeID() { return m_iNodeID; }

private:
	//
	PRODUCT_NODE_TYPE m_nodeType;

	// ID of product node.
	LONGLONG m_iNodeID;
};

END_CUTLEADER_NAMESPACE()
