#include "stdafx.h"
#include "afxwinappex.h"
#include "clApp.h"

#include "baseConst.h"
#include "IntegerList.h"
#include "ProcessMgr.h"

#include "ProductFileDefine.h"
#include "ProductFolderManager.h"
#include "clBaseDataResource.h"
#include "clKnowledgeBaseConfigResource.h"

#include "CLOptionSheet.h"
#include "clExpertLibResource.h"
#include "ParamConfigLoader.h"
#include "DefaultParamConfig.h"
#include "DflTplDlg.h"

void ClApp::OnCLOption()
{
	CLOptionSheet clOptionSheet;
	clOptionSheet.DoModal();
}

void ClApp::OnMacLib()
{
	/************************************************************************/
	// first, make sure one instance of cam config app is running.

	int iProcessID = INVALID_ID;

	// get the exe path of cam config app.
	CString strCAMConfigPath = ProductFolderManager::GetProductPath();
	strCAMConfigPath += _T(KB_CONFIG_EXE_FILE);

	// check whether cam config app is running.
	BOOL bRunning = FALSE;
	IntegerListPtr pProcessIDList = ProcessMgr::IsProcessRun(strCAMConfigPath);
	if (pProcessIDList->size() > 0)
	{
		bRunning = TRUE;
		iProcessID = (int)pProcessIDList->at(0);
	}

	// if no instance of cam config app is running, run it.
	if (!bRunning)
		iProcessID = ProcessMgr::RunProcess(strCAMConfigPath);
	/************************************************************************/

	/************************************************************************/
	// we should send msg to that instance to activate the proper doc.

	// get the main frame of the running app.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	HWND hMainWnd = ProcessMgr::GetProcessMainWnd(iProcessID, str);

	// send msg to the main frame of the running app.
	WndMsg wndMsg;
	wndMsg.iWndMsgType = WNDMSG_ACTIVE_DOC;
	wndMsg.iDocNameRes = IDS_MAC_MGR;
	ProcessMgr::SendCopyMsg(hMainWnd, wndMsg);
	/************************************************************************/
}

void ClApp::OnExpertLib()
{
	/************************************************************************/
	// first, make sure one instance of cam config app is running.

	int iProcessID = INVALID_ID;

	// get the exe path of cam config app.
	CString strCAMConfigPath = ProductFolderManager::GetProductPath();
	strCAMConfigPath += _T(KB_CONFIG_EXE_FILE);

	// check whether cam config app is running.
	BOOL bRunning = FALSE;
	IntegerListPtr pProcessIDList = ProcessMgr::IsProcessRun(strCAMConfigPath);
	if (pProcessIDList->size() > 0)
	{
		bRunning = TRUE;
		iProcessID = (int)pProcessIDList->at(0);
	}

	// if no instance of cam config app is running, run it.
	if (!bRunning)
		iProcessID = ProcessMgr::RunProcess(strCAMConfigPath);
	/************************************************************************/

	/************************************************************************/
	// we should send msg to that instance to activate the proper doc.

	// get the main frame of the running app.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	HWND hMainWnd = ProcessMgr::GetProcessMainWnd(iProcessID, str);

	// send msg to the main frame of the running app.
	WndMsg wndMsg;
	wndMsg.iWndMsgType = WNDMSG_ACTIVE_DOC;
	wndMsg.iDocNameRes = IDS_EXPERTLIB_MGR;
	ProcessMgr::SendCopyMsg(hMainWnd, wndMsg);
	/************************************************************************/
}

void ClApp::OnMatLib()
{
	/************************************************************************/
	// first, make sure one instance of cam config app is running.

	int iProcessID = INVALID_ID;

	// get the exe path of cam config app.
	CString strCAMConfigPath = ProductFolderManager::GetProductPath();
	strCAMConfigPath += _T(KB_CONFIG_EXE_FILE);

	// check whether cam config app is running.
	BOOL bRunning = FALSE;
	IntegerListPtr pProcessIDList = ProcessMgr::IsProcessRun(strCAMConfigPath);
	if (pProcessIDList->size() > 0)
	{
		bRunning = TRUE;
		iProcessID = (int)pProcessIDList->at(0);
	}

	// if no instance of cam config app is running, run it.
	if (!bRunning)
		iProcessID = ProcessMgr::RunProcess(strCAMConfigPath);
	/************************************************************************/

	/************************************************************************/
	// we should send msg to that instance to activate the proper doc.

	// get the main frame of the running app.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	HWND hMainWnd = ProcessMgr::GetProcessMainWnd(iProcessID, str);

	// send msg to the main frame of the running app.
	WndMsg wndMsg;
	wndMsg.iWndMsgType = WNDMSG_ACTIVE_DOC;
	wndMsg.iDocNameRes = IDS_MATLIB_MGR;
	ProcessMgr::SendCopyMsg(hMainWnd, wndMsg);
	/************************************************************************/
}

void ClApp::OnDflReportTpl()
{
	DflTplDlg dlg;
	dlg.DoModal();
}

void ClApp::OnDesignReportTpl()
{
	int iProcessID = INVALID_ID;

	// 报表模板设计器的路径。
	CString strPath = ProductFolderManager::GetProductPath();
	strPath += _T(TPL_DESIGN_EXE_FILE);

	// check whether cam config app is running.
	BOOL bRunning = FALSE;
	IntegerListPtr pProcessIDList = ProcessMgr::IsProcessRun(strPath);
	if (pProcessIDList->size() > 0)
	{
		bRunning = TRUE;
		iProcessID = (int)pProcessIDList->at(0);
	}

	// if no instance of cam config app is running, run it.
	if (!bRunning)
		iProcessID = ProcessMgr::RunProcess(strPath);
}