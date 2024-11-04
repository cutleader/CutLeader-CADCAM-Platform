#include "StdAfx.h"
#include "ImpExpHelper.h"

#include "DBConnect.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"

#include "MachineItem.h"
#include "MachineItemLoader.h"
#include "MachineItemWriter.h"

#include "ConditionValueList.h"
#include "PierceConfigItem.h"
#include "ExpertLibNode.h"
#include "ExpertLibItemLoader.h"
#include "ExpertLibItem.h"
#include "ExpertLibItemLoader.h"
#include "ExpertLibItemWriter.h"
#include "ParamConfig.h"
#include "ParamConfigList.h"
#include "NCConfigItem.h"
#include "NCConfigValueList.h"
#include "ToolAssignConfigItem.h"
#include "ToolInstance.h"
#include "ToolInstanceList.h"

#include "MaterialLibNode.h"
#include "Material.h"
#include "MaterialLibWriter.h"
#include "MaterialLibLoader.h"

#include "clDataImportExportResource.h"
#include "ExpDataDialog.h"
#include "ImpDataSelDlg.h"
#include "ExportManager.h"
#include "ImportManager.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

void ImpExpHelper::ExportExpLibItem(ExpertLibTree* pExpLibTree)
{
	CString str, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// �õ�ѡ�е�ר�ҿ��
	HTREEITEM hSelItem = pExpLibTree->GetSelectedItem();
	IExpertLibNode* pNodeData = (IExpertLibNode*)pExpLibTree->GetItemData(hSelItem);

	// ���ܵ���������ר�ҿ⡣
	DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iMacID = ExpertLibItemLoader::GetMacIDByExpLibItem(pExpLibDBCon, pNodeData->GetDataID());
	if (MachineItemLoader::GetReserverdMacItem(pMacLibDBConnect) == iMacID)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_RESERVED_EXPLIB);
		MessageBox(NULL, str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_EXPLIBITEM_FILEEXT);
	strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_EXPLIBITEM_EXT);
	ExpDataDialog expFileDlg(strDflExt, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (expFileDlg.DoModal() == IDOK)
	{
		// ����Ҫ������ר�ҿ⡣
		ExpertLibItemPtr pExpLibItem = ExpertLibItemLoader::LoadExpLibItem(pExpLibDBCon, pNodeData->GetDataID(), EXPLIBITEM_LOAD_ALL);

		// ����ר�ҿ⵽ָ��Ŀ¼��
		CString strFilePath = expFileDlg.GetPathName();
		if (ExportManager::ExportExpLibItem(pExpLibItem, strFilePath))
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_EXPLIB_SUCCESS);
			MessageBox(NULL, str, strProductName, MB_OK | MB_ICONINFORMATION);
		}
	}
}

