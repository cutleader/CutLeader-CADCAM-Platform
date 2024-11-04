#include "StdAfx.h"
#include "ImpDataSelDlgEx.h"

#include "baseConst.h"
#include "LineArc2DList.h"
#include "PathHelper.h"
#include "DataBaseManager.h"

#include "FolderItem.h"
#include "ProductFolderDlg.h"
#include "ProductItemLoader.h"

#include "ParamConfigLoader.h"
#include "ExpertLibItemLoader.h"

#include "clDataImportExportResource.h"
#include "ImportManager.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ImpDataSelDlgEx, CFileDialog)

ImpDataSelDlgEx::ImpDataSelDlgEx(LPCTSTR lpszDefExt, DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
CFileDialog(TRUE, lpszDefExt, NULL, dwFlags,lpszFilter, pParentWnd, 0, FALSE)
{
	SetTemplate(0, IDD_clDataImportExportUI_SelectData);
}

ImpDataSelDlgEx::~ImpDataSelDlgEx()
{
}

void ImpDataSelDlgEx::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_ctrlPreviewWnd);
	DDX_Text(pDX, IDC_EDIT_SAVE_PATH, m_ctrlSavePath);
}

BEGIN_MESSAGE_MAP(ImpDataSelDlgEx, CFileDialog)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SAVE_PATH, OnSelectFolder)
END_MESSAGE_MAP()

BOOL ImpDataSelDlgEx::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	// ����
	MultiLanguageMgr::GetInstance()->TranslateDialog(this);

	// ��ʼ������Ŀ¼��
	m_ctrlSavePath = m_pSaveFolder->GetName();
	if (!m_bEnableFolder)
		((CWnd*)GetDlgItem(IDC_BTN_SAVE_PATH))->EnableWindow(FALSE);
	UpdateData(FALSE);

	// ��ʼ���ӿڡ�
	Rect2D geomRect(.0, 10.0, .0, 10.0);
	m_ctrlPreviewWnd.InitViewPort(geomRect);

	return TRUE;
}

void ImpDataSelDlgEx::OnFileNameChange()
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
	else if (impType == IMPEXP_DATA_TAOPRT)
    {
		pGeom2DList = ImportManager::GetPrtBaseGeom(strFilePath);
    }
	else if (impType == IMPEXP_DATA_TAOSHT)
    {
		pGeom2DList = ImportManager::GetShtBaseGeom(strFilePath);
    }

	// Ԥ��
	if (pGeom2DList && pGeom2DList->size() > 0)
	{
		// ��ʼ���ӿڡ�
		Rect2D geomRect = pGeom2DList->GetRectBox();
		m_ctrlPreviewWnd.FitRect(geomRect);

		// ����ͼ�Ρ�
		m_ctrlPreviewWnd.SetSolidGeom2DList(pGeom2DList);

		// ���ơ�
		m_ctrlPreviewWnd.OnPaint();
	}

	CFileDialog::OnFileNameChange();
}

BOOL ImpDataSelDlgEx::OnFileNameOK()
{
	// Ҫ�Ǵ򿪵���dxf/dwg����Ҫ���һ��
	//CString strFilePath = m_szFileName; // �����ѡʱ��ò�����ֵ�ǿյ�
	CString strFilePath = GetPathName();
	IMPEXP_DATA_TYPE impType = GetImpType(strFilePath);
	if (impType == IMPEXP_DATA_DXF || impType == IMPEXP_DATA_DWG)
	{
		if (m_strDxfFileName_loadedIn_OnFileNameChange != strFilePath) // ˵������ļ�û�б�OnFileNameChange����load��
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

void ImpDataSelDlgEx::OnDestroy()
{
	// �ͷ��ӿڡ�
	m_ctrlPreviewWnd.ReleaseViewPort();

	CFileDialog::OnDestroy();
}

void ImpDataSelDlgEx::OnSelectFolder()
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

IMPEXP_DATA_TYPE ImpDataSelDlgEx::GetImpType(CString strFilePath)
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

	return impType;
}

END_CUTLEADER_NAMESPACE()
