#include "StdAfx.h"
#include "DependNodeMgr.h"

#include "baseConst.h"
#include "DBConnect.h"
#include "DataBaseManager.h"

#include "PartLoader.h"
#include "PartManager.h"
#include "Sheet.h"
#include "SheetLoader.h"

#include "NestJob.h"
#include "NestJobMaterialList.h"
#include "NestJobMaterial.h"
#include "NestJobLoader.h"

#include "ProductItemLoader.h"
#include "ProductItemList.h"
#include "ProductItemBase.h"
#include "FolderItem.h"
#include "DataItem.h"
#include "ProdNodeData.h"
#include "clDataManagerResource.h"
#include "DependNode.h"
#include "DependNodeList.h"

#include "IMaterialSize.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

DependNodePtr DependNodeMgr::GetDependTree(ProductItemListPtr pDataItemList, DataItemPtr pDataItem)
{
	DependNodePtr pDependNode(new DependNode);
	pDependNode->SetDataItem(pDataItem);

	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();

	vector<DataProperty> propList;
	if (pDataItem->GetDataType() == DATANODE_PART)
	{
		// check each sheet which depend on this part.
		SheetLoader::GetSheetInfoList(pMainDBCon, propList, pDataItem->GetDataID());
		for (unsigned int i = 0; i < propList.size(); i++)
		{
			DataProperty prop = propList[i];

			// get the ID of sheet.
			DataPropItem propItem = prop.GetItem(0);
			VARIANT v = propItem.GetValue();
			LONGLONG iSheetID = v.llVal;

			// get the ProductItem relative to this sheet.
			ProductItemPtr pProductItem = pDataItemList->GetItemByDataID(iSheetID);
			DataItemPtr pShtDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

			// get the dependence tree of the sheet.
			// notes: no data will depend on the sheet.
			DependNodePtr pTmpDependNode = GetDependTree(pDataItemList, pShtDataItem);

			// append "pTmpDependNode" to the dependence tree.
			DependNodeListPtr pDependNodeList = pDependNode->GetDependNodeList();
			pDependNodeList->push_back(pTmpDependNode);
		}
	}
	else if (pDataItem->GetDataType() == DATANODE_SHEET)
	{
		;
	}
	else if (pDataItem->GetDataType() == DATANODE_NESTJOB)
	{
		/************************************************************************/
		// check the sheet which depend on this nest job.

		// get all sheet within this nest job.
		NestJobMaterialListPtr pNestJobMats = NestJobLoader::LoadNestJobMat(pMainDBCon, pDataItem->GetDataID());
		for (unsigned int i = 0; i < pNestJobMats->size(); i++)
		{
			NestJobMaterialPtr pNestJobMat = pNestJobMats->at(i);

			// get the ProductItem relative to this sheet, we should check whether the sheet node has been deleted.
			ProductItemPtr pProductItem;
			SheetPtr pSheet = pNestJobMat->GetSheet();
			if (pSheet)
			{
				pProductItem = pDataItemList->GetItemByDataID(pSheet->GetID());
				if (!pProductItem)
					continue;
			}
			else
				continue;

			// get the dependence tree of the sheet.
			// notes: no data will depend on the sheet.
			DataItemPtr pShtDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
			DependNodePtr pTmpDependNode = GetDependTree(pDataItemList, pShtDataItem);

			// append "pTmpDependNode" to the dependence tree.
			DependNodeListPtr pDependNodeList = pDependNode->GetDependNodeList();
			pDependNodeList->push_back(pTmpDependNode);
		}
		/************************************************************************/
	}

	return pDependNode;
}

