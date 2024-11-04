#include "stdafx.h"
#include "clApp.h"

#include "DateHelper.h"
#include "clBaseDataResource.h"
#include "DataItem.h"
#include "FolderItem.h"
#include "ProductItemList.h"
#include "DataBaseManager.h"
#include "ProductFolderManager.h"
#include "ProductFileDefine.h"
#include "LogMgr.h"

#include "Part.h"
#include "PartCamDataList.h"
#include "PartItem.h"
#include "PartLoader.h"

#include "Sheet.h"
#include "SheetLoader.h"
#include "SheetProcessor.h"
#include "SheetInfoDlg.h"

#include "ImpDataSelDlg.h"
#include "ImportManager.h"
#include "clDataImportExportResource.h"

#include "clDataManagerResource.h"
#include "DataItemTracker.h"
#include "RecentData.h"
#include "RecentDataList.h"
#include "ProductItemLoader.h"
#include "DataMgrDlg.h"
#include "ImpHelper.h"

#include "clFrameResource.h"
#include "PartDocument.h"
#include "PartView.h"
#include "SheetDocument.h"
#include "SheetView.h"
#include "DocManager.h"
#include "UIManager.h"

#include "CLOptionInfo.h"
#include "ProductParam.h"
#include "CLOptionWriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void ClApp::OnDataMgr()
{
	DataMgrDlg dataMgrDlg;

	// navigate to the last select data node.
	LONGLONG iLastSelNodeID = DataItemTracker::GetInstance()->GetLastSelNodeID();
	dataMgrDlg.InitNaviNodeID(iLastSelNodeID);

	// display the data management window.
	if (dataMgrDlg.DoModal() == IDC_BTN_EXPLORER_OPEN)
	{
		DataItemPtr pDataItem = dataMgrDlg.GetOpeningDataItem();

		// open the data item.
		if (pDataItem->GetDataType() == DATANODE_PART)
			DocManager::OpenPartDoc(pDataItem, m_pPartDocTemplate);
		else if (pDataItem->GetDataType() == DATANODE_SHEET)
			DocManager::OpenShtDoc(pDataItem, m_pSheetDocTemplate);
	}
}

void ClApp::OnNewPart()
{
	DocManager::NewPartDoc(m_pPartDocTemplate);
}

void ClApp::OnOpenDxf()
{
	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_DXFDWG_FILEEXT);
	ImpDataSelDlg impFileDlg(TRUE, strDflExt, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (impFileDlg.DoModal() == IDOK)
	{
		// dxf or dwg file.
		CString strFilePath = impFileDlg.GetPathName();
		IMPEXP_DATA_TYPE impFileType = impFileDlg.GetImpType(strFilePath);
		BOOL bDwg = FALSE;
		if (impFileType == IMPEXP_DATA_DWG)
			bDwg = TRUE;

        // 得到dxf data
        DxfDataPtr pDxfData = impFileDlg.GetDxfData();
        if (!pDxfData)
        {
            LogMgr::DebugWarn(_T("Debug warning 1 in OnOpenDxf function!"), GetMainWnd());
			return;
        }

		// import the file and open it.
		IBusinessDataPtr pBusinessData = ImportManager::ImportDxfDwg2Part(strFilePath, pDxfData);
		if (pBusinessData)
		{
			// 关闭“开始页面”。
			DocManager::CloseStartPage();

			// build the part item.
			PartPtr pPart = boost::dynamic_pointer_cast<Part>(pBusinessData);
			PartItemPtr pPartItem(new PartItem(pPart->GetID(), pPart->GetName(), pPart->GetCadData(), pPart->GetCamDataList()->at(0)));

			// open the part item.
			m_pPartDocTemplate->OpenDocumentFile(NULL);

			// set PartItem into PartDocument.
			PartDocument* pPartDoc = dynamic_cast<PartDocument*>(DocManager::GetActiveDocument());
			pPartDoc->SetPartItem(pPartItem);

			// init the part view.
			PartView* pPartView = dynamic_cast<PartView*>(DocManager::GetActiveView());
			pPartView->Init();
		}
	}
}

void ClApp::OnNewSheet()
{
	DocManager::NewShtDoc(m_pSheetDocTemplate);
}

void ClApp::OnNewNestJob()
{
	DocManager::NewNestJob(m_pSheetDocTemplate);
}

void ClApp::OnImport()
{
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	FolderNodePtr pSaveFolder = ProductItemLoader::LoadProductTree(pMainDBCon);
	ImpHelper::ImportData(pSaveFolder, TRUE);
}

void ClApp::OnSplitImport()
{
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	FolderNodePtr pSaveFolder = ProductItemLoader::LoadProductTree(pMainDBCon);
	ImpHelper::SplitImportPart(pSaveFolder, TRUE);
}

