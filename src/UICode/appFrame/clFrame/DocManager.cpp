#include "StdAfx.h"
#include "DocManager.h"

#include "InteractBar.h"
#include "IntegerList.h"
#include "DateHelper.h"
#include "LogMessage.h"
#include "LogMgr.h"

#include "clBaseDataResource.h"
#include "DataItem.h"
#include "FolderItem.h"
#include "ProductSaveInfoDlg.h"
#include "DataBaseManager.h"
#include "ProgressData.h"
#include "ProgressDlg.h"

#include "Part.h"
#include "PartCadData.h"
#include "PartCamData.h"
#include "PartCamDataList.h"
#include "CamDataSelDlg.h"
#include "PartItem.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "PartInfoDlg.h"
#include "PartBuilder.h"
#include "PartManager.h"
#include "LoopCutFeatureManager.h"

#include "Sheet.h"
#include "SheetLoader.h"
#include "SheetWriter.h"
#include "SheetProcessor.h"
#include "SheetInfoDlg.h"
#include "SheetManager.h"
#include "clSheetResource.h"

#include "NestScheduleDlg.h"
#include "NestJob.h"
#include "NestJobMgr.h"

#include "clDataManagerResource.h"
#include "DataItemTracker.h"
#include "RecentData.h"
#include "RecentDataList.h"
#include "ProductItemLoader.h"
#include "ProductItemWriter.h"

#include "clFrameResource.h"
#include "PartDocument.h"
#include "PartView.h"
#include "SheetDocument.h"
#include "SheetView.h"
#include "UIManager.h"

#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "MaterialLibWriter.h"
#include "ParamConfigLoader.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

void DocManager::NewPartDoc(CMultiDocTemplate* pPartDocTemplate)
{
	PartInfoDlg partInfoDlg(AfxGetApp()->GetMainWnd());
	if (partInfoDlg.DoModal() == IDOK)
	{
		// 关闭“开始页面”。
		CloseStartPage();

		// open the part.
		pPartDocTemplate->OpenDocumentFile(NULL);

		// create the part item from the part, now there are only one cam data in part.
		PartPtr pPart = partInfoDlg.GetPart();
		PartCamDataListPtr pPartCamDataList = pPart->GetCamDataList();
		PartItemPtr pPartItem(new PartItem(pPart->GetID(), pPart->GetName(), pPart->GetCadData(), pPartCamDataList->at(0)));
		pPartItem->SetPartComment(pPart->GetComment());

		// set PartItem into PartDocument.
		PartDocument* pPartDoc = dynamic_cast<PartDocument*>(GetActiveDocument());
		pPartDoc->SetPartItem(pPartItem);

		// init the part view.
		PartView* pPartView = dynamic_cast<PartView*>(GetActiveView());
		pPartView->Init();
	}
}

void DocManager::NewShtDoc(CMultiDocTemplate* pShtDocTemplate, bool bGuideMode)
{
	SheetInfoDlg sheetInfoDlg;
	if (sheetInfoDlg.DoModal() == IDOK)
	{
		// 关闭“开始页面”。
		CloseStartPage();

		// update the cache data of sheet object.
		SheetPtr pSheet = sheetInfoDlg.GetSheet();
		SheetProcessor::UpdateSheetCache(pSheet);

		// open the sheet.
		pShtDocTemplate->OpenDocumentFile(NULL);

		// set sheet into SheetDocument.
		CLeaderDoc* pDoc = GetActiveDocument();
		SheetDocument* pSheetDoc = dynamic_cast<SheetDocument*>(pDoc);
		pSheetDoc->SetSheet(pSheet);

		// init the sheet view.
		SheetView* pSheetView = dynamic_cast<SheetView*>(GetActiveView());
		pSheetView->Init(bGuideMode);
	}
}

