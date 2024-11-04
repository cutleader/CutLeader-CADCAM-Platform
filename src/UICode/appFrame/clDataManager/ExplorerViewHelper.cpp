#include "StdAfx.h"
#include "ExplorerViewHelper.h"

#include "baseConst.h"
#include "DBConnect.h"
#include "taoresource.h"
#include "IntegerList.h"
#include "DateHelper.h"
#include "ProgressData.h"
#include "ProgressDlg.h"
#include "clSecureResource.h"

#include "LogMessage.h"
#include "LogMessageList.h"
#include "LogMgr.h"
#include "clBaseDataResource.h"
#include "ProductFolderManager.h"
#include "ProductFileManager.h"
#include "DataBaseManager.h"

#include "ProductItemLoader.h"
#include "ProductItemList.h"
#include "ProductItemBase.h"
#include "FolderItem.h"
#include "DataItem.h"
#include "ProdNodeData.h"

#include "PartCadData.h"
#include "PartCamData.h"
#include "PartCamDataList.h"
#include "Part.h"
#include "PartItem.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "PartInfoDlg.h"
#include "PartBuilder.h"
#include "PartManager.h"

#include "Sheet.h"
#include "SheetLoader.h"
#include "SheetWriter.h"
#include "SheetManager.h"
#include "SheetProcessor.h"
#include "SheetInfoDlg.h"

#include "NestJob.h"
#include "NestJobMaterialList.h"
#include "NestJobMaterial.h"
#include "NestScheduleDlg.h"
#include "NestJobLoader.h"
#include "NestJobWriter.h"
#include "NestJobMgr.h"

#include "ImportDataInfo.h"
#include "ImportDataInfoList.h"
#include "ExportDataInfo.h"
#include "clDataImportExportResource.h"
#include "ImportManager.h"
#include "ExportManager.h"
#include "ExpDataDialog.h"

#include "clDataManagerResource.h"
#include "FolderInfoDlg.h"
#include "ProductItemWriter.h"
#include "DependNodeMgr.h"
#include "DependMsgDlg.h"
#include "DependViewDlg.h"
#include "DependNode.h"
#include "DependNodeList.h"
#include "ImpHelper.h"
#include "DataItemTracker.h"
#include "ImpDataTask.h"

#include "MaterialLibLoader.h"
#include "MaterialLibWriter.h"
#include "IMaterialSize.h"
#include "MaterialThickness.h"
#include "Material.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

ExplorerViewHelper::ExplorerViewHelper(void)
{
}

ExplorerViewHelper::~ExplorerViewHelper(void)
{
}

void ExplorerViewHelper::Init(ProductTree* pProductTree)
{
	m_pProductTree = pProductTree;
}

FolderNodePtr ExplorerViewHelper::GetParentNode(FolderNodePtr pWorkspace, HTREEITEM hItem)
{
	// get the parent node.
	HTREEITEM hParentItem = m_pProductTree->GetParentItem(hItem);
	UINT dataVal1 = m_pProductTree->GetItemData(hParentItem);
	ProductNodeData* pParentData = (ProductNodeData*)dataVal1;

	// get parent node.
	ProductItemPtr pTmpItem = pWorkspace->SearchSubNode(pParentData->GetProductNodeID());
	FolderNodePtr pParentNode = boost::dynamic_pointer_cast<FolderNode>(pTmpItem);

	return pParentNode;
}

ProductItemListPtr ExplorerViewHelper::GetProdItemList(FolderNodePtr pWorkspace, IntegerListPtr pProductDataList)
{
	ProductItemListPtr pProductItemList(new ProductItemList);

	for (unsigned int i = 0; i < pProductDataList->size(); i++)
	{
		ProductNodeData* pData = (ProductNodeData*)pProductDataList->at(i);

		ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
		pProductItemList->push_back(pProductItem);
	}

	return pProductItemList;
}

void ExplorerViewHelper::CreateFolder(FolderNodePtr pWorkspace)
{
	CString str;

	/************************************************************************/	
	// get the selected item, you know, it should be a folder node.

	HTREEITEM hParentNode = m_pProductTree->GetSelectedList().at(0);
	UINT dataVal = m_pProductTree->GetItemData(hParentNode);
	ProductNodeData* pData = (ProductNodeData*)dataVal;

	ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(pProductItem);
	/************************************************************************/

	if (pData->GetProductNodeType() != PRODUCT_NODE_FOLDER)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SEL_FOLDER);
		MessageBox(NULL, str, NULL, MB_OK | MB_ICONWARNING);
	}
	else
	{
		// create a folder item.
		FolderInfoDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			CString strName, strComment;
			dlg.GetMatNameInfo(strName, strComment);

			/************************************************************************/
			// build a folder item.

			FolderNodePtr pFolderNode(new FolderNode);
			pFolderNode->SetParentID(pParentFolder->GetID());
			pFolderNode->SetName(strName);
			pFolderNode->SetComment(strComment);
			pFolderNode->SetCreateTime(DateHelper::GetCurrentTime());
			pFolderNode->SetModifyTime(DateHelper::GetCurrentTime());
			ProductItemListPtr pProductItemList(new ProductItemList);
			pFolderNode->SetSubItemList(pProductItemList);

			// append it to parent node.
			pParentFolder->GetSubItemList()->push_back(pFolderNode);
			/************************************************************************/

			// save it to database.
			DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
			ProductItemWriter::CreateProductItem(pMainDBCon, pFolderNode);

			/************************************************************************/
			// update the tree node.

			// the data which will be saved to the tree node.
			ProductNodeData* pProductNodeData = new ProductNodeData();
			pProductNodeData->SetProductNodeID(pFolderNode->GetID());
			pProductNodeData->SetProductNodeType(PRODUCT_NODE_FOLDER);

			// create tree node.
			TVINSERTSTRUCT hItem;
			hItem.hParent = hParentNode;
			hItem.item.pszText = pFolderNode->GetName().AllocSysString();
			hItem.item.cchTextMax = MAXLENGTH_STR;
			hItem.hInsertAfter = TVI_LAST;
			hItem.item.iImage = 0;
			hItem.item.iSelectedImage = 0;
			hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;

			// save data in tree node.
			HTREEITEM hSubNode = m_pProductTree->InsertItem(&hItem);
			m_pProductTree->SetItemData(hSubNode, (int)pProductNodeData);
			m_pProductTree->Expand(hParentNode, TVE_EXPAND);
			/************************************************************************/
		}
	}
}