void ImpExpHelper::ImportExpLibItem(ExpertLibTree* pExpLibTree)
{
	DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	DBConnectPtr pExpLibConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();

	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_EXPLIBITEM_FILEEXT);
	strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_EXPLIBITEM_EXT);
	ImpDataSelDlg impFileDlg(FALSE, strDflExt, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (impFileDlg.DoModal() == IDOK)
	{
		// �ӵ����ļ��м���ר�ҿ⡣
		CString strFilePath = impFileDlg.GetPathName();
		ExpertLibItemPtr pExpLibItem = ImportManager::ImportExpLibItem(strFilePath);
		if (!pExpLibItem)
			return;

		// ���д��ڵĻ�������
		vector<DataProperty> propList;
		MachineItemLoader::GetMacItemPropList(pMacLibDBConnect, propList);
		StringListPtr pMacItemNameList(new vector<CString>);
		for (unsigned int i = 0; i < propList.size(); i++)
		{
			DataProperty prop = propList.at(i);
			pMacItemNameList->push_back(prop.GetItem(1).GetValue().bstrVal);
		}

		// �������е�ר�ҿ�����
		StringListPtr pExpLibItemNameList(new vector<CString>);
		vector<DataProperty> expLibItemPropList = ExpertLibItemLoader::GetExpLibItemInfoList(pExpLibConnect);
		for (unsigned int i = 0; i < expLibItemPropList.size(); i++)
		{
			DataProperty prop = expLibItemPropList[i];
			pExpLibItemNameList->push_back(prop.GetItem(1).GetValue().bstrVal);
		}

		/************************************************************************/
		// ���Ҫ��Ҫ��¡������Ϣ��ר�ҿ���Ϣ��

		MachineItemPtr pMacItem = pExpLibItem->GetMacItem();

		// ���Ҫ��Ҫ��¡������Ϣ��
		MachineItemPtr pNewMacItem = pMacItem;
		if (MachineItemLoader::LoadMacItem(pMacLibDBConnect, pMacItem->GetID()))
			pNewMacItem = boost::dynamic_pointer_cast<MachineItem>(pMacItem->Clone());

		// ���Ҫ��Ҫ��¡ר�ҿ���Ϣ��
		ExpertLibItemPtr pNewExpLibItem = pExpLibItem;
		if (ExpertLibItemLoader::GetMacIDByExpLibItem(pExpLibConnect, pExpLibItem->GetID()) != INVALID_ID)
		{
			// ��¡ר�ҿ⡣
			pNewExpLibItem = boost::dynamic_pointer_cast<ExpertLibItem>(pExpLibItem->Clone());
			pNewExpLibItem->SetMacItem(pNewMacItem);

			// ������ר�ҿ��е�һЩ��Ϣ��
			ConditionTypeListPtr pOldCondTypeList = pMacItem->GetCondTypeList();
			ConditionTypeListPtr pNewCondTypeList = pNewMacItem->GetCondTypeList();
			NCConfigTypeListPtr pOldNCConfigTypes = pMacItem->GetNCConfigTypeList();
			NCConfigTypeListPtr pNewNCConfigTypes = pNewMacItem->GetNCConfigTypeList();
			ParamConfigListPtr pParamConfigList = pNewExpLibItem->GetParamConfigList();
			for (unsigned int i = 0; i < pParamConfigList->size(); i++)
			{
				ParamConfigPtr pParamConfig = pParamConfigList->at(i);

                // ���������е���������Ҫ��Ҫ�滻
                {
                    ToolAssignConfigItemPtr pToolAssignConfigItem = boost::dynamic_pointer_cast<ToolAssignConfigItem>(pParamConfig->GetItemByType(PARAMCONFIG_ITEM_TOOL_ASSIGN));
                    ToolInstanceListPtr pToolInstanceList = pToolAssignConfigItem->GetToolInstanceList();
                    for (unsigned int j = 0; j < pToolInstanceList->size(); j++)
                    {
                        ToolInstancePtr pToolInstance = pToolInstanceList->at(j);
                        pToolInstance->GetCondValList()->ReplaceCondType(pOldCondTypeList, pNewCondTypeList);
                    }
                }

				NCConfigItemPtr pNCConfigItem = boost::dynamic_pointer_cast<NCConfigItem>(pParamConfig->GetItemByType(PARAMCONFIG_ITEM_GENERATE_NC));
				NCConfigValueListPtr pNCConfigValList = pNCConfigItem->GetNCConfigVal();
				pNCConfigValList->ReplaceNCConfigType(pOldNCConfigTypes, pNewNCConfigTypes);
			}
		}
		else
		{
			pNewExpLibItem->SetMacItem(pNewMacItem);
		}

		// check the name.
		CString strNewName;
		if (CheckName(pMacItemNameList, pNewMacItem->GetName(), strNewName))
			pNewMacItem->SetName(strNewName);
		if (CheckName(pExpLibItemNameList, pNewExpLibItem->GetName(), strNewName))
			pNewExpLibItem->SetName(strNewName);
		/************************************************************************/

		// ���������Ϣ��ר�ҿ⡣
		MachineItemWriter::SaveMacItem(pMacLibDBConnect, pNewMacItem, TRUE);
		ExpertLibItemWriter::SaveExpLibItem(pExpLibConnect, pNewExpLibItem);

		/************************************************************************/
		// �½����ڵ㡣

		// init the expert library item node.
		ExpLibNodePtr pExpLibItemNode(new ExpertLibNode(EXPLIB_NODE_EXPLIBITEM));
		pExpLibItemNode->SetDataID(pNewExpLibItem->GetID());
		pExpLibItemNode->SetNodeName(pNewExpLibItem->GetName());
		ExpertLibItemLoader::InitExpLibItem(pExpLibConnect, pExpLibItemNode);

		// create tree node.
		HTREEITEM hExpLibItemNode = pExpLibTree->AppendExpLibItemNode(pExpLibItemNode);

		// select the ExpertLibItem node.
		pExpLibTree->Select(hExpLibItemNode, TVGN_CARET);
		pExpLibTree->SetFocus();
		/************************************************************************/
	}
}

