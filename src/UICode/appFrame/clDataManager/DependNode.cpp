#include "StdAfx.h"
#include "DependNode.h"

#include "IntegerList.h"

#include "DataItem.h"
#include "ProductItemBase.h"
#include "ProductItemList.h"
#include "DependNodeList.h"

BEGIN_CUTLEADER_NAMESPACE()

DependNode::DependNode(void)
{
	m_pDependNodeList.reset(new DependNodeList);
}

DependNode::~DependNode(void)
{

}

IntegerListPtr DependNode::GetAllDataItemID()
{
	IntegerListPtr pIntegerList(new IntegerList);
	pIntegerList->AddInt(m_pDataItem->GetID());

	for (unsigned int i = 0; i < m_pDependNodeList->size(); i++)
	{
		DependNodePtr pDependNode = m_pDependNodeList->at(i);
		IntegerListPtr pTmpIntegerList = pDependNode->GetAllDataItemID();
		pIntegerList->InsertIntList(pTmpIntegerList);
	}

	return pIntegerList;
}

ProductItemListPtr DependNode::GetAllDataItem()
{
	ProductItemListPtr pProductItemList(new ProductItemList);
	pProductItemList->push_back(m_pDataItem);

	for (unsigned int i = 0; i < m_pDependNodeList->size(); i++)
	{
		DependNodePtr pDependNode = m_pDependNodeList->at(i);
		ProductItemListPtr pTmpProductItemList = pDependNode->GetAllDataItem();

		//
		pProductItemList->insert(pProductItemList->end(), pTmpProductItemList->begin(), pTmpProductItemList->end());
	}

	return pProductItemList;
}

BOOL DependNode::HasDependence()
{
	if (m_pDependNodeList->size() > 0)
		return TRUE;

	return FALSE;
}

END_CUTLEADER_NAMESPACE()
