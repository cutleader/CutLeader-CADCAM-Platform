#include "stdafx.h"
#include "afxwinappex.h"
#include "clApp.h"

#include "PathHelper.h"
#include "clSecureResource.h"

#include "baseConst.h"
#include "clBaseDataResource.h"
#include "ProductFolderManager.h"
#include "ProductFileDefine.h"
#include "AboutDlg.h"
#include "HelpVideoDlg.h"

#include "CLOptionInfo.h"
#include "ProductParam.h"
#include "CLOptionWriter.h"

void ClApp::OnHelpVideo()
{
	ProductParamPtr pProductParam = CLOptionInfo::GetInstance()->GetProductParam();
	HelpVideoDlg* pDlg = new HelpVideoDlg(pProductParam->GetLang(), pProductParam->IsShowHelpVideo());
	pDlg->Create(IDD_clBaseDataUI_HelpVideo, m_pMainWnd);
	pDlg->ShowWindow(SW_SHOW);
}

void ClApp::OnHelpTopics()
{
	// language type
	ProductParamPtr pProductParam = CLOptionInfo::GetInstance()->GetProductParam();

	CString strSupprtFilePath = ProductFolderManager::GetHelpPath();
	strSupprtFilePath += _T("\\");
	if (pProductParam->GetLang() == LANG_CHS)
		strSupprtFilePath += HELP_CHS_FILE;
	else if (pProductParam->GetLang() == LANG_ENG)
		strSupprtFilePath += HELP_ENG_FILE;
	ShellExecute(NULL, _T("open"), strSupprtFilePath, NULL, NULL, SW_SHOWMAXIMIZED);
}

void ClApp::OnWebSite()
{
	ShellExecute(NULL, NULL, PRODUCT_WEBSITE, NULL, NULL, NULL);
}

void ClApp::OnAbout()
{
	AboutDlg aboutDlg;
	aboutDlg.DoModal();
}
