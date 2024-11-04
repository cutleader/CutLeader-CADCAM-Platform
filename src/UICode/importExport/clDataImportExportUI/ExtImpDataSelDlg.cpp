#include "StdAfx.h"
#include "ExtImpDataSelDlg.h"

#include "baseConst.h"
#include "LineArc2DList.h"
#include "PathHelper.h"
#include "DataBaseManager.h"
#include "IntegerList.h"

#include "FolderItem.h"
#include "ProductFolderDlg.h"
#include "ProductItemLoader.h"

#include "ParamConfigLoader.h"
#include "ExpertLibItemLoader.h"
#include "MachineItemLoader.h"

#include "clDataImportExportResource.h"
#include "ImportManager.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ExtImpDataSelDlg, CFileDialog)

ExtImpDataSelDlg::ExtImpDataSelDlg(LPCTSTR lpszDefExt, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
CFileDialog(TRUE, lpszDefExt, NULL, dwFlags,lpszFilter, pParentWnd, 0, FALSE)
{
	SetTemplate(0, IDD_clDataImportExportUI_SelectExternalData);
	m_iParamConfigID = INVALID_ID;
}

ExtImpDataSelDlg::~ExtImpDataSelDlg()
{
}

void ExtImpDataSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_ctrlSavePath);
	DDX_Control(pDX, IDC_COMBO_EXPLIB_ITEM, m_ctrlExpLibItem);
	DDX_Control(pDX, IDC_COMBO_PART_SCHEME, m_ctrlPartScheme);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_ctrlPreviewWnd);
}

BEGIN_MESSAGE_MAP(ExtImpDataSelDlg, CFileDialog)
	ON_WM_DESTROY()

	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, OnSelectFolder)
	ON_CBN_SELCHANGE(IDC_COMBO_EXPLIB_ITEM, OnSelchangeExpLibItem)
END_MESSAGE_MAP()

BOOL ExtImpDataSelDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	// 翻译
	MultiLanguageMgr::GetInstance()->TranslateDialog(this);

	// 初始化视口。
	Rect2D geomRect(.0, 10.0, .0, 10.0);
	m_ctrlPreviewWnd.InitViewPort(geomRect);

	// 初始化保存目录。
	m_ctrlSavePath = m_pSaveFolder->GetName();
	if (!m_bEnableFolder)
		((CWnd*)GetDlgItem(IDC_BTN_SAVE_PATH))->EnableWindow(FALSE);

	// 初始化专家库项。
	DBConnectPtr pMacLibDBConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	IntegerListPtr pVisibleMacList = MachineItemLoader::GetVisibleMacList(pMacLibDBConnect);
	vector<DataProperty> expLibItemList = ExpertLibItemLoader::GetExpLibItemInfoList(pExpLibDBCon);
	int iCurIdx = 0;
	for (unsigned int i = 0; i < expLibItemList.size(); i++)
	{
		DataProperty prop = expLibItemList[i];

		// 检查对应的机器是否可见。
		if (!pVisibleMacList->Include(prop.GetItem(3).GetValue().llVal))
			continue;

		m_ctrlExpLibItem.AddString((LPCTSTR)prop.GetItem(1).GetValue().bstrVal);
		LONGLONG iExpLibItemID = prop.GetItem(0).GetValue().llVal;
		m_ctrlExpLibItem.SetItemData(m_ctrlExpLibItem.GetCount()-1, (DWORD_PTR)(new LONGLONG(iExpLibItemID)));
		if (iExpLibItemID == m_iExpLibItemID)
			iCurIdx = m_ctrlExpLibItem.GetCount()-1;
	}
	m_ctrlExpLibItem.SetCurSel(iCurIdx);
	if (!m_bEnableExpLib)
		((CWnd*)GetDlgItem(IDC_COMBO_EXPLIB_ITEM))->EnableWindow(FALSE);

	// 初始化参数配置。
	InitParamConfigCBX(m_iParamConfigID);
	if (!m_bEnableConfig)
		((CWnd*)GetDlgItem(IDC_COMBO_PART_SCHEME))->EnableWindow(FALSE);

	UpdateData(FALSE);

	return TRUE;
}

