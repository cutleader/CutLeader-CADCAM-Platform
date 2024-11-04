#include "StdAfx.h"
#include "ImpHelper.h"

#include "DateHelper.h"
#include "PathHelper.h"
#include "ProgressData.h"
#include "ProgressDlg.h"
#include "DataBaseManager.h"
#include "FolderItem.h"
#include "ProductItemList.h"
#include "ProductItemLoader.h"
#include "clBaseDataResource.h"
#include "LogMgr.h"

#include "PatternLoopList.h"
#include "LoopProcessor.h"
#include "Part.h"
#include "PartCadData.h"
#include "PartCamDataList.h"
#include "PartItem.h"
#include "PartItemList.h"
#include "PartManager.h"
#include "PartBuilder.h"
#include "Loop2PartDlg.h"

#include "ImportDataInfo.h"
#include "ImportDataInfoList.h"
#include "ImpDataSelDlgEx.h"
#include "ExtImpDataSelDlg.h"
#include "clDataImportExportResource.h"
#include "ImportManager.h"

#include "ImpDataTask.h"
#include "clDataManagerResource.h"

#include "ExpertLibItemLoader.h"
#include "ParamConfigLoader.h"
#include "DefaultParamConfig.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

ProductItemListPtr ImpHelper::ImportData(FolderNodePtr pFolderNode, BOOL bCanChangeFolder)
{
	ProductItemListPtr pNewProductItemList;

	/************************************************************************/
	// �õ�Ҫ��������ݡ�

	ImportDataInfoListPtr pImpDataInfoList(new ImportDataInfoList);

	// ѡ�����ݡ�
	CString strFileName, strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_FILEEXT);
	ImpDataSelDlgEx impFileDlg(strDflExt, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT, strFilter);
	int iBufferSize = 100*1000*MAX_PATH;
	impFileDlg.m_ofn.lpstrFile = strFileName.GetBuffer(iBufferSize);
	impFileDlg.m_ofn.nMaxFile = iBufferSize;
	impFileDlg.SetSavePath(pFolderNode, bCanChangeFolder);
	if (impFileDlg.DoModal() == IDOK)
	{
        // �õ�ѡ�е��ļ���
        int iSelectedFileCount = 0;
        {
            POSITION pos = impFileDlg.GetStartPosition();
            while (pos != NULL)
            {
                iSelectedFileCount++;
                impFileDlg.GetNextPathName(pos);
            }
        }

		POSITION pos = impFileDlg.GetStartPosition();
		while (pos != NULL)
		{
			CString strFilePath = impFileDlg.GetNextPathName(pos);
			ImportDataInfoPtr pImpDataInfo(new ImportDataInfo(strFilePath, impFileDlg.GetImpType(strFilePath)));
            if (iSelectedFileCount == 1)
            {
                pImpDataInfo->SetDxfData(impFileDlg.GetDxfData());
            }
			pImpDataInfoList->push_back(pImpDataInfo);
		}
		pFolderNode = impFileDlg.GetSavePath();
	}
	strFileName.ReleaseBuffer();

	// ��顣
	if (pImpDataInfoList->size() == 0)
		return pNewProductItemList;
	/************************************************************************/

	/************************************************************************/
	// ����������ִ�е��롣

	// init the task.
	ImpDataTaskPtr pImpDataTask(new ImpDataTask(pFolderNode, pImpDataInfoList));

	// display the progress of the task.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_IMPORT_DATA);
	ProgressDlg progressDlg;
	progressDlg.Init(str, pImpDataTask);
	progressDlg.DoModal();

	// get the result of the task.
	pNewProductItemList = pImpDataTask->GetNewProductItemList();
	/************************************************************************/

	return pNewProductItemList;
}