void ExplorerViewHelper::CreatePart(FolderNodePtr pWorkspace)
{
	CString str;

	/************************************************************************/	
	// get the selected item, it should be a folder node.

	HTREEITEM hParentNode = m_pProductTree->GetSelectedList().at(0);
	UINT dataVal = m_pProductTree->GetItemData(hParentNode);
	ProductNodeData* pData = (ProductNodeData*)dataVal;

	ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(pProductItem);
	/************************************************************************/

	if (pData->GetProductNodeType() != PRODUCT_NODE_FOLDER)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SEL_FOLDER);
		MessageBox(NULL, str, NULL, MB_OK | MB_ICONWARNING);
	}
	else
	{
		// create a part here.
		PartInfoDlg partInfoDlg;
		if (partInfoDlg.DoModal() == IDOK)
		{
			PartPtr pPart = partInfoDlg.GetPart();
			pPart->SetCreateTime(DateHelper::GetCurrentTime());
			pPart->SetModifyTime(DateHelper::GetCurrentTime());

			// save the new part.
			DataItemPtr pDataItem = PartManager::SaveNewPart(pPart.get(), pParentFolder);

			/************************************************************************/			
			// update the tree node.

			// the data which will be saved to the tree node.
			ProductNodeData* pProductNodeData = new ProductNodeData();
			pProductNodeData->SetProductNodeID(pDataItem->GetID());
			pProductNodeData->SetProductNodeType(PRODUCT_NODE_PART);

			// create tree node.
			TVINSERTSTRUCT hItem;
			hItem.hParent = hParentNode;
			hItem.item.pszText = pDataItem->GetName().AllocSysString();
			hItem.item.cchTextMax = MAXLENGTH_STR;
			hItem.hInsertAfter = TVI_LAST;
			hItem.item.iImage = 4;
			hItem.item.iSelectedImage = 4;
			hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;

			// save data in tree node.
			HTREEITEM hSubNode = m_pProductTree->InsertItem(&hItem);
			m_pProductTree->SetItemData(hSubNode, (int)pProductNodeData);
			m_pProductTree->Expand(hParentNode, TVE_EXPAND);
			/************************************************************************/
		}
	}
}

void ExplorerViewHelper::CreateSheet(FolderNodePtr pWorkspace)
{
	CString str;

	/************************************************************************/	
	// get the selected item, you know, it should be a folder node.

	HTREEITEM hParentNode = m_pProductTree->GetSelectedList().at(0);
	UINT dataVal = m_pProductTree->GetItemData(hParentNode);
	ProductNodeData* pData = (ProductNodeData*)dataVal;

	ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(pProductItem);
	/************************************************************************/

	if (pData->GetProductNodeType() != PRODUCT_NODE_FOLDER)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SEL_FOLDER);
		MessageBox(NULL, str, NULL, MB_OK | MB_ICONWARNING);
	}
	else
	{
		// create a sheet here.
		SheetInfoDlg sheetInfoDlg;
		if (sheetInfoDlg.DoModal() == IDOK)
		{
			SheetPtr pSheet = sheetInfoDlg.GetSheet();

			// build a data item and append it to parent node.
			DataItemPtr pDataItem(new DataItem(pSheet->GetID(), pParentFolder->GetID(), DATANODE_SHEET, pSheet->GetName(),
											   DateHelper::GetCurrentTime(), DateHelper::GetCurrentTime()));
			pDataItem->SetComment(pSheet->GetComment());
			pParentFolder->GetSubItemList()->push_back(pDataItem);

			/************************************************************************/
			// save some data into database.

			// save the new sheet.
			ProductFileManager::CreateEmptyShtDB(pSheet->GetID());
			DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(pSheet->GetID());
			SheetWriter::SaveSheet(pShtDBCon, pSheet);

			// update "matSizeInfo" table, we should increase "m_iFinishedNum" of the MatSize object.
			DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
			IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, pSheet->GetMaterialSizeID());
			pMaterialSize->SetAvailNum(pMaterialSize->GetAvailNum()-1);
			MaterialLibWriter::SaveMatSize(pMatLibDBCon, pMaterialSize, FALSE);

			// save the data item.
			DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
			ProductItemWriter::CreateProductItem(pMainDBCon, pDataItem);
			/************************************************************************/

			/************************************************************************/			
			// update the tree node.

			// the data which will be saved to the tree node.
			ProductNodeData* pProductNodeData = new ProductNodeData();
			pProductNodeData->SetProductNodeID(pDataItem->GetID());
			pProductNodeData->SetProductNodeType(PRODUCT_NODE_SHEET);

			// create tree node.
			TVINSERTSTRUCT hItem;
			hItem.hParent = hParentNode;
			hItem.item.pszText = pDataItem->GetName().AllocSysString();
			hItem.item.cchTextMax = MAXLENGTH_STR;
			hItem.hInsertAfter = TVI_LAST;
			hItem.item.iImage = 5;
			hItem.item.iSelectedImage = 5;
			hItem.item.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_TEXT;

			// save data in tree node.
			HTREEITEM hSubNode = m_pProductTree->InsertItem(&hItem);
			m_pProductTree->SetItemData(hSubNode, (int)pProductNodeData);
			m_pProductTree->Expand(hParentNode, TVE_EXPAND);
			/************************************************************************/
		}
	}
}

