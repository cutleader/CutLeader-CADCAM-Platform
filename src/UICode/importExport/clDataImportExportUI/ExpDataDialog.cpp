#include "StdAfx.h"
#include "ExpDataDialog.h"

#include "baseConst.h"
#include "PathHelper.h"

#include "clDataImportExportResource.h"
#include "DxfDwgConvertor.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ExpDataDialog, CFileDialog)

ExpDataDialog::ExpDataDialog(LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, 
							 LPCTSTR lpszFilter, CWnd* pParentWnd) :
	CFileDialog(FALSE, lpszDefExt, lpszFileName, dwFlags,lpszFilter, pParentWnd)
{
}

ExpDataDialog::~ExpDataDialog()
{
}

BEGIN_MESSAGE_MAP(ExpDataDialog, CFileDialog)
END_MESSAGE_MAP()

BOOL ExpDataDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();
	
	return TRUE;
}

IMPEXP_DATA_TYPE ExpDataDialog::GetExpType()
{
	IMPEXP_DATA_TYPE expType;

	//
	CString strExt = GetFileExt();
	strExt.Insert(0, _T("."));

	//
	CString strTmp;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DXF_EXT);
	if (strExt == strTmp)
		expType = IMPEXP_DATA_DXF;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_DWG_EXT);
	if (strExt == strTmp)
		expType = IMPEXP_DATA_DWG;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_TAOPRT_EXT);
	if (strExt == strTmp)
		expType = IMPEXP_DATA_TAOPRT;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_TAOSHT_EXT);
	if (strExt == strTmp)
		expType = IMPEXP_DATA_TAOSHT;
	strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPEXP_EXPLIBITEM_EXT);
	if (strExt == strTmp)
		expType = IMPEXP_DATA_TAOEXPLIB;

	return expType;
}

BOOL ExpDataDialog::OnFileNameOK()
{
	return CFileDialog::OnFileNameOK();
}

END_CUTLEADER_NAMESPACE()