ProductItemListPtr ImpHelper::SplitImportPart(FolderNodePtr pFolderNode, BOOL bCanChangeFolder)
{
	ProductItemListPtr pNewProductItemList;

	// ׼�����ݡ�
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	DefaultParamConfigPtr pDflParamConfig = ParamConfigLoader::LoadDflParamConfig(pExpLibDBCon);

	// ѡ��Ҫ�����dxf/dwg�ļ���
	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_DXFDWG_FILEEXT);
	strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DXF_EXT);
	ExtImpDataSelDlg impFileDlg(strDflExt, OFN_HIDEREADONLY|OFN_FILEMUSTEXIST, strFilter);
	impFileDlg.SetSavePath(pFolderNode, bCanChangeFolder);
	impFileDlg.SetExpLibItem(pDflParamConfig->GetExpLibItemID(), TRUE);
	impFileDlg.SetParamConfig(pDflParamConfig->GetParamConfigID(), TRUE);
	if (impFileDlg.DoModal() == IDOK)
	{
		CString strFilePath = impFileDlg.GetPathName();
		CString strName = PathHelper::GetFileName(strFilePath);
		IMPEXP_DATA_TYPE impFileType = impFileDlg.GetImpType(strFilePath);
		LONGLONG iParamConfigID = impFileDlg.GetPartScheme();

        // �õ�dxf data
        DxfDataPtr pDxfData = impFileDlg.GetDxfData();
        if (!pDxfData)
        {
            LogMgr::DebugWarn(_T("Debug warning 1 in SplitImportPart function!"), &impFileDlg);
			return pNewProductItemList;
        }

		// ���е��������
		PartItemListPtr pPartItemList(new PartItemList);

		// ����dxf/dwg�ļ�ΪPartCadData���󣬴�ʱ��Ҫ�������л�·�������Ӱ�����˹�ϵʶ��
		PartCadDataPtr pPartCadData = ImportManager::DxfDwg2CadData(strFilePath, pDxfData.get(), FALSE);

		LoopTopologyItemListPtr pLoopTopItemList = LoopProcessor::CalcLoopTopStruct(pPartCadData->GetPatternLoopList().get());
        if (!LoopProcessor::NeedToSeperate(pLoopTopItemList.get()))
		{
			// ���á����cad���ݡ�����������������������
			PartPtr pPart = PartBuilder::BuildPart(strName, pPartCadData, iParamConfigID, TRUE);
			PartItemPtr pPartItem(new PartItem(pPart->GetID(), strName, pPartCadData, pPart->GetCamDataList()->at(0)));
			pPartItemList->push_back(pPartItem);
		}
		else
		{
			// ����Ի������ڴӻ�·ʶ�������
			Loop2PartDlg dlg;
			dlg.Init(pLoopTopItemList, strName, FALSE);
			if (dlg.DoModal() == IDC_BTN_ACCEPT)
			{
				// �õ�ʶ�����������ϣ���Щ����з�յ���������
				BOOL bRecognizeSamePart = false;
				vector<pair<CString, PatternLoopListPtr> > pPartList = dlg.GetRecognizedParts(bRecognizeSamePart);

				// �������С�������
				for (unsigned int i = 0; i < pPartList.size(); i++)
				{
					PatternListPtr pPatList = pPartList.at(i).second->GetPatList();
					PartCadDataPtr pPartCadData(new PartCadData(pPatList, pPartList.at(i).second));
					PartPtr pPart = PartBuilder::BuildPart(pPartList.at(i).first, pPartCadData, iParamConfigID, TRUE);
					PartItemPtr pPartItem(new PartItem(pPart->GetID(), pPart->GetName(), pPartCadData, pPart->GetCamDataList()->at(0)));
					pPartItemList->push_back(pPartItem);
				}
			}
			else
				return pNewProductItemList; // �˳���
		}

		// ���������
		pNewProductItemList.reset(new ProductItemList);
		for (unsigned int i = 0; i < pPartItemList->size(); i++)
		{
			PartItemPtr pPartItem = pPartItemList->at(i);
			PartPtr pPart(new Part(pPartItem->GetPartID(), pPartItem->GetPartName(), pPartItem->GetPartComment(), pPartItem->GetCadData(), pPartItem->GetCamData()));
			pPart->SetCreateTime(DateHelper::GetCurrentTime());
			pPart->SetModifyTime(DateHelper::GetCurrentTime());
			DataItemPtr pNewDataItem = PartManager::SaveNewPart(pPart.get(), impFileDlg.GetSavePath());
			pNewProductItemList->push_back(pNewDataItem);
		}

		// ��ʾ��Ϣ��
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_IMPORTED_ALL_DATA);
		MessageBox(NULL, str, strProductName, MB_OK|MB_ICONINFORMATION);
	}

	return pNewProductItemList;
}

END_CUTLEADER_NAMESPACE()