void ImpExpHelper::ExportMat(MaterialLibTree* pMatLibTree)
{
	// ѡ�еĲ��Ͻڵ㡣
	HTREEITEM hSelItem = pMatLibTree->GetSelectedItem();
	IMaterialLibNode* pNodeData = (IMaterialLibNode*)pMatLibTree->GetItemData(hSelItem);

	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_MATLIB_FILEEXT);
	strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_MATLIB_EXT);
	ExpDataDialog expFileDlg(strDflExt, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (expFileDlg.DoModal() == IDOK)
	{
		// ����Ҫ�����Ĳ��ϡ�
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		MaterialListPtr pMaterialList = MaterialLibLoader::LoadAllMat(pMatLibDBCon);
		MaterialPtr pMat = pMaterialList->GetItemByID(pNodeData->GetDataID());

		// ����ר�ҿ⵽ָ��Ŀ¼��
		CString strFilePath = expFileDlg.GetPathName();
		if (ExportManager::ExportMat(pMat, strFilePath))
		{
			CString str, strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_MATLIB_SUCCESS);
			MessageBox(NULL, str, strProductName, MB_OK | MB_ICONINFORMATION);
		}
	}
}

void ImpExpHelper::ImportMat(MaterialLibTree* pMatLibTree)
{
	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_MATLIB_FILEEXT);
	strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_MATLIB_EXT);
	ImpDataSelDlg impFileDlg(FALSE, strDflExt, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (impFileDlg.DoModal() == IDOK)
	{
		// �ӵ����ļ��м��ز��ϡ�
		CString strFilePath = impFileDlg.GetPathName();
		MaterialPtr pMaterial = ImportManager::ImportMat(strFilePath);
		if (!pMaterial)
			return;

		// ��鵱ǰ���ݿ��ǲ�����IDһ���Ĳ��ϡ�
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		DataProperty prop;
		if (MaterialLibLoader::GetMatProp(pMatLibDBCon, pMaterial->GetID(), prop))
			pMaterial = boost::dynamic_pointer_cast<Material>(pMaterial->Clone());

		// ������ϡ�
		MaterialLibWriter::SaveMat(pMatLibDBCon, pMaterial, TRUE, TRUE);

		// ���ϲ��Ͻڵ㡣
		IMaterialLibNodePtr pMatLibNode = MaterialLibLoader::LoadMatTreeNode(pMatLibDBCon, pMaterial->GetID());
		HTREEITEM hMatNode = pMatLibTree->AppendMatNode(pMatLibNode);
		pMatLibTree->Select(hMatNode, TVGN_CARET);
		pMatLibTree->SetFocus();
	}
}

BOOL ImpExpHelper::CheckName(StringListPtr pExistNameList, CString strName, CString& strNewName)
{
	BOOL bConflict = FALSE;

	/************************************************************************/
	// check whether "strName" existed in "pExistNameList".

	for (unsigned int i = 0; i < pExistNameList->size(); i++)
	{
		CString strTmpName = pExistNameList->at(i);

		if (strTmpName == strName)
		{
			bConflict = TRUE;
			break;
		}
	}
	/************************************************************************/

	/************************************************************************/
	// if "strName" conflict with an existing string, try to change the name
	// to get a legal one.

	if (bConflict)
	{
		for (int i = 1; ; i++)
		{
			// build the postfix.
			CString strPostfix = _T("_");
			CString strTmp;
			strTmp.Format(_T("%d"), i);
			strPostfix += strTmp;

			// the new name
			CString strTmpNewName = strName;
			strTmpNewName += strPostfix;

			// check the new name.
			BOOL bStillConflict = FALSE;
			for (unsigned int j = 0; j < pExistNameList->size(); j++)
			{
				CString strTmpName = pExistNameList->at(j);

				if (strTmpName == strTmpNewName)
				{
					bStillConflict = TRUE;
					break;
				}
			}

			// if the new name is OK, break.
			if (!bStillConflict)
			{
				strNewName = strTmpNewName;
				break;
			}
		}
	}
	/************************************************************************/

	return bConflict;
}

END_CUTLEADER_NAMESPACE()
