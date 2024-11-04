#include "StdAfx.h"
#include "SheetDocument.h"

#include "DateHelper.h"
#include "LogMessage.h"
#include "LogMgr.h"
#include "DataItem.h"
#include "FolderItem.h"
#include "clBaseDataResource.h"
#include "ProductSaveInfoDlg.h"
#include "DataBaseManager.h"
#include "ProductFileManager.h"

#include "SheetWriter.h"
#include "SheetLoader.h"
#include "Sheet.h"
#include "SheetManager.h"

#include "RecentData.h"
#include "RecentDataList.h"
#include "clDataManagerResource.h"
#include "ProductItemLoader.h"
#include "ProductItemWriter.h"
#include "DataItemTracker.h"

#include "clFrameResource.h"
#include "UIManager.h"

#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "MaterialLibWriter.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SheetDocument, CLeaderDoc)

SheetDocument::SheetDocument(void)
{
	m_pSheet.reset();
}

SheetDocument::~SheetDocument(void)
{
}

BOOL SheetDocument::OnNewDocument()
{
	return CLeaderDoc::OnNewDocument();
}

BOOL SheetDocument::OnSaveDocument()
{
	CString strMsgContent, strTaskName, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_SHEET);

	if (m_pSheet->IsModified())
	{
		// whether this sheet is a new sheet.
		DataProperty sheetProp;
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		if (!ProductItemLoader::GetDataProp(pMainDBCon, m_pSheet->GetID(), sheetProp))
		{
			// choose where to save.
			ProductSaveInfoDlg dlg;
			dlg.Init(strTaskName, ProductItemLoader::LoadProductTree(pMainDBCon), m_pSheet->GetName());
			if (dlg.DoModal() == IDOK)
			{
				FolderNodePtr pParentFolder = dlg.GetSelectFolder();
				CString strName = dlg.GetName();
				m_pSheet->SetName(strName);

				// update the date.
				m_pSheet->SetCreateTime(DateHelper::GetCurrentTime());
				m_pSheet->SetModifyTime(DateHelper::GetCurrentTime());

				// save the new sheet.
				DataItemPtr pDataItem = SheetManager::SaveNewSheet(m_pSheet, 1, pParentFolder);

				/************************************************************************/
				// update the tracked info.

				// update the current opened data.
				DataItemTracker::GetInstance()->AddDataItem(pDataItem);

				// update data which opened recently.
				RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
				RecentDataPtr pRecentData(new RecentData(pDataItem->GetFullPath(), pDataItem->GetDataID()));
				pRecentDataList->insert(pRecentDataList->begin(), pRecentData);
				UIManager::GetInstance()->UpdateRecentDataMenu();
				/************************************************************************/

				// create a message.
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_SAVED);
				strMsgContent.Replace(_T("WILLBEREPLACED1"), m_pSheet->GetName());
				LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));
				LogMgr::WriteLog(pLogMsg);
				POSITION pos = GetFirstViewPosition();
				MessageBox(GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
			}
		}
		else
		{
			// save existing sheet.
			SheetManager::SaveExistSheet(m_pSheet);

			// update data which opened recently.
			RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
			RecentDataPtr pRecentData = pRecentDataList->GetItemByDataID(m_pSheet->GetID());
			if (pRecentData)
			{
				pRecentDataList->MoveToHead(pRecentData->GetID());
				UIManager::GetInstance()->UpdateRecentDataMenu();
			}

			// create a log Message.
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_SAVED);
			strMsgContent.Replace(_T("WILLBEREPLACED1"), m_pSheet->GetName());
			LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));
			LogMgr::WriteLog(pLogMsg);
			POSITION pos = GetFirstViewPosition();
			MessageBox(GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
		}
	}
	else
	{
		// create a log Message.
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_NOT_CHANGED);
		strMsgContent.Replace(_T("WILLBEREPLACED1"), m_pSheet->GetName());
		LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_WARNING, strTaskName, strMsgContent));
		LogMgr::WriteLog(pLogMsg);
		POSITION pos = GetFirstViewPosition();
		MessageBox(GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
	}

	return TRUE;
}

void SheetDocument::SetModifyStatus()
{
	BOOL bModified = m_pSheet->IsModified();
	SetModifiedFlag(bModified);
}

void SheetDocument::SetSheet(SheetPtr pSheet) 
{
	m_pSheet = pSheet;
	SetTitle(pSheet->GetName());
}

END_CUTLEADER_NAMESPACE()