void DocManager::OpenPartDoc(DataItemPtr pDataItem, CMultiDocTemplate* pPartDocTemplate)
{
	// 关闭“开始页面”。
	CloseStartPage();

	// check whether "pDataItem" is opened.
	if (ActivePartDoc(pDataItem, pPartDocTemplate))
		return;

	// init connection to the part db file.
	DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(pDataItem->GetDataID());

	// if the part has more than one cam data, select one.
	LONGLONG iSelCAMDatdID = INVALID_ID;
	vector<DataProperty> camDataList = PartLoader::GetPartCamDataProp(pPartDBCon);
	if (camDataList.size() > 1)
	{
		CamDataSelDlg dlg;
		dlg.SetPartID(pDataItem->GetDataID());
		if (dlg.DoModal() == IDOK)
			iSelCAMDatdID = dlg.GetSelCamDataID();
		else
			return;
	}
	else if (camDataList.size() == 1)
	{
		DataProperty prop = camDataList[0];
		VARIANT v = prop.GetItem(0).GetValue();
		iSelCAMDatdID = v.llVal;
	}

	// load the PartItem object, and open it.
	PartItemPtr pPartItem = PartLoader::LoadPartItem(pPartDBCon, pDataItem->GetDataID(), iSelCAMDatdID, true);
	pPartDocTemplate->OpenDocumentFile(NULL);

	// set PartItem into PartDocument.
	PartDocument* pPartDoc = dynamic_cast<PartDocument*>(GetActiveDocument());
	pPartDoc->SetPartItem(pPartItem);

	/************************************************************************/
	// update the tracked info.

	// add PartItem into DataItemTracker object.
	DataItemTracker::GetInstance()->AddDataItem(pDataItem);

	// update data which opened recently.
	RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
	RecentDataPtr pRecentData = pRecentDataList->GetItemByDataID(pDataItem->GetDataID());
	if (pRecentData)
		pRecentDataList->MoveToHead(pRecentData->GetID());
	else
	{
		RecentDataPtr pRecentData(new RecentData(pDataItem->GetFullPath(), pDataItem->GetDataID()));
		pRecentDataList->insert(pRecentDataList->begin(), pRecentData);
	}
	UIManager::GetInstance()->UpdateRecentDataMenu();
	/************************************************************************/

	// init the part view.
	PartView* pPartView = dynamic_cast<PartView*>(GetActiveView());
	pPartView->Init();

	// show the interact bar.
	InteractBar::GetInstance()->ShowBar(TRUE);
}

void DocManager::OpenShtDoc(DataItemPtr pDataItem, CMultiDocTemplate* pShtDocTemplate)
{
	// 关闭“开始页面”。
	CloseStartPage();

	// check whether "pDataItem" is opened.
	if (ActiveShtDoc(pDataItem, pShtDocTemplate))
		return;

	/************************************************************************/
	// load sheet and open it.

	// load sheet.
	DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(pDataItem->GetDataID());
	SheetPtr pSheet = SheetLoader::LoadSheet(pShtDBCon, pDataItem->GetDataID());
	SheetProcessor::CheckLoopCutFeature(pSheet); // 处理空的“回路起切特征”。

	// if the sheet is modified during loading, save it.
	if (pSheet->IsModified())
		SheetManager::SaveExistSheet(pSheet);

	// open the sheet.
	pShtDocTemplate->OpenDocumentFile(NULL);
	/************************************************************************/

	// set sheet object into current sheet document.
	SheetDocument* pSheetDoc = dynamic_cast<SheetDocument*>(GetActiveDocument());
	pSheetDoc->SetSheet(pSheet);

	/************************************************************************/
	// update the tracked info.

	// add it into DataItemTracker object.
	DataItemTracker::GetInstance()->AddDataItem(pDataItem);

	// update data which opened recently.
	RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
	RecentDataPtr pRecentData = pRecentDataList->GetItemByDataID(pDataItem->GetDataID());
	if (pRecentData)
		pRecentDataList->MoveToHead(pRecentData->GetID());
	else
	{
		RecentDataPtr pRecentData(new RecentData(pDataItem->GetFullPath(), pDataItem->GetDataID()));
		pRecentDataList->insert(pRecentDataList->begin(), pRecentData);
	}
	UIManager::GetInstance()->UpdateRecentDataMenu();
	/************************************************************************/

	// init the "editors" in sheet view.
	SheetView* pSheetView = dynamic_cast<SheetView*>(GetActiveView());
	pSheetView->Init();

	// show the interact bar.
	InteractBar::GetInstance()->ShowBar(TRUE);
}

void DocManager::NewNestJob(CMultiDocTemplate* pShtDocTemplate, bool bGuideMode)
{
	// create a nest task to do nesting.
	NestScheduleDlg nestScheduleDlg(AfxGetApp()->GetMainWnd());
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	nestScheduleDlg.SetProductStruct(ProductItemLoader::LoadProductTree(pMainDBCon));
	INT_PTR iRet = nestScheduleDlg.DoModal();
	if (iRet == IDC_BTN_EXECUTE_NEST)
	{
		CString strJobName;
		FolderNodePtr pParentFolder = nestScheduleDlg.GetSaveFolder(strJobName);
		NestJobPtr pNestJob = nestScheduleDlg.GetNestJob();
		pNestJob->SetName(strJobName);

		// save the nest job into db.
		vector<SheetPtr> sheetList = NestJobMgr::SaveNestJob(pNestJob, pParentFolder);

		// 是否打开生成的板材。
		if (nestScheduleDlg.IsOpenSheet())
		{
			// 关闭“开始页面”。
			CloseStartPage();

			// 打开每个板材。
			for (unsigned int i = 0; i < sheetList.size(); i++)
			{
				// update the cache data of sheet object.
				SheetPtr pSheet = sheetList.at(i);
				SheetProcessor::UpdateSheetCacheEx(pSheet);

				// open the sheet.
				pShtDocTemplate->OpenDocumentFile(NULL);

				// set sheet into SheetDocument.
				CLeaderDoc* pDoc = GetActiveDocument();
				SheetDocument* pSheetDoc = dynamic_cast<SheetDocument*>(pDoc);
				pSheetDoc->SetSheet(pSheet);

				// init the sheet view.
				SheetView* pSheetView = dynamic_cast<SheetView*>(GetActiveView());
				pSheetView->Init(bGuideMode);
			}
		}
	}
}

