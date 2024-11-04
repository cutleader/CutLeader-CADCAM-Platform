#include "StdAfx.h"
#include "PartDocument.h"

#include "DateHelper.h"
#include "LogMessage.h"
#include "LogMgr.h"
#include "DataItem.h"
#include "FolderItem.h"
#include "clBaseDataResource.h"
#include "ProductSaveInfoDlg.h"
#include "DataBaseManager.h"
#include "ProductItemLoader.h"

#include "PartCadData.h"
#include "PartCamData.h"
#include "PartCamDataList.h"
#include "PartItem.h"
#include "Part.h"
#include "PartWriter.h"
#include "PartLoader.h"
#include "PartBuilder.h"
#include "PartManager.h"

#include "SheetLoader.h"

#include "RecentData.h"
#include "RecentDataList.h"
#include "DataItemTracker.h"
#include "clDataManagerResource.h"
#include "ProductItemLoader.h"
#include "ProductItemWriter.h"

#include "clFrameResource.h"
#include "UIManager.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartDocument, CLeaderDoc)

PartDocument::PartDocument(void)
{
	m_pPartItem.reset();
}

PartDocument::~PartDocument(void)
{
}

BOOL PartDocument::OnNewDocument()
{
	return CLeaderDoc::OnNewDocument();
}

BOOL PartDocument::OnSaveDocument()
{
	CString strProductName, strMsgContent, strTaskName;
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_PART);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_pPartItem->IsModified())
	{
		// 是否是新零件。
		DataProperty partProp;
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		if (!ProductItemLoader::GetDataProp(pMainDBCon, m_pPartItem->GetPartID(), partProp))
		{
			// choose where to save.
			ProductSaveInfoDlg dlg;
			dlg.Init(strTaskName, ProductItemLoader::LoadProductTree(pMainDBCon), m_pPartItem->GetPartName());
			if (dlg.DoModal() == IDOK)
			{
				FolderNodePtr pParentFolder = dlg.GetSelectFolder();
				CString strName = dlg.GetName();

				// create part object.
				PartCadDataPtr pPartCadData = m_pPartItem->GetCadData();
				PartCamDataListPtr pCamDataList(new PartCamDataList);
				pCamDataList->push_back(m_pPartItem->GetCamData());
				PartPtr pPart(new Part(m_pPartItem->GetPartID(), strName, m_pPartItem->GetPartComment(), pPartCadData, pCamDataList));
				pPart->SetCreateTime(DateHelper::GetCurrentTime());
				pPart->SetModifyTime(DateHelper::GetCurrentTime());

				// save the new part.
				DataItemPtr pDataItem = PartManager::SaveNewPart(pPart.get(), pParentFolder);
				m_pPartItem->SetModified(FALSE);

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
				strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SAVED);
				strMsgContent.Replace(_T("WILLBEREPLACED1"), m_pPartItem->GetPartName());
				LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));
				LogMgr::WriteLog(pLogMsg);
				POSITION pos = GetFirstViewPosition();
				MessageBox(GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
			}
		}
		else
		{
			// save the existing part.
			PartManager::SaveExistPart(m_pPartItem);
			m_pPartItem->SetModified(FALSE);

			// update data which opened recently.
			RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
			RecentDataPtr pRecentData = pRecentDataList->GetItemByDataID(m_pPartItem->GetPartID());
			pRecentDataList->MoveToHead(pRecentData->GetID());
			UIManager::GetInstance()->UpdateRecentDataMenu();

			// create a message.
			strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SAVED);
			strMsgContent.Replace(_T("WILLBEREPLACED1"), m_pPartItem->GetPartName());
			LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_INFO, strTaskName, strMsgContent));
			LogMgr::WriteLog(pLogMsg);
			POSITION pos = GetFirstViewPosition();
			MessageBox(GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
		}
	}
	else
	{
		// create a message.
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_NOT_CHANGED);
		strMsgContent.Replace(_T("WILLBEREPLACED1"), m_pPartItem->GetPartName());
		LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_WARNING, strTaskName, strMsgContent));
		LogMgr::WriteLog(pLogMsg);
		POSITION pos = GetFirstViewPosition();
		MessageBox(GetNextView(pos)->m_hWnd, strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
	}

	return TRUE;
}

void PartDocument::SetModifyStatus()
{
	BOOL bModified = m_pPartItem->IsModified();
	SetModifiedFlag(bModified);
}

void PartDocument::SetPartItem(PartItemPtr pPartItem) 
{ 
	m_pPartItem = pPartItem;

	CString str = GetDisplayName();
	SetTitle(str);
}

CString PartDocument::GetDisplayName()
{
	CString strName = m_pPartItem->GetPartName();
	return strName;
}

END_CUTLEADER_NAMESPACE()