void ExplorerViewHelper::Import(FolderNodePtr pWorkspace)
{
	// 得到节点中保存的数据。
	HTREEITEM hParentNode = m_pProductTree->GetSelectedList().at(0);
	ProductNodeData* pData = (ProductNodeData*)m_pProductTree->GetItemData(hParentNode);

	// 导入数据。
	ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(pProductItem);
	ProductItemListPtr pNewProductItemList = ImpHelper::ImportData(pParentFolder, FALSE);
	if (pNewProductItemList)
	{
		// append the nodes under sheet folder and return all tree nodes.
		vector<HTREEITEM> newTreeItemList;
		m_pProductTree->AppendSubNode(pNewProductItemList, hParentNode, newTreeItemList);

		// select the items which inserted.
		// notes:
		//   1) we only select the first layer tree nodes which inserted.
		m_pProductTree->Expand(hParentNode, TVE_EXPAND);
		m_pProductTree->SelectAll(FALSE);
		m_pProductTree->CanIgnore_selectChange_event(true);
		for (unsigned int i = 0; i < newTreeItemList.size(); i++)
		{
			if (i == newTreeItemList.size()-1)
			{
				m_pProductTree->CanIgnore_selectChange_event(false);
			}

			HTREEITEM newTreeItem = newTreeItemList.at(i);
			ProductNodeData* pProductNodeData = (ProductNodeData*)m_pProductTree->GetItemData(newTreeItem);
			if (pNewProductItemList->GetItemByID(pProductNodeData->GetProductNodeID()))
				m_pProductTree->SetItemState(newTreeItem, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

void ExplorerViewHelper::SplitImport(FolderNodePtr pWorkspace)
{
	// 得到节点中保存的数据。
	HTREEITEM hParentNode = m_pProductTree->GetSelectedList().at(0);
	ProductNodeData* pData = (ProductNodeData*)m_pProductTree->GetItemData(hParentNode);

	// 导入数据。
	ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(pProductItem);
	ProductItemListPtr pNewProductItemList = ImpHelper::SplitImportPart(pParentFolder, FALSE);
	if (pNewProductItemList)
	{
		// append the nodes under sheet folder and return all tree nodes.
		vector<HTREEITEM> newTreeItemList;
		m_pProductTree->AppendSubNode(pNewProductItemList, hParentNode, newTreeItemList);

		// select the items which inserted.
		// notes:
		//   1) we only select the first layer tree nodes which inserted.
		m_pProductTree->Expand(hParentNode, TVE_EXPAND);
		m_pProductTree->SelectAll(FALSE);
		for (unsigned int i = 0; i < newTreeItemList.size(); i++)
		{
			HTREEITEM newTreeItem = newTreeItemList.at(i);
			ProductNodeData* pProductNodeData = (ProductNodeData*)m_pProductTree->GetItemData(newTreeItem);
			if (pNewProductItemList->GetItemByID(pProductNodeData->GetProductNodeID()))
				m_pProductTree->SetItemState(newTreeItem, LVIS_SELECTED, LVIS_SELECTED);
		}
	}
}

void ExplorerViewHelper::Export(FolderNodePtr pWorkspace)
{
	CString strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// get the business object which will be exported.
	IBusinessDataPtr pBusinessData;
	{
		// get item data of the tree node.
		HTREEITEM hItem = m_pProductTree->GetSelectedList().at(0);
		UINT dataVal = m_pProductTree->GetItemData(hItem);
		ProductNodeData* pData = (ProductNodeData*)dataVal;

		// get the business object.
		ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
		DataItemPtr pDataNode = boost::dynamic_pointer_cast<DataItem>(pProductItem);
		if (pDataNode->GetDataType() == DATANODE_PART)
		{
			DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(pDataNode->GetDataID());
			pBusinessData = PartLoader::LoadPart(pPartDBCon, pDataNode->GetDataID());
		}
		else if (pDataNode->GetDataType() == DATANODE_SHEET)
		{
			// load sheet.
			DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(pDataNode->GetDataID());
			SheetPtr pSheet = SheetLoader::LoadSheet(pShtDBCon, pDataNode->GetDataID());
			SheetProcessor::CheckLoopCutFeature(pSheet); // 处理空的“回路起切特征”。
			pBusinessData = pSheet;
		}
		else if (pDataNode->GetDataType() == DATANODE_NESTJOB)
		{
			// 加载排版作业
			DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
			pBusinessData = NestJobLoader::LoadNestJob(pMainDBCon, pDataNode->GetDataID(), TRUE, true, SHEETLOAD_ALL);
		}
	}

	// get the export info from "ExpDataDialog".
	BUSINESS_DATA_TYPE busiDataType = pBusinessData->GetBusinessDataType();
	if (busiDataType == BUSINESS_DATA_NESTJOB)
	{
		// 导出位置
		CString strFolder;
		{
			BROWSEINFO bi;
			memset(&bi, 0, sizeof(BROWSEINFO));
			bi.hwndOwner = NULL;
			bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
			CString strTitle;
			strTitle = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NEST_EXPORT_FOLDER);
			bi.lpszTitle = strTitle;
			LPITEMIDLIST idl = SHBrowseForFolder(&bi);
			if (idl == NULL)
				return;
			SHGetPathFromIDList(idl, strFolder.GetBuffer(MAX_PATH));
			strFolder.ReleaseBuffer();
		}

		NestJobPtr pNestJob = boost::dynamic_pointer_cast<NestJob>(pBusinessData);
		if (ExportManager::ExportNestJob(pNestJob, strFolder))
		{
			CString str;
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_NEST_SUCCESS);
			MessageBox(NULL, str, strProductName, MB_OK | MB_ICONINFORMATION);
		}
	}
	else
	{
		ExportDataInfoPtr pExpDataInfo = GetExpDataInfo(busiDataType);
		if (pExpDataInfo)
		{
			CString strExpFilePath = pExpDataInfo->GetExpFilePath();
			IMPEXP_DATA_TYPE expDataType = pExpDataInfo->GetExpDataType();

			if (busiDataType == BUSINESS_DATA_PART)
			{
				PartPtr pPart = boost::dynamic_pointer_cast<Part>(pBusinessData);
				if (ExportManager::ExportPart(pPart, strExpFilePath, expDataType))
				{
					CString str;
					str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_PRT_SUCCESS);
					MessageBox(NULL, str, strProductName, MB_OK | MB_ICONINFORMATION);
				}
			}
			else if (busiDataType == BUSINESS_DATA_SHEET)
			{
				SheetPtr pSheet = boost::dynamic_pointer_cast<Sheet>(pBusinessData);
				if (ExportManager::ExportSheet(pSheet, strExpFilePath, expDataType))
				{
					CString str;
					str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_SHT_SUCCESS);
					MessageBox(NULL, str, strProductName, MB_OK | MB_ICONINFORMATION);
				}
			}
		}
	}
}