BOOL DocManager::ActivePartDoc(DataItemPtr pDataItem, CMultiDocTemplate* pPartDocTemplate)
{
	// check whether this DataItem is opened.
	if (DataItemTracker::GetInstance()->IsDataItemOpened(pDataItem))
	{
		// find the opened document and active it.
		POSITION docpos = pPartDocTemplate->GetFirstDocPosition();
		while (docpos)
		{
			CDocument* pDoc = pPartDocTemplate->GetNextDoc(docpos);
			PartDocument* pPartDoc = dynamic_cast<PartDocument*>(pDoc);

			PartItemPtr pPartItem = pPartDoc->GetPartItem();
			if (pPartItem->GetPartID() == pDataItem->GetDataID())
			{
				// get the relative view of the document.
				POSITION pos = pDoc->GetFirstViewPosition();
				CView* pFirstView = pDoc->GetNextView(pos);

				// activate the view.
				CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetMainWnd();
				pFrame->MDIActivate(pFirstView->GetParentFrame());

				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL DocManager::ActiveShtDoc(DataItemPtr pDataItem, CMultiDocTemplate* pShtDocTemplate)
{
	// check whether this DataItem is opened.
	if (DataItemTracker::GetInstance()->IsDataItemOpened(pDataItem))
	{
		// find the opened document and active it.
		POSITION docpos = pShtDocTemplate->GetFirstDocPosition();
		while (docpos)
		{
			CDocument* pDoc = pShtDocTemplate->GetNextDoc(docpos);
			SheetDocument* pShtDoc = dynamic_cast<SheetDocument*>(pDoc);

			SheetPtr pSheet = pShtDoc->GetSheet();
			if (pSheet->GetID() == pDataItem->GetDataID())
			{
				// get the relative view of the document.
				POSITION pos = pDoc->GetFirstViewPosition();
				CView* pFirstView = pDoc->GetNextView(pos);

				// activate the view.
				CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetMainWnd();
				pFrame->MDIActivate(pFirstView->GetParentFrame());

				return TRUE;
			}
		}
	}

	return FALSE;
}

void DocManager::SaveAsPart(PartDocument* pPartDoc)
{
	/************************************************************************/
	// prepare some data.

	CString strProductName, strMsgContent, strTaskName;
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_PART);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	PartItemPtr pOldPartItem = pPartDoc->GetPartItem();
	/************************************************************************/

	// choose where to save.
	ProductSaveInfoDlg dlg;
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	dlg.Init(strTaskName, ProductItemLoader::LoadProductTree(pMainDBCon), pOldPartItem->GetPartName());
	if (dlg.DoModal() == IDOK)
	{
		FolderNodePtr pParentFolder = dlg.GetSelectFolder();
		CString strName = dlg.GetName();

		// check the integrity of the part.
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		PartCadDataPtr pOldCadData = pOldPartItem->GetCadData();
		PartCamDataPtr pOldCamData = pOldPartItem->GetCamData();
		ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pOldCamData->GetParamConfigID());
		LoopCutFeatureManager::CheckIntegrity(pOldCadData->GetPatternLoopList().get(), pOldCamData->GetLoopFeatureList(), pParamConfig.get(), TRUE, FALSE, FALSE, Matrix2D());

		// clone the part item.
		PartItemPtr pNewPartItem = PartBuilder::ClonePartItem(pOldPartItem.get());
		pNewPartItem->SetPartName(strName);

		// update the cache data in part item.
		PartManager::UpdateCacheData(pNewPartItem);

		// set the new part item to doc.
		pPartDoc->SetPartItem(pNewPartItem);

		// save the new part.
		PartCamDataListPtr pCamDataList(new PartCamDataList);
		pCamDataList->push_back(pNewPartItem->GetCamData());
		PartPtr pNewPart(new Part(pNewPartItem->GetPartID(), pNewPartItem->GetPartName(), _T(""), pNewPartItem->GetCadData(), pCamDataList));
		DataItemPtr pDataItem = PartManager::SaveNewPart(pNewPart.get(), pParentFolder);
		pNewPartItem->SetModified(FALSE);

		/************************************************************************/
		// update the tracked info.

		// update the current opened part.
		DataItemTracker::GetInstance()->RemoveDataItem(pOldPartItem->GetPartID());
		DataItemTracker::GetInstance()->AddDataItem(pDataItem);

		// update data which opened recently.
		RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
		RecentDataPtr pRecentData(new RecentData(pDataItem->GetFullPath(), pDataItem->GetDataID()));
		pRecentDataList->insert(pRecentDataList->begin(), pRecentData);
		UIManager::GetInstance()->UpdateRecentDataMenu();
		/************************************************************************/

		// update the part view.
		PartView* pPartView = dynamic_cast<PartView*>(DocManager::GetActiveView());
		pPartView->Init();

		// create a message.
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SAVED);
		strMsgContent.Replace(_T("WILLBEREPLACED1"), pOldPartItem->GetPartName());
		LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));
		LogMgr::WriteLog(pLogMsg);
		POSITION pos = pPartDoc->GetFirstViewPosition();
		MessageBox(pPartDoc->GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
	}
}