void ExtImpDataSelDlg::OnDestroy()
{
	RlsExpLibItemData();
	RlsPartSchemeItemData();

	// 释放视口。
	m_ctrlPreviewWnd.ReleaseViewPort();
}

void ExtImpDataSelDlg::OnSelectFolder()
{
	ProductFolderDlg dlg(this);
	if (dlg.DoModal() == IDOK)
	{
		m_pSaveFolder = dlg.GetSelectFolder();

		// get the path of the folder.
		CString strPath = m_pSaveFolder->GetItemPath();
		strPath += _T("\\");
		strPath += m_pSaveFolder->GetName();

		m_ctrlSavePath = strPath;
		UpdateData(FALSE);
	}
}

void ExtImpDataSelDlg::OnSelchangeExpLibItem()
{
	int iIndex = m_ctrlExpLibItem.GetCurSel();
	if (iIndex != -1)
		InitParamConfigCBX(INVALID_ID);
}

void ExtImpDataSelDlg::OnFileNameChange()
{
	// get the selected file type.
	CString strFilePath = GetPathName();
	IMPEXP_DATA_TYPE impType = GetImpType(strFilePath);
	if (impType == IMPEXP_DATA_NOT_INIT)
		return;

	// get the basic geoms of the selected file.
	LineArc2DListPtr pGeom2DList;
	if (impType == IMPEXP_DATA_DXF)
    {
        pair<LineArc2DListPtr, DxfDataPtr> pairData = ImportManager::GetDxfDwgBaseGeomsAndDxfData(strFilePath, FALSE);
        pGeom2DList = pairData.first;
        m_pDxfData = pairData.second;
		m_strDxfFileName_loadedIn_OnFileNameChange = strFilePath;
    }
	else if (impType == IMPEXP_DATA_DWG)
    {
        pair<LineArc2DListPtr, DxfDataPtr> pairData = ImportManager::GetDxfDwgBaseGeomsAndDxfData(strFilePath, TRUE);
        pGeom2DList = pairData.first;
        m_pDxfData = pairData.second;
		m_strDxfFileName_loadedIn_OnFileNameChange = strFilePath;
    }

	// 预览
	if (pGeom2DList && pGeom2DList->size() > 0)
	{
		// 初始化视口。
		Rect2D geomRect = pGeom2DList->GetRectBox();
		m_ctrlPreviewWnd.FitRect(geomRect);

		// 设置图形。
		m_ctrlPreviewWnd.SetSolidGeom2DList(pGeom2DList);

		// 绘制。
		m_ctrlPreviewWnd.Invalidate();
	}

	CFileDialog::OnFileNameChange();
}

BOOL ExtImpDataSelDlg::OnFileNameOK()
{
	// 选择的零件方案。
	int iIndex = m_ctrlPartScheme.GetCurSel();
	m_iParamConfigID = *((LONGLONG*)m_ctrlPartScheme.GetItemData(iIndex));

	// 要是打开的是dxf/dwg，需要检查一下
	//CString strFilePath = m_szFileName; // 允许多选时，貌似这个值是空的
	CString strFilePath = GetPathName();
	IMPEXP_DATA_TYPE impType = GetImpType(strFilePath);
	if (impType == IMPEXP_DATA_DXF || impType == IMPEXP_DATA_DWG)
	{
		if (m_strDxfFileName_loadedIn_OnFileNameChange != strFilePath) // 说明这个文件没有被OnFileNameChange函数load过
		{
			if (impType == IMPEXP_DATA_DXF)
			{
				pair<LineArc2DListPtr, DxfDataPtr> pairData = ImportManager::GetDxfDwgBaseGeomsAndDxfData(strFilePath, FALSE);
				m_pDxfData = pairData.second;
			}
			else if (impType == IMPEXP_DATA_DWG)
			{
				pair<LineArc2DListPtr, DxfDataPtr> pairData = ImportManager::GetDxfDwgBaseGeomsAndDxfData(strFilePath, TRUE);
				m_pDxfData = pairData.second;
			}
		}
	}

	return CFileDialog::OnFileNameOK();
}