ExportDataInfoPtr ExplorerViewHelper::GetExpDataInfo(BUSINESS_DATA_TYPE businessDataType)
{
    ExportDataInfoPtr pExpDataInfo;


    /************************************************************************/
    // decide the "save as" file types.

    CString strFilter, strDflExt;
    if (businessDataType == BUSINESS_DATA_PART)
    {
        strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_PRT_DXFDWG_FILEEXT);
        strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DXF_EXT);
    }
    else if (businessDataType == BUSINESS_DATA_SHEET)
    {
        strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_SHT_DXFDWG_FILEEXT);
        strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DXF_EXT);
    }
    /************************************************************************/


    // display the "save as" dialog.
    ExpDataDialog expFileDlg(strDflExt, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
    if (expFileDlg.DoModal() == IDOK)
    {
        // get the target file.
        CString strFilePath = expFileDlg.GetPathName();
        IMPEXP_DATA_TYPE expType = expFileDlg.GetExpType();

        pExpDataInfo.reset(new ExportDataInfo(strFilePath, expType));
    }

    return pExpDataInfo;
}

void ExplorerViewHelper::DeleteTreeNode(HTREEITEM hParentItem, LONGLONG iSubNodeID)
{
	UINT iItemData;
	ProductNodeData* pData = NULL;

	HTREEITEM hChildItem = m_pProductTree->GetChildItem(hParentItem);
	while (hChildItem)
	{
		// get the item data.
		iItemData = m_pProductTree->GetItemData(hChildItem);
		pData = (ProductNodeData*)iItemData;

		if (pData->GetProductNodeID() == iSubNodeID)
		{
			// delete node.
			m_pProductTree->DeleteItem(hChildItem);
			break;
		}
		else
		{
			// get the item data.
			iItemData = m_pProductTree->GetItemData(hChildItem);
			pData = (ProductNodeData*)iItemData;
			if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
			{
				DeleteTreeNode(hChildItem, iSubNodeID);
			}

			hChildItem = m_pProductTree->GetNextSiblingItem(hChildItem);
		}
	}
}

void ExplorerViewHelper::DeleteTreeNode(HTREEITEM hParentItem, IntegerListPtr pSubNodeIDList)
{
	UINT iItemData;
	ProductNodeData* pData = NULL;

	HTREEITEM hChildItem = m_pProductTree->GetChildItem(hParentItem);
	while (hChildItem) 
	{
		// get the item data.
		iItemData = m_pProductTree->GetItemData(hChildItem);
		pData = (ProductNodeData*)iItemData;

		/************************************************************************/
		// check whether we should delete "hChildItem".

		BOOL bFind = FALSE;
		for (unsigned int i = 0; i < pSubNodeIDList->size(); i++)
		{
			LONGLONG iTmpID = pSubNodeIDList->at(i);
			if (pData->GetProductNodeID() == iTmpID)
			{
				bFind = TRUE;
				break;
			}
		}
		/************************************************************************/

		if (bFind)
		{
			HTREEITEM hTmpItem = m_pProductTree->GetNextSiblingItem(hChildItem);

			// delete node.
			m_pProductTree->DeleteItem(hChildItem);

			//
			hChildItem = hTmpItem;
		}
		else
		{
			// get the item data.
			iItemData = m_pProductTree->GetItemData(hChildItem);
			pData = (ProductNodeData*)iItemData;
			if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
			{
				DeleteTreeNode(hChildItem, pSubNodeIDList);
			}

			hChildItem = m_pProductTree->GetNextSiblingItem(hChildItem);
		}
	}
}

void ExplorerViewHelper::DeletePartNode(FolderNodePtr pWorkspace, HTREEITEM hItem, DataItemPtr pPartNode)
{
	HTREEITEM hParentItem = m_pProductTree->GetParentItem(hItem);

	/************************************************************************/
	// update the tree node.

	// get parent node.
	FolderNodePtr pParentNode = GetParentNode(pWorkspace, hItem);

	// delete the tree node.
	DeleteTreeNode(hParentItem, pPartNode->GetID());

	// delete DataItem from "pParentNode".
	pParentNode->DeleteSubNode(pPartNode->GetID());
	/************************************************************************/

	// delete the part from db.
	PartManager::DeletePart(pPartNode->GetDataID());
}