void DocManager::SaveAsSht(SheetDocument* pShtDoc)
{
	//************************************************************************/
	// prepare some data.

	CString strProductName, strMsgContent, strTaskName;
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_PART);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	const Sheet* pOldSheet = pShtDoc->GetSheet().get();
	//************************************************************************/

	// choose where to save.
	ProductSaveInfoDlg dlg;
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	dlg.Init(strTaskName, ProductItemLoader::LoadProductTree(pMainDBCon), pOldSheet->GetName());
	if (dlg.DoModal() == IDOK)
	{
		FolderNodePtr pParentFolder = dlg.GetSelectFolder();
		CString strName = dlg.GetName();

		// clone the sheet.
		SheetPtr pNewSheet = SheetManager::CloneSheet(pOldSheet);
		pNewSheet->SetName(strName);
		pShtDoc->SetSheet(pNewSheet);

		// update the cache data of sheet.
		SheetProcessor::UpdateSheetCacheEx(pNewSheet);

		// save the new sheet.
		DataItemPtr pDataItem = SheetManager::SaveNewSheet(pNewSheet, 1, pParentFolder);

		/************************************************************************/
		// update the tracked info.

		// update the current opened data.
		DataItemTracker::GetInstance()->RemoveDataItem(pOldSheet->GetID());
		DataItemTracker::GetInstance()->AddDataItem(pDataItem);

		// update data which opened recently.
		RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
		RecentDataPtr pRecentData(new RecentData(pDataItem->GetFullPath(), pDataItem->GetDataID()));
		pRecentDataList->insert(pRecentDataList->begin(), pRecentData);
		UIManager::GetInstance()->UpdateRecentDataMenu();
		/************************************************************************/

		// update the sheet view.
		SheetView* pSheetView = dynamic_cast<SheetView*>(DocManager::GetActiveView());
		pSheetView->Init();

		// create a message.
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_SAVED);
		strMsgContent.Replace(_T("WILLBEREPLACED1"), pNewSheet->GetName());
		LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));
		LogMgr::WriteLog(pLogMsg);
		POSITION pos = pShtDoc->GetFirstViewPosition();
		MessageBox(pShtDoc->GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
	}
}

CLeaderDoc* DocManager::GetActiveDocument()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();

	// get the active MDI child window.
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	if (!pChild)
		return NULL;

	// get the active view attached to the active MDI child window.
	CLeaderView* pView = (CLeaderView*)pChild->GetActiveView();
	if (!pView)
		return NULL;

	// 得到文档对象。
	CLeaderDoc* pDoc = pView->GetDocument();
	if (!pDoc)
		return NULL;

	return pDoc;
}

CLeaderView* DocManager::GetActiveView()
{
	CMDIFrameWnd* pFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();

	// get the active MDI child window.
	CMDIChildWnd* pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();
	if (!pChild)
		return NULL;

	// get the active view attached to the active MDI child window.
	CLeaderView* pView = (CLeaderView*)pChild->GetActiveView();

	return pView;
}

void DocManager::CloseStartPage()
{
	// 得到“开始页面”文档模板。
	CWinApp* pCWinApp = AfxGetApp();
	POSITION docTplPos = pCWinApp->GetFirstDocTemplatePosition();
	CDocTemplate* pDocTpl = pCWinApp->GetNextDocTemplate(docTplPos);
	pDocTpl = pCWinApp->GetNextDocTemplate(docTplPos);
	pDocTpl = pCWinApp->GetNextDocTemplate(docTplPos);
	CMultiDocTemplate* pStartPageDocTpl = (CMultiDocTemplate*)pDocTpl;

	// 关闭开始页面。
	POSITION docpos = pStartPageDocTpl->GetFirstDocPosition();
	while (docpos)
	{
		CDocument* pDoc = pStartPageDocTpl->GetNextDoc(docpos);
		pDoc->OnCloseDocument();
	}
}

END_CUTLEADER_NAMESPACE()
