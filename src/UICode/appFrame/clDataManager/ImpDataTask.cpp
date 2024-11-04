#include "StdAfx.h"
#include "ImpDataTask.h"

#include "ProgressData.h"
#include "FolderItem.h"
#include "ProductItemList.h"

#include "PartManager.h"
#include "Part.h"
#include "Sheet.h"
#include "SheetManager.h"

#include "ImportDataInfo.h"
#include "ImportDataInfoList.h"
#include "ImportManager.h"
#include "ImpHelper.h"
#include "clDataManagerResource.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

ImpDataTask::ImpDataTask(FolderNodePtr pParentFolder, ImportDataInfoListPtr pImpDataInfoList)
{
	m_pParentFolder = pParentFolder;
	m_pImpDataInfoList = pImpDataInfoList;

	m_pNewProductItemList.reset(new ProductItemList);
}

ImpDataTask::~ImpDataTask(void)
{
}

void ImpDataTask::Run() 
{
	// the import data count.
	unsigned int iImportCount = m_pImpDataInfoList->size();

	// the progress delta.
	double dDelta = 100.0/iImportCount;
	dDelta /= 2;

	// import each object into database and return each data item object.
	CString str;
	for (unsigned int i = 0; i < iImportCount; i++)
	{
		// get the ImportDataInfo.
		ImportDataInfoPtr pImpDataInfo = m_pImpDataInfoList->at(i);
		CString strImpFilePath = pImpDataInfo->GetImpFilePath();
		IMPEXP_DATA_TYPE impDataType = pImpDataInfo->GetImpDataType();

		// import the file.
		if (impDataType == IMPEXP_DATA_DXF || impDataType == IMPEXP_DATA_DWG)
		{
			BOOL bDwg = TRUE;
			if (impDataType == IMPEXP_DATA_DXF)
				bDwg = FALSE;

			/************************************************************************/
			// import dxf/dwg.

			// update the progress data.
			WaitForSingleObject(m_pProgressData->GetProgressDataMtx(), INFINITE);
			if (m_pProgressData->IsCanceled())
			{
				m_pProgressData->IsFinished(TRUE);
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_IMPORT_TASK_CANCEL);
				m_pProgressData->SetSubTask(str);
				PostMessage(m_hProgressWnd, WM_UPDATE_PROGRESS, 0, 0);
				ReleaseMutex(m_pProgressData->GetProgressDataMtx());
				return; 
			}
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PARSING_DXFDWG);
			m_pProgressData->SetSubTask(str);
			PostMessage(m_hProgressWnd, WM_UPDATE_PROGRESS, 0, 0);
			ReleaseMutex(m_pProgressData->GetProgressDataMtx());

			// import dxf/dwg to the business object.
			IBusinessDataPtr pBusinessData;
            if (DxfDataPtr pDxfData = pImpDataInfo->GetDxfData())
            {
                pBusinessData = ImportManager::ImportDxfDwg2Part(strImpFilePath, pDxfData);
            }
            else
            {
                pBusinessData = ImportManager::ImportDxfDwg2Part(strImpFilePath, bDwg);
            }
			if (pBusinessData)
			{
				// update the progress data.
				WaitForSingleObject(m_pProgressData->GetProgressDataMtx(), INFINITE);
				m_pProgressData->SetTaskPercent(m_pProgressData->GetTaskPercent() + dDelta);
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SAVING_DATA_DB);
				m_pProgressData->SetSubTask(str);
				PostMessage(m_hProgressWnd, WM_UPDATE_PROGRESS, 0, 0);
				ReleaseMutex(m_pProgressData->GetProgressDataMtx());

				// import business object into database and append it to the selected tree node.
				PartPtr pPart = boost::dynamic_pointer_cast<Part>(pBusinessData);
				DataItemPtr pNewDataItem = PartManager::SavePart(pPart, m_pParentFolder);
				m_pNewProductItemList->push_back(pNewDataItem);

				// update the progress data.
				WaitForSingleObject(m_pProgressData->GetProgressDataMtx(), INFINITE);
				m_pProgressData->SetTaskPercent(m_pProgressData->GetTaskPercent() + dDelta);
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SAVED_DATA_DB);
				m_pProgressData->SetSubTask(str);
				PostMessage(m_hProgressWnd, WM_UPDATE_PROGRESS, 0, 0);
				ReleaseMutex(m_pProgressData->GetProgressDataMtx());
			}
			/************************************************************************/
		}
		else if (impDataType == IMPEXP_DATA_TAOPRT)
		{
			IBusinessDataPtr pBusinessData = ImportManager::ImportPartObj(strImpFilePath);
			if (pBusinessData)
			{
				PartPtr pPart = boost::dynamic_pointer_cast<Part>(pBusinessData);
				DataItemPtr pNewDataItem = PartManager::SavePart(pPart, m_pParentFolder);
				m_pNewProductItemList->push_back(pNewDataItem);
			}
		}
		else if (impDataType == IMPEXP_DATA_TAOSHT)
		{
			vector<DataProperty> prtExpInfoList;
			IBusinessDataPtr pBusinessData = ImportManager::ImportSheetObj(strImpFilePath, prtExpInfoList);
			if (pBusinessData)
			{
				const Sheet* pSheet = dynamic_cast<const Sheet*>(pBusinessData.get());
				FolderNodePtr pNewFolderNode = SheetManager::ImportSheet(pSheet, m_pParentFolder, prtExpInfoList);
				m_pNewProductItemList->push_back(pNewFolderNode);
			}
		}

		// update the progress data.
		if (i == (m_pImpDataInfoList->size()-1))
		{
			WaitForSingleObject(m_pProgressData->GetProgressDataMtx(), INFINITE);
			m_pProgressData->SetTaskPercent(100);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_IMPORTED_ALL_DATA);
			m_pProgressData->SetSubTask(str);
			PostMessage(m_hProgressWnd, WM_UPDATE_PROGRESS, 0, 0);
			ReleaseMutex(m_pProgressData->GetProgressDataMtx());
		}
	}

	Sleep(1000);

	// update the progress data.
	WaitForSingleObject(m_pProgressData->GetProgressDataMtx(), INFINITE);
	m_pProgressData->IsFinished(TRUE);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_IMPORT_TASK_OK);
	m_pProgressData->SetSubTask(str);
	PostMessage(m_hProgressWnd, WM_UPDATE_PROGRESS, 0, 0);
	ReleaseMutex(m_pProgressData->GetProgressDataMtx());
}

END_CUTLEADER_NAMESPACE()