void ExplorerViewHelper::DeleteDependTree(FolderNodePtr pWorkspace, DependNodePtr pDependNode, BOOL bTransaction)
{
	// get all data item ID in the dependence tree.
	IntegerListPtr pIntegerList = pDependNode->GetAllDataItemID();

	/************************************************************************/
	// update the tree node.

	// delete the tree node.
	DeleteTreeNode(m_pProductTree->GetRootItem(), pIntegerList);

	// delete all DataItem from "pParentNode".
	pWorkspace->DeleteSubNode(pIntegerList);
	/************************************************************************/

	// update the database.
	DependNodeMgr::DelDataOnDpdTree(pDependNode);
}

void ExplorerViewHelper::DeleteSheetNode(FolderNodePtr pWorkspace, HTREEITEM hItem, DataItemPtr pSheetNode)
{
	HTREEITEM hParentItem = m_pProductTree->GetParentItem(hItem);

	/************************************************************************/
	// update the tree node.

	// get parent node.
	FolderNodePtr pParentNode = GetParentNode(pWorkspace, hItem);

	// delete the tree node.
	DeleteTreeNode(hParentItem, pSheetNode->GetID());

	// delete DataItem from "pParentNode".
	pParentNode->DeleteSubNode(pSheetNode->GetID());
	/************************************************************************/

	// update the database.
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	int iSheetCount = NestJobLoader::GetSheetCount(pMainDBCon, pSheetNode->GetDataID());
	if (iSheetCount != INVALID_VALUE)
		SheetManager::DeleteSheet(pSheetNode->GetDataID(), iSheetCount, FALSE);
	else
		SheetManager::DeleteSheet(pSheetNode->GetDataID(), 1, FALSE);
}

void ExplorerViewHelper::DeleteNestJobNode(FolderNodePtr pWorkspace, HTREEITEM hItem, DataItemPtr pNestJobNode)
{
	HTREEITEM hParentItem = m_pProductTree->GetParentItem(hItem);

	/************************************************************************/
	// update the tree node.

	// get parent node.
	FolderNodePtr pParentNode = GetParentNode(pWorkspace, hItem);

	// delete the tree node.
	DeleteTreeNode(hParentItem, pNestJobNode->GetID());

	// delete DataItem from "pParentNode".
	pParentNode->DeleteSubNode(pNestJobNode->GetID());
	/************************************************************************/

	/************************************************************************/
	// update the database.

	// delete the nest job.
	NestJobMgr::DeleteNestJob(pNestJobNode->GetDataID(), FALSE);

	// update the "productStruct_data" table.
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	IntegerListPtr pIDList(new IntegerList);
	pIDList->AddInt(pNestJobNode->GetID());
	ProductItemWriter::DeleteDataItem(pMainDBCon, pIDList);
	/************************************************************************/
}

void ExplorerViewHelper::ViewDepend(FolderNodePtr pWorkspace, HWND hWnd)
{
	/************************************************************************/
	// get all selected data items.

	// all selected product items.
	IntegerListPtr pProductDataList = m_pProductTree->GetSelTreeItemData();
	ProductItemListPtr pProductItemList = GetProdItemList(pWorkspace, pProductDataList);

	// all selected data items.
	ProductItemListPtr pDataItemList(new ProductItemList);
	pProductItemList->GetAllDataItem(pDataItemList);
	/************************************************************************/

	// 得到依赖关系。
	ProductItemListPtr pAllDataItemList(new ProductItemList);
	pWorkspace->GetAllDataItem(pAllDataItemList);
	DependNodeListPtr pDependNodeList = DependNodeMgr::GetDependTree(pAllDataItemList, pDataItemList);
	if (pDependNodeList->HasDependence())
	{
		DependViewDlg dlg;
		dlg.SetDependNodeList(pDependNodeList);
		dlg.DoModal();
	}
	else
	{
		CString str, strProductName;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NO_DEPENDENCE);
		MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
	}
}

BOOL ExplorerViewHelper::CheckDataItemOpened(CString strTaskName, ProductItemListPtr pDataItemList)
{
	BOOL bOpened = FALSE;

	LogMessageListPtr pLogMessageList(new LogMessageList);

	// check each DataItem.
	for (unsigned int i = 0; i < pDataItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pDataItemList->at(i);
		DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		// whether this DataItem has been opened.
		if (DataItemTracker::GetInstance()->IsDataItemOpened(pDataItem))
		{
			bOpened = TRUE;

			// prepare the message content.
			CString strMsgContent;
			if (pDataItem->GetDataType() == DATANODE_PART)
			{
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_OPENED);
				strMsgContent.Replace(_T("WILLBEREPLACED1"), pDataItem->GetName());
			}
			else if (pDataItem->GetDataType() == DATANODE_SHEET)
			{
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_OPENED);
				strMsgContent.Replace(_T("WILLBEREPLACED1"), pDataItem->GetName());
			}

			LONGLONG iTimeStamp = DateHelper::GetCurrentTime();
			LogMessagePtr pLogMsg(new LogMessage(iTimeStamp, LOG_MSG_WARNING, strTaskName, strMsgContent));
			pLogMessageList->push_back(pLogMsg);
		}
	}

	if (pLogMessageList->size() > 0)
		LogMgr::WriteLog(pLogMessageList);

	return bOpened;
}