void ClApp::OnFileSave()
{
	CLeaderDoc* pDoc = DocManager::GetActiveDocument();

	// whether current doc is part.
	PartDocument* pPartDoc = dynamic_cast<PartDocument*>(pDoc);
	if (pPartDoc)
	{
		pPartDoc->OnSaveDocument();
	}

	// whether current doc is sheet.
	SheetDocument* pShtDoc = dynamic_cast<SheetDocument*>(pDoc);
	if (pShtDoc)
	{
		pShtDoc->OnSaveDocument();
	}
}

void ClApp::OnFileSaveAs()
{
	CLeaderDoc* pDoc = DocManager::GetActiveDocument();

	// whether current doc is part.
	PartDocument* pPartDoc = dynamic_cast<PartDocument*>(pDoc);
	if (pPartDoc)
		DocManager::SaveAsPart(pPartDoc);

	// whether current doc is sheet.
	SheetDocument* pShtDoc = dynamic_cast<SheetDocument*>(pDoc);
	if (pShtDoc)
		DocManager::SaveAsSht(pShtDoc);
}

void ClApp::OnOpenDataItem(WPARAM wParam, LPARAM lParam)
{
	// parse the message param.
	DataItem* pDataItem = (DataItem*)wParam;

	// build a tmp DataItem.
	DataItemPtr pTmpDataItem(new DataItem());
	pTmpDataItem->SetID(pDataItem->GetID());
	pTmpDataItem->SetName(pDataItem->GetName());
	pTmpDataItem->SetItemPath(pDataItem->GetItemPath());
	pTmpDataItem->SetDataID(pDataItem->GetDataID());
	pTmpDataItem->SetDataType(pDataItem->GetDataType());

	// release the resource.
	delete pDataItem;

	// open the data item.
	if (pTmpDataItem->GetDataType() == DATANODE_PART)
		DocManager::OpenPartDoc(pTmpDataItem, m_pPartDocTemplate);
	else if (pDataItem->GetDataType() == DATANODE_SHEET)
		DocManager::OpenShtDoc(pTmpDataItem, m_pSheetDocTemplate);
}

void ClApp::OnOpenPartItem(WPARAM wParam, LPARAM lParam)
{
	PartItem* pTmpPartItem = (PartItem*)wParam;
	PartItemPtr pPartItem(pTmpPartItem);

	// open the part item.
	m_pPartDocTemplate->OpenDocumentFile(NULL);

	// set PartItem into PartDocument.
	PartDocument* pPartDoc = dynamic_cast<PartDocument*>(DocManager::GetActiveDocument());
	pPartDoc->SetPartItem(pPartItem);

	// init the part view.
	PartView* pPartView = dynamic_cast<PartView*>(DocManager::GetActiveView());
	pPartView->Init();
}

void ClApp::OnAutoPopupVideo(WPARAM wParam, LPARAM lParam)
{
	CString strPath = ProductFolderManager::GetOptionPath();
	strPath += USER_OPTION_FILE;

	ProductParamPtr pProductParam = CLOptionInfo::GetInstance()->GetProductParam();
	pProductParam->IsShowHelpVideo((BOOL)wParam);
	CLOptionWriter::SaveCLOption(CLOptionInfo::GetInstance(), strPath);
}

void ClApp::OnGuideMode1(WPARAM wParam, LPARAM lParam)
{
	DocManager::NewShtDoc(m_pSheetDocTemplate, true);
}

void ClApp::OnGuideMode2(WPARAM wParam, LPARAM lParam)
{
	DocManager::NewNestJob(m_pSheetDocTemplate, true);
}

BOOL ClApp::OnOpenRecentData(UINT nID)
{
	// the RecentData to be opened.
	RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
	RecentDataPtr pRecentData = pRecentDataList->at(nID - (IDC_RECENT_DATA_BASE));

	/************************************************************************/	
	// get the DataItem object and open it.

	// get the DataItem object.
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	FolderNodePtr pFolderNode = ProductItemLoader::LoadProductTree(pMainDBCon);
	ProductItemListPtr pAllDataItem(new ProductItemList);
	pFolderNode->GetAllDataItem(pAllDataItem);
	ProductItemPtr pProductItem = pAllDataItem->GetItemByDataID(pRecentData->GetDataID());

	// whether exist.
	if (!pProductItem)
	{
		// if the data not exist, delete it from the recent data list.
		pRecentDataList->RemoveItemByID(pRecentData->GetID());

		// prompt user.
		CString strProductName, strMsgContent;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DATA_NOT_FIND);
		strMsgContent.Replace(_T("WILLBEREPLACED1"), pRecentData->GetDataPath());
		MessageBox(GetMainWnd()->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		// open the data.
		DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);
		if (pDataItem->GetDataType() == DATANODE_PART)
			DocManager::OpenPartDoc(pDataItem, m_pPartDocTemplate);
		else if (pDataItem->GetDataType() == DATANODE_SHEET)
			DocManager::OpenShtDoc(pDataItem, m_pSheetDocTemplate);
	}

	// update the UI.
	UIManager::GetInstance()->UpdateRecentDataMenu();
	/************************************************************************/

	return TRUE;
}

void ClApp::OnRecentData()
{
	// dummy.
	;
}