DependNodeListPtr DependNodeMgr::GetDependTree(ProductItemListPtr pAllDataItemList, FolderNodePtr pFolderNode)
{
	DependNodeListPtr pDependNodeList(new DependNodeList);

	// get all data items under "pDataItemList".
	ProductItemListPtr pDataItemList(new ProductItemList);
	pFolderNode->GetAllDataItem(pDataItemList);

	// get the dependence tree of each data item.
	for (unsigned int i = 0; i < pDataItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pDataItemList->at(i);
		DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		//
		DependNodePtr pDependNode = GetDependTree(pAllDataItemList, pDataItem);
		pDependNodeList->push_back(pDependNode);
	}

	return pDependNodeList;
}

DependNodeListPtr DependNodeMgr::GetDependTree(ProductItemListPtr pAllDataItemList, ProductItemListPtr pDataItemList)
{
	DependNodeListPtr pDependNodeList(new DependNodeList);

	// get the dependence tree of each data item.
	for (unsigned int i = 0; i < pDataItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pDataItemList->at(i);
		DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		//
		DependNodePtr pDependNode = GetDependTree(pAllDataItemList, pDataItem);
		pDependNodeList->push_back(pDependNode);
	}

	return pDependNodeList;
}

void DependNodeMgr::DelAllDataOnDpdTree(DependNodeListPtr pDependNodeList, BOOL bTransaction)
{
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	if (bTransaction)
	{
		pMainDBCon->BeginTransaction();
		pMatLibDBCon->BeginTransaction();
	}

	ProductItemListPtr pProductItemList = pDependNodeList->GetAllDataItem();
	for (unsigned int i = 0; i < pProductItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pProductItemList->at(i);
		DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		// delete this data.
		if (pDataItem->GetDataType() == DATANODE_PART)
		{
			PartManager::DeletePart(pDataItem->GetDataID(), FALSE);
		}
		else if (pDataItem->GetDataType() == DATANODE_SHEET)
		{
			// check whether the sheet is within a nest job.
			int iSheetCount = NestJobLoader::GetSheetCount(pMainDBCon, pDataItem->GetDataID());
			if (iSheetCount != INVALID_VALUE)
				SheetManager::DeleteSheet(pDataItem->GetDataID(), iSheetCount, FALSE);
			else
				SheetManager::DeleteSheet(pDataItem->GetDataID(), 1, FALSE);
		}
		else if (pDataItem->GetDataType() == DATANODE_NESTJOB)
		{
			NestJobMgr::DeleteNestJob(pDataItem->GetDataID(), FALSE);
		}
	}

	if (bTransaction)
	{
		pMainDBCon->CommitTransaction();
		pMatLibDBCon->CommitTransaction();
	}
}

void DependNodeMgr::DelDataOnDpdTree(DependNodePtr pDependNode, BOOL bTransaction)
{
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	if (bTransaction)
	{
		pMainDBCon->BeginTransaction();
		pMatLibDBCon->BeginTransaction();
	}

	ProductItemListPtr pProductItemList = pDependNode->GetAllDataItem();
	for (unsigned int i = 0; i < pProductItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pProductItemList->at(i);
		DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		// delete this data.
		if (pDataItem->GetDataType() == DATANODE_PART)
		{
			PartManager::DeletePart(pDataItem->GetDataID(), FALSE);
		}
		else if (pDataItem->GetDataType() == DATANODE_SHEET)
		{
			// check whether the sheet is within a nest job.
			int iSheetCount = NestJobLoader::GetSheetCount(pMainDBCon, pDataItem->GetDataID());
			if (iSheetCount != INVALID_VALUE)
				SheetManager::DeleteSheet(pDataItem->GetDataID(), iSheetCount, FALSE);
			else
				SheetManager::DeleteSheet(pDataItem->GetDataID(), 1, FALSE);
		}
		else if (pDataItem->GetDataType() == DATANODE_NESTJOB)
		{
			NestJobMgr::DeleteNestJob(pDataItem->GetDataID(), FALSE);
		}
	}

	if (bTransaction)
	{
		pMainDBCon->CommitTransaction();
		pMatLibDBCon->CommitTransaction();
	}
}

END_CUTLEADER_NAMESPACE()