void ExplorerViewHelper::DeleteSelItem(FolderNodePtr pWorkspace, HWND hWnd)
{
	CString str, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// get all selected tree items.
	vector<HTREEITEM> selTreeItemList = m_pProductTree->GetSelTreeItem();

	// if more than one node is selected.
	if (selTreeItemList.size() > 1)
	{
		/************************************************************************/
		// get all selected data items.

		// all selected product items.
		// notes:
		//   1) no parent/child relationship in the returned list.
		IntegerListPtr pProductDataList = m_pProductTree->GetSelTreeItemData();
		ProductItemListPtr pProductItemList = GetProdItemList(pWorkspace, pProductDataList);

		// get all data items in "pProductItemList".
		ProductItemListPtr pDataItemList(new ProductItemList);
		pProductItemList->GetAllDataItem(pDataItemList);

		// get all folder nodes in "pProductItemList".
		// notes:
		//   1) no parent/child relationship in the returned list.
		ProductItemListPtr pFolderNodeList = pProductItemList->GetFolderNodeList();
		/************************************************************************/

		// get the dependence tree of all selected data items.
		ProductItemListPtr pAllDataItemList(new ProductItemList);
		pWorkspace->GetAllDataItem(pAllDataItemList);
		DependNodeListPtr pDependNodeList = DependNodeMgr::GetDependTree(pAllDataItemList, pDataItemList);

		/************************************************************************/
		// user should make sure whether delete.

		BOOL bDelete = FALSE;
		if (pDependNodeList->HasDependence())
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SEL_ITEMS_USED);
			DependMsgDlg dlg;
			dlg.Init(str, pDependNodeList);
			if (dlg.DoModal() == IDC_BTN_YES)
			{
				// check whether the selected items or the dependence of the selected items have been opened.
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_ITEMS);
				ProductItemListPtr pTmpDataItemList = pDependNodeList->GetAllDataItem();
				if (CheckDataItemOpened(str, pTmpDataItemList))
				{
					str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_OPENED_ITEMS);
					MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
					bDelete = FALSE;
				}
				else
					bDelete = TRUE;
			}
		}
		else
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_SEL_ITEMS);
			if (MessageBox(NULL, str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				// check whether the selected items or the dependence of the selected items have been opened.
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_ITEMS);
				ProductItemListPtr pTmpDataItemList = pDependNodeList->GetAllDataItem();
				if (CheckDataItemOpened(str, pTmpDataItemList))
				{
					str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_OPENED_ITEMS);
					MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
					bDelete = FALSE;
				}
				else
					bDelete = TRUE;
			}
		}
		/************************************************************************/

		// if user want to delete the folder, do it.
		if (bDelete)
		{
			/************************************************************************/
			// update the tree node.
			// notes:
			//   1) delete node from tree, then update "pWorkspace" data.

			// all data items in the dependence tree.
			IntegerListPtr pDataItemIDList = pDependNodeList->GetAllDataItemID();

			// delete all data item from the tree.
			DeleteTreeNode(m_pProductTree->GetRootItem(), pDataItemIDList);
			// update "pWorkspace" data.
			pWorkspace->DeleteSubNode(pDataItemIDList);

			// delete all folder item from the tree and then update "pWorkspace" data.
			for (unsigned int i = 0; i < pFolderNodeList->size(); i++)
			{
				ProductItemPtr pTmpFolderNode = pFolderNodeList->at(i);
				DeleteTreeNode(m_pProductTree->GetRootItem(), pTmpFolderNode->GetID());
				pWorkspace->DeleteSubNode(pTmpFolderNode->GetID());
			}
			/************************************************************************/

			/************************************************************************/
			// update the database.

			// delete all data in the dependence tree.
			DependNodeMgr::DelAllDataOnDpdTree(pDependNodeList);

			// update the "productStruct_data" and "productStruct_folder" table.
			DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
			pMainDBCon->BeginTransaction();
			for (unsigned int i = 0; i < pFolderNodeList->size(); i++)
			{
				FolderNodePtr pFolderNode = boost::dynamic_pointer_cast<FolderNode>(pFolderNodeList->at(i));
				ProductItemWriter::DeleteFolderItem(pMainDBCon, pFolderNode, FALSE);
			}
			pMainDBCon->CommitTransaction();
			/************************************************************************/
		}
	}
	else if (selTreeItemList.size() == 1)
	{
		HTREEITEM hItem = selTreeItemList.at(0);
		UINT dataVal = m_pProductTree->GetItemData(hItem);
		ProductNodeData* pData = (ProductNodeData*)dataVal;

		if (pData->GetProductNodeType() == PRODUCT_NODE_FOLDER)
		{
			// get the selected folder item.
			ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
			FolderNodePtr pTmpFolderNode = boost::dynamic_pointer_cast<FolderNode>(pProductItem);

			// we cannot delete the root folder.
			if (pTmpFolderNode->GetParentID() == INVALID_ID)
			{
				// prompt message to user.
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_ROOT);
				MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
				return;
			}

			/************************************************************************/
			// get dependence tree of all data under the folder.

			// get the dependence tree of all data items under the folder.
			ProductItemListPtr pAllDataItemList(new ProductItemList);
			pWorkspace->GetAllDataItem(pAllDataItemList);
			DependNodeListPtr pDependNodeList = DependNodeMgr::GetDependTree(pAllDataItemList, pTmpFolderNode);

			// all data items in the dependence tree.
			IntegerListPtr pDataItemIDList = pDependNodeList->GetAllDataItemID();
			/************************************************************************/

			/************************************************************************/
			// if some data item in other folder depend on data in this folder, we 
			// should prompt user to delete all these data.

			// whether we can find each item(in "dataItemIDList") in "pTmpFolderNode".
			BOOL bAllDataInFolder = TRUE;
			for (unsigned int i = 0; i < pDataItemIDList->size(); i++)
			{
				LONGLONG iTmpDataItemID = pDataItemIDList->at(i);

				if (!pTmpFolderNode->SearchSubNode(iTmpDataItemID))
				{
					bAllDataInFolder = FALSE;
					break;
				}
			}

			// if some data item in other folder depend on data in this folder, prompt user to delete all these data.
			BOOL bDelete = FALSE;
			if (!bAllDataInFolder)
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_FOLDER_USED);
				DependMsgDlg dlg;
				dlg.Init(str, pDependNodeList);
				if (dlg.DoModal() == IDC_BTN_YES)
				{
					// check whether the selected items or the dependence of the selected items have been opened.
					str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_ITEMS);
					ProductItemListPtr pTmpDataItemList = pDependNodeList->GetAllDataItem();
					if (CheckDataItemOpened(str, pTmpDataItemList))
					{
						str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_OPENED_ITEMS);
						MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
						bDelete = FALSE;
					}
					else
						bDelete = TRUE;
				}
			}
			else
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_FOLDER);
				if (MessageBox(hWnd, str, strProductName, MB_YESNO | MB_ICONWARNING) == IDYES)
				{
					// check whether the selected items or the dependence of the selected items have been opened.
					str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_ITEMS);
					ProductItemListPtr pTmpDataItemList = pDependNodeList->GetAllDataItem();
					if (CheckDataItemOpened(str, pTmpDataItemList))
					{
						str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_OPENED_ITEMS);
						MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
						bDelete = FALSE;
					}
					else
						bDelete = TRUE;
				}
			}
			/************************************************************************/

			/************************************************************************/
			// if user want to delete the folder, do it.
			if (bDelete)
			{
				// update the tree node.
				pWorkspace->DeleteSubNode(pTmpFolderNode->GetID());
				pWorkspace->DeleteSubNode(pDataItemIDList);
				DeleteTreeNode(m_pProductTree->GetRootItem(), pTmpFolderNode->GetID());
				DeleteTreeNode(m_pProductTree->GetRootItem(), pDataItemIDList);

				// delete all data on the dependence tree.
				DependNodeMgr::DelAllDataOnDpdTree(pDependNodeList);

				// update the "productStruct_data" and "productStruct_folder" table.
				DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
				ProductItemWriter::DeleteFolderItem(pMainDBCon, pTmpFolderNode);
			}
			/************************************************************************/
		}
		else if (pData->GetProductNodeType() == PRODUCT_NODE_PART)
		{
			// get the selected part node.
			ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
			DataItemPtr pPartNode = boost::dynamic_pointer_cast<DataItem>(pProductItem);

			// get the dependence tree of the part.
			ProductItemListPtr pDataItemList(new ProductItemList);
			pWorkspace->GetAllDataItem(pDataItemList);
			DependNodePtr pDependNode = DependNodeMgr::GetDependTree(pDataItemList, pPartNode);

			// first, we should check whether the part and its dependence have been opened.
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_ITEMS);
			ProductItemListPtr pTmpDataItemList = pDependNode->GetAllDataItem();
			if (CheckDataItemOpened(str, pTmpDataItemList))
			{
				// prompt message to user.
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_OPENED_ITEMS);
				MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
				return;
			}

			if (pDependNode->GetDependNodeList()->size() > 0)
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_USED);
				DependNodeListPtr pDependNodeList(new DependNodeList);
				pDependNodeList->push_back(pDependNode);

				// make choice
				DependMsgDlg dlg;
				dlg.Init(str, pDependNodeList);
				if (dlg.DoModal() == IDC_BTN_YES)
				{
					// here we should delete all the data on the dependence tree.
					DeleteDependTree(pWorkspace, pDependNode);
				}
			}
			else
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_PART);
				if (MessageBox(hWnd, str, strProductName, MB_YESNO | MB_ICONQUESTION) != IDYES)
					return;

				// delete the part node from the tree, we also need to update
				// the relative data structure and the database.
				DeletePartNode(pWorkspace, hItem, pPartNode);
			}
		}
		else if (pData->GetProductNodeType() == PRODUCT_NODE_SHEET)
		{
			// get the selected sheet node.
			ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
			DataItemPtr pSheetNode = boost::dynamic_pointer_cast<DataItem>(pProductItem);

			// get the dependence tree of the sheet.
			ProductItemListPtr pDataItemList(new ProductItemList);
			pWorkspace->GetAllDataItem(pDataItemList);
			DependNodePtr pDependNode = DependNodeMgr::GetDependTree(pDataItemList, pSheetNode);

			// first, we should check whether the sheet has been opened.
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_ITEMS);
			ProductItemListPtr pTmpDataItemList = pDependNode->GetAllDataItem();
			if (CheckDataItemOpened(str, pTmpDataItemList))
			{
				// prompt message to user.
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_OPENED_ITEMS);
				MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
				return;
			}

			if (pDependNode->GetDependNodeList()->size() > 0)
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_USED);
				DependNodeListPtr pDependNodeList(new DependNodeList);
				pDependNodeList->push_back(pDependNode);

				// make choice
				DependMsgDlg dlg;
				dlg.Init(str, pDependNodeList);
				if (dlg.DoModal() == IDC_BTN_YES)
				{
					// here we should delete all the data on the dependence tree.
					DeleteDependTree(pWorkspace, pDependNode);
				}
			}
			else
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_SHEET);
				if (MessageBox(hWnd, str, strProductName, MB_YESNO | MB_ICONQUESTION) != IDYES)
					return;

				// delete the sheet node from the tree, we also need to update
				// the relative data structure and the database.
				DeleteSheetNode(pWorkspace, hItem, pSheetNode);
			}
		}
		else if (pData->GetProductNodeType() == PRODUCT_NODE_NESTJOB)
		{
			// get the selected nest job node.
			ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
			DataItemPtr pNestJobNode = boost::dynamic_pointer_cast<DataItem>(pProductItem);

			// get the dependence tree of the nest job.
			ProductItemListPtr pDataItemList(new ProductItemList);
			pWorkspace->GetAllDataItem(pDataItemList);
			DependNodePtr pDependNode = DependNodeMgr::GetDependTree(pDataItemList, pNestJobNode);

			// here, we should check whether the dependence of the nest job have been opened.
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_ITEMS);
			ProductItemListPtr pTmpDataItemList = pDependNode->GetAllDataItem();
			if (CheckDataItemOpened(str, pTmpDataItemList))
			{
				// prompt message to user.
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_OPENED_ITEMS);
				MessageBox(hWnd, str, strProductName, MB_OK | MB_ICONWARNING);
				return;
			}

			// we think a nest job must have its dependence, BUT for one case,
			// that is, if we all the sheets under the job have been deleted, the job has no dependence.
			if (pDependNode->GetDependNodeList()->size() > 0)
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NESTJOB_USED);
				DependNodeListPtr pDependNodeList(new DependNodeList);
				pDependNodeList->push_back(pDependNode);

				// make choice
				DependMsgDlg dlg;
				dlg.Init(str, pDependNodeList);
				if (dlg.DoModal() == IDC_BTN_YES)
				{
					// here we should delete all the data on the dependence tree.
					DeleteDependTree(pWorkspace, pDependNode);
				}
			}
			else
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_NESTJOB);
				if (MessageBox(hWnd, str, strProductName, MB_YESNO | MB_ICONQUESTION) != IDYES)
					return;

				// delete the job node from the tree, we also need to update
				// the relative data structure and the database.
				DeleteNestJobNode(pWorkspace, hItem, pNestJobNode);
			}
		}
	}
}