void ExtImpDataSelDlg::SetSavePath(FolderNodePtr pFolderNode, BOOL bEnable)
{ 
	m_pSaveFolder = pFolderNode;
	m_bEnableFolder = bEnable;
}

void ExtImpDataSelDlg::SetExpLibItem(LONGLONG iExpLibItemID, BOOL bEnable)
{
	m_iExpLibItemID = iExpLibItemID;
	m_bEnableExpLib = bEnable;
}

void ExtImpDataSelDlg::SetParamConfig(LONGLONG iParamConfigID, BOOL bEnable)
{ 
	m_iParamConfigID = iParamConfigID;
	m_bEnableConfig = bEnable;
}

IMPEXP_DATA_TYPE ExtImpDataSelDlg::GetImpType(CString strFilePath) const
{
	IMPEXP_DATA_TYPE impType = IMPEXP_DATA_NOT_INIT;

	// get the ext of the file.
	CString strExt = PathHelper::GetFileExt(strFilePath);

	CString strTmp;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DXF_EXT);
	if (strExt.CompareNoCase(strTmp) == 0)
		impType = IMPEXP_DATA_DXF;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DWG_EXT);
	if (strExt.CompareNoCase(strTmp) == 0)
		impType = IMPEXP_DATA_DWG;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_TAOPRT_EXT);
	if (strExt.CompareNoCase(strTmp) == 0)
		impType = IMPEXP_DATA_TAOPRT;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_TAOSHT_EXT);
	if (strExt.CompareNoCase(strTmp) == 0)
		impType = IMPEXP_DATA_TAOSHT;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_EXPLIBITEM_EXT);
	if (strExt == strTmp)
		impType = IMPEXP_DATA_TAOEXPLIB;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_BK_DB_EXT);
	if (strExt == strTmp)
		impType = IMPEXP_DATA_BK_DB;

	return impType;
}

void ExtImpDataSelDlg::InitParamConfigCBX(LONGLONG iCurParamConfigID)
{
	// at first, clear the combo box.
	m_ctrlPartScheme.ResetContent();
	RlsPartSchemeItemData();

	// 当前选择的专家库项。
	int iIndex = m_ctrlExpLibItem.GetCurSel();
	LONGLONG* pExpLibItemID = (LONGLONG*)m_ctrlExpLibItem.GetItemData(iIndex);

	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	vector<DataProperty> paramConfigList = ParamConfigLoader::GetParamConfigList(pExpLibDBCon, *pExpLibItemID);
	int iCurIdx = 0;
	for (unsigned int i = 0; i < paramConfigList.size(); i++)
	{
		DataProperty prop = paramConfigList[i];

		m_ctrlPartScheme.AddString((LPCTSTR)prop.GetItem(2).GetValue().bstrVal);
		LONGLONG iParamConfigID = prop.GetItem(0).GetValue().llVal;
		m_ctrlPartScheme.SetItemData(i, (DWORD_PTR)(new LONGLONG(iParamConfigID)));
		if (iParamConfigID == iCurParamConfigID)
			iCurIdx = i;
	}
	m_ctrlPartScheme.SetCurSel(iCurIdx);
}

void ExtImpDataSelDlg::RlsExpLibItemData()
{
	for (int i = 0; i < m_ctrlExpLibItem.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_ctrlExpLibItem.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void ExtImpDataSelDlg::RlsPartSchemeItemData()
{
	for (int i = 0; i < m_ctrlPartScheme.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_ctrlPartScheme.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

END_CUTLEADER_NAMESPACE()
