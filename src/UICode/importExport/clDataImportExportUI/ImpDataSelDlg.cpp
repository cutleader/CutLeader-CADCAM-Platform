#include "StdAfx.h"
#include "ImpDataSelDlg.h"

#include "baseConst.h"
#include "LineArc2DList.h"
#include "PathHelper.h"

#include "clDataImportExportResource.h"
#include "ImportManager.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ImpDataSelDlg, CFileDialog)

ImpDataSelDlg::ImpDataSelDlg(BOOL bNeedPreview, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, 
							 LPCTSTR lpszFilter, CWnd* pParentWnd) :
CFileDialog(TRUE, lpszDefExt, lpszFileName, dwFlags,lpszFilter, pParentWnd, 0, FALSE)
{
	m_bNeedPreview = bNeedPreview;
}

ImpDataSelDlg::~ImpDataSelDlg()
{
}

BEGIN_MESSAGE_MAP(ImpDataSelDlg, CFileDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL ImpDataSelDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	if (m_bNeedPreview)
	{
		// 创建预览窗口。
		CRect rect;
		CWnd* parent = GetParent();
		parent->GetWindowRect(&rect);
		parent->SetWindowPos(0, rect.left, rect.top, rect.Width()+200, rect.Height(), 0);
		ScreenToClient(rect);
		CRect staticWinRect(rect.Width(), rect.top+65, rect.Width()+200-20, rect.Height()-100);
		m_previewStatic.Create(NULL, WS_CHILD|WS_VISIBLE|WS_BORDER|WS_BORDER, staticWinRect, parent);

		// 初始化视口。
		Rect2D geomRect(.0, 10.0, .0, 10.0);
		m_previewStatic.InitViewPort(geomRect);
	}

	return TRUE;
}

void ImpDataSelDlg::OnFileNameChange()
{
	if (m_bNeedPreview)
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

		// preview line/arcs.
		if (pGeom2DList && pGeom2DList->size() > 0)
		{
			// 调整视口以适合矩形框。
			Rect2D geomRect = pGeom2DList->GetRectBox();
			m_previewStatic.FitRect(geomRect);

			// 设置图形。
			m_previewStatic.SetSolidGeom2DList(pGeom2DList);

			// 绘制。
			m_previewStatic.Invalidate();
		}
	}

	CFileDialog::OnFileNameChange();
}

void ImpDataSelDlg::OnDestroy()
{
	// 释放视口。
	m_previewStatic.ReleaseViewPort();

	CFileDialog::OnDestroy();
}

BOOL ImpDataSelDlg::OnFileNameOK()
{
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

IMPEXP_DATA_TYPE ImpDataSelDlg::GetImpType(CString strFilePath)
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

END_CUTLEADER_NAMESPACE()