void ExplorerViewHelper::MoveItem(FolderNodePtr pWorkspace, vector<HTREEITEM> itemList, HTREEITEM targetItem)
{
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();

	/************************************************************************/
	// get some data of the parent node.

	// get the new parent ID.
	ProductNodeData* pData = (ProductNodeData*)m_pProductTree->GetItemData(targetItem);
	LONGLONG iNewParentID = pData->GetProductNodeID();

	// get the new parent item.
	ProductItemPtr pTmpProductItem = pWorkspace->SearchSubNode(iNewParentID);
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(pTmpProductItem);
	/************************************************************************/

	/************************************************************************/
	// change the parent ID of each item in "itemList"

	// begin the db transaction.
	pMainDBCon->BeginTransaction();

	for (unsigned int i = 0; i < itemList.size(); i++)
	{
		// get the product item.
		HTREEITEM hItem = itemList.at(i);
		pData = (ProductNodeData*)m_pProductTree->GetItemData(hItem);
		ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());

		// change the data structure.
		pProductItem->SetParentID(iNewParentID);

		// update "pWorkspace".
		pWorkspace->DeleteSubNode(pProductItem->GetID());
		pParentFolder->GetSubItemList()->push_back(pProductItem);

		// save the change to database.
		ProductItemWriter::ChangeParentNode(pMainDBCon, pProductItem, FALSE);
	}

	// end the db transaction.
	pMainDBCon->CommitTransaction();
	/************************************************************************/
}

void ExplorerViewHelper::CopyItem(FolderNodePtr pWorkspace, HTREEITEM item, HTREEITEM newItem, HTREEITEM targetItem)
{
	/************************************************************************/
	// get some data of the parent node.

	// get the new parent ID.
	ProductNodeData* pData = (ProductNodeData*)m_pProductTree->GetItemData(targetItem);
	LONGLONG iParentID = pData->GetProductNodeID();

	// get the new parent item.
	ProductItemPtr pTmpProductItem = pWorkspace->SearchSubNode(iParentID);
	FolderNodePtr pParentFolder = boost::dynamic_pointer_cast<FolderNode>(pTmpProductItem);
	/************************************************************************/

	/************************************************************************/
	// clone the data and save it into database.

	LONGLONG iNewDataID = INVALID_ID;
	ProductNodeData* pNewNodeData = new ProductNodeData();

	pData = (ProductNodeData*)m_pProductTree->GetItemData(item);
	ProductItemPtr pProductItem = pWorkspace->SearchSubNode(pData->GetProductNodeID());
	DataItemPtr pSrcDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
	if (pSrcDataItem->GetDataType() == DATANODE_PART)
	{
		// clone part.
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(pSrcDataItem->GetDataID());
		PartPtr pOldPart = PartLoader::LoadPart(pPartDBCon, pSrcDataItem->GetDataID());
		PartPtr pNewPart = PartBuilder::ClonePart(pOldPart.get());

		// save new part.
		DataItemPtr pNewDataItem = PartManager::SaveNewPart(pNewPart.get(), pParentFolder);

		// update the item data of "newItem".
		pNewNodeData->SetProductNodeType(PRODUCT_NODE_PART);
		pNewNodeData->SetProductNodeID(pNewDataItem->GetID());
		m_pProductTree->SetItemData(newItem, (int)pNewNodeData);
	}
	else if (pSrcDataItem->GetDataType() == DATANODE_SHEET)
	{
		// clone the sheet.
		DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(pSrcDataItem->GetDataID());
		SheetPtr pOldSheet = SheetLoader::LoadSheet(pShtDBCon, pSrcDataItem->GetDataID());
		SheetProcessor::CheckLoopCutFeature(pOldSheet); // 处理空的“回路起切特征”。
		SheetPtr pNewSheet = SheetManager::CloneSheet(pOldSheet.get());

		// save new part.
		DataItemPtr pNewDataItem = SheetManager::SaveNewSheet(pNewSheet, 1, pParentFolder);

		// update the item data of "newItem".
		pNewNodeData->SetProductNodeType(PRODUCT_NODE_SHEET);
		pNewNodeData->SetProductNodeID(pNewDataItem->GetID());
		m_pProductTree->SetItemData(newItem, (int)pNewNodeData);
	}
	/************************************************************************/
}

END_CUTLEADER_NAMESPACE()
