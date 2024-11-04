#include "stdafx.h"
#include "afxwinappex.h"
#include "clApp.h"

#include "ProcessMgr.h"
#include "IntegerList.h"
#include "DateHelper.h"
#include "RandomGen.h"
#include "InteractBar.h"
#include "UnitHelper.h"
#include "DBConnect.h"

#include "clUILibResource.h"
#include "ProductFileDefine.h"
#include "ProductFolderManager.h"
#include "clBaseDataResource.h"
#include "clSecureResource.h"
#include "HelpVideoDlg.h"
#include "DataBaseManager.h"
#include "LogMgr.h"
#include "DataBaseManager.h"
#include "baseConst.h"
#include "clSecureResource.h"
#include "Polygon2DProcessor.h"
#include "MultiLanguageMgr.h"
#include "ProductFileManager.h"
#include "LicenseManager.h"

#include "RecentDataLoader.h"
#include "RecentDataWriter.h"
#include "MainFrame.h"
#include "ChildFrm.h"
#include "UIManager.h"
#include "DataItemTracker.h"
#include "FolderContentPage.h"
#include "clDataImportExportResource.h"

#include "StartPageDoc.h"
#include "StartPageFrame.h"
#include "StartPageView.h"
#include "PartFrame.h"
#include "PartDocument.h"
#include "PartView.h"
#include "SheetFrame.h"
#include "SheetDocument.h"
#include "SheetView.h"

#include "CLOptionSheet.h"
#include "CLOptionLoader.h"
#include "CLOptionWriter.h"
#include "SheetEditParam.h"
#include "ProductParam.h"

#include "ConfigItemWriter.h"
#include "clMaterialLibResource.h"
#include "clExpertLibResource.h"
#include "clOptionResource.h"
#include "clKnowledgeBaseConfigResource.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "CutNodeSimulator.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(ClApp, CWinAppEx)
	ON_COMMAND(IDC_DATA_MANAGEMENT, OnDataMgr)
	ON_COMMAND(IDC_TAO_FILE_SAVE, OnFileSave)
	ON_COMMAND(IDC_TAO_FILE_SAVEAS, OnFileSaveAs)

	ON_COMMAND(IDC_NEW_PART, OnNewPart)
	ON_COMMAND(IDC_OPEN_DXF, OnOpenDxf)
	ON_COMMAND(IDC_NEW_SHEET, OnNewSheet)
	ON_COMMAND(IDC_NEW_NESTJOB, OnNewNestJob)
	ON_COMMAND(IDC_IMPORT, OnImport)
	ON_COMMAND(IDC_SPLIT_IMPORT, OnSplitImport)

	// Configuration Category.
	ON_COMMAND(IDC_OPTIONS, OnCLOption)
	ON_COMMAND(IDC_MAC_MGR, OnMacLib)
	ON_COMMAND(IDC_EXPERTLIB_MGR, OnExpertLib)
	ON_COMMAND(IDC_MATLIB_MGR, OnMatLib)
	ON_COMMAND(IDC_DFL_REPORT_TPL, OnDflReportTpl)
	ON_COMMAND(IDC_DESIGN_REPORT_TPL, OnDesignReportTpl)

	// "Help" Category.
	ON_COMMAND(IDC_HELP_HELPVIDEO, OnHelpVideo)
	ON_COMMAND(IDC_HELP_TOPIC, OnHelpTopics)
	ON_COMMAND(IDC_HELP_WEBSITE, OnWebSite)
	ON_COMMAND(IDC_HELP_ABOUT, OnAbout)

	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)

	// 处理自定义消息。
	ON_THREAD_MESSAGE(WM_DATA_ITEM_DB_CLICK, OnOpenDataItem)
	ON_THREAD_MESSAGE(WM_OPEN_PART_ITEM, OnOpenPartItem)
	ON_THREAD_MESSAGE(WM_DATA_AUTO_POPUP_VIDEO, OnAutoPopupVideo)
	ON_THREAD_MESSAGE(WM_GUIDE_MODE_1, OnGuideMode1)
	ON_THREAD_MESSAGE(WM_GUIDE_MODE_2, OnGuideMode2)

	ON_COMMAND_EX_RANGE(IDC_RECENT_DATA_BASE, IDC_RECENT_DATA_BASE+RECENT_DATA_COUNT, OnOpenRecentData)
	ON_COMMAND(IDC_RECENT_DATA, OnRecentData)

END_MESSAGE_MAP()

ClApp::ClApp()
{
	m_bNotLaunch = FALSE;
}

ClApp theApp;

BOOL ClApp::InitInstance()
{
	// init the COM apartment
	CoInitialize(NULL);

	// init the unit conversion resolution value.
	CClientDC dc(CWnd::FromHandle(GetDesktopWindow()));
	UnitHelper::Init(dc.GetDeviceCaps(LOGPIXELSX));

	// init the random value manager.
	RandomGen::Initialize((unsigned long long)time(NULL));

	// init the preference information.
	CString strPath = ProductFolderManager::GetOptionPath();
	strPath += USER_OPTION_FILE;
	CLOptionInfoPtr pCLOptionInfo = CLOptionLoader::LoadCLOption(strPath);
	CLOptionInfo::Init(pCLOptionInfo);

	// 初始化Polygon2DProcessor中的变量。
	SheetEditParamPtr pSheetEditParam = pCLOptionInfo->GetSheetEditParam();
	Polygon2DProcessor::Init(pSheetEditParam->EnableSleekCtrl(), pSheetEditParam->GetSleekRate());

	// 加载资源。
	ProductParamPtr pProductParam = pCLOptionInfo->GetProductParam();
	HINSTANCE hInstRes = ::LoadLibrary(_T(CL_RES_FILE));
	if (hInstRes)
    {
		AfxSetResourceHandle(hInstRes);
    }
	MultiLanguageMgr::GetInstance()->LoadTranslationData(ProductFileManager::GetTranslationLibPath(), pProductParam->GetLang()); // 加载当前语言对应的翻译数据，这一行调用需要在加载资源之后。


	// 检查许可证, you MUST check license here, otherwise you cannot run cutLeader correctly.
    {
        CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
        int iProductID = -1, iCustomerID = -1;
        CTime startTime, endTime;
        LicenseCheckResultType iLicenseCheckResult = LicenseManager::CheckLicense_2(iProductID, iCustomerID, startTime, endTime);
        if (iLicenseCheckResult == LicenseCheckResult_integratyFail)
        {
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CoreModules_ShouldBeIntegraty);
            MessageBox(NULL, str, strProductName, MB_OK | MB_ICONWARNING);
            m_bNotLaunch = TRUE;
            return FALSE;
        }
        else if (iLicenseCheckResult == LicenseCheckResult_noLicense)
        {
            // if no dongle attached, it's the education edition.
            //CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_ATTACH_DONGLE);
            //MessageBox(NULL, str, strProductName, MB_OK | MB_ICONWARNING);
            //m_bNotLaunch = TRUE;
            //return FALSE;
        }
        else if (iLicenseCheckResult == LicenseCheckResult_dateExpired)
        {
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_EXPIRE_DATE);
            CString strCustomerID;
            strCustomerID.Format(_T("%d"), iCustomerID);
            str.Replace(_T("WILLBEREPLACED1"), strCustomerID);
            MessageBox(NULL, str, strProductName, MB_OK | MB_ICONWARNING);
            m_bNotLaunch = TRUE;
            return FALSE;
        }
    }


	// 只能有一个实例启动。
	TCHAR strCurExePath[MAXLENGTH_STR];
	GetModuleFileName(NULL, strCurExePath, MAXLENGTH_STR);
	IntegerListPtr pProcessIDList = ProcessMgr::IsProcessRun(strCurExePath);
	if (pProcessIDList->size() == 1)
	{
		;
	}
	else if (pProcessIDList->size() == 2)
	{
		// remove the current process ID.
		int iCurProcessID = GetCurrentProcessId();
		pProcessIDList->RemoveInt(iCurProcessID);

		// get the main frame of the running app.
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		HWND hMainWnd = ProcessMgr::GetProcessMainWnd((int)pProcessIDList->at(0),str);

		// send msg to the main frame of the running app.
		WndMsg wndMsg;
		wndMsg.iWndMsgType = WNDMSG_ACTIVE_MAIN;
		ProcessMgr::SendCopyMsg(hMainWnd, wndMsg);

		m_bNotLaunch = TRUE;
		return FALSE;
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// 注册表键值。
	CString strRegKey;
	strRegKey = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_REG_KEY);
	SetRegistryKey(strRegKey);
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();

	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// display the splasher
	BeginSplash();

	/************************************************************************/
	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.

	// init the part doc template.
	m_pPartDocTemplate = new CMultiDocTemplate(IDR_PART_DOC_TYPE, RUNTIME_CLASS(PartDocument), RUNTIME_CLASS(PartFrame), RUNTIME_CLASS(PartView));
	if (!m_pPartDocTemplate)
		return FALSE;
	AddDocTemplate(m_pPartDocTemplate);

	// init the sheet doc template.
	m_pSheetDocTemplate = new CMultiDocTemplate(IDR_SHEET_DOC_TYPE, RUNTIME_CLASS(SheetDocument), RUNTIME_CLASS(SheetFrame), RUNTIME_CLASS(SheetView));
	if (!m_pSheetDocTemplate)
		return FALSE;
	AddDocTemplate(m_pSheetDocTemplate);

	// 初始化“起始页”文档模板。
	m_pStartPageDocTpl = new CMultiDocTemplate(IDR_STARTPAGE_TYPE, RUNTIME_CLASS(StartPageDoc), RUNTIME_CLASS(StartPageFrame), RUNTIME_CLASS(StartPageView));
	if (!m_pStartPageDocTpl)
		return FALSE;
	AddDocTemplate(m_pStartPageDocTpl);
	/************************************************************************/

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		;
	else
	{
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	::SetForegroundWindow(pMainFrame->m_hWnd);
	pMainFrame->UpdateWindow();

	//
	Init();

	// erase the splasher
	EndSplash();

	// 检查是否显示“起始页面”。
	if (pProductParam->IsShowStartPage())
		m_pStartPageDocTpl->OpenDocumentFile(NULL);

	// check whether display the help video.
	if (pProductParam->IsShowHelpVideo())
	{
		HelpVideoDlg* pDlg = new HelpVideoDlg(pProductParam->GetLang(), pProductParam->IsShowHelpVideo());
		pDlg->Create(IDD_clBaseDataUI_HelpVideo, pMainFrame);
		pDlg->ShowWindow(SW_SHOW);
	}

	return TRUE;
}

int ClApp::ExitInstance()
{
	// clear something.
	if (!m_bNotLaunch)
		UnInit();

	// 清理随机数生成器
	RandomGen::UnInitialize();

	return CWinAppEx::ExitInstance();
}

void ClApp::Init()
{
	// init the database connection which will be used by app.
	DataBaseManager::GetInstance()->InitDB();

	// init data which opened recently.
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	RecentDataListPtr pRecentDataList = RecentDataLoader::LoadRecentData(pMainDBCon);
	DataItemTracker::GetInstance()->SetRecentData(pRecentDataList);

	// hide all "prop" bar.
	UIManager::GetInstance()->GetBarManager()->HideAllBar();

    // to be commented, 检查数据库，看看要不要修复，正常运行时不需下面的代码。
    {
        //ConfigItemWriter::CheckData_of_toolAssignInfo(DataBaseManager::GetInstance()->GetExpLibDBConnect());
    }

    // 专家库版本升到6.3时，expLib_geomLeadInOut中新增了字段，调用这个函数可以把老版本的专家库升级。
    ConfigItemWriter::CheckTable_4_v63();
}

void ClApp::UnInit()
{
	// release UI resource.
	delete UIManager::GetInstance();

	/************************************************************************/
	// check whether we should backup data.

	CString strProductName, str;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// some params.
	ProductParamPtr pProductParam = CLOptionInfo::GetInstance()->GetProductParam();
	int iBkInterval = pProductParam->GetDBBackupDay();
	int iLogBkSize = pProductParam->GetLogBackupSize();

	// get the db backup date, if not backup yet, update the date.
	LONGLONG iDBBackupDate = LogMgr::GetDBBackupDate();
	if (iDBBackupDate == INVALID_VALUE)
	{
		iDBBackupDate = DateHelper::GetCurrentTime();
		LogMgr::UpdateDBBackupDate(iDBBackupDate);
	}

	// the interval data from the previous backup.
	LONGLONG iDayIterval = DateHelper::GetIntervalDay(iDBBackupDate, DateHelper::GetCurrentTime());

	// the log file size.
	int iLogFileSize = LogMgr::GetLogFileSize();

	// check whether need to backup.
	CString strBKAppPath = ProductFolderManager::GetProductPath();
	strBKAppPath += _T(DATA_BK_EXE_FILE);
	if (iDayIterval >= iBkInterval && iLogFileSize >= iLogBkSize)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_BACKUP_DB_AND_LOG);
		if (MessageBox(NULL, str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
			ProcessMgr::RunProcess(strBKAppPath);
	}
	else if (iDayIterval >= iBkInterval)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_BACKUP_DB);
		if (MessageBox(NULL, str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
			ProcessMgr::RunProcess(strBKAppPath);
	}
	else if (iLogFileSize >= iLogBkSize)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_BACKUP_LOG);
		if (MessageBox(NULL, str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
			ProcessMgr::RunProcess(strBKAppPath);
	}
	/************************************************************************/

	// update the RecentData list to database and release the data tracker.
	RecentDataListPtr pRecentDataList = DataItemTracker::GetInstance()->GetRecentData();
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	RecentDataWriter::SaveRecentData(pMainDBCon, pRecentDataList);
	delete DataItemTracker::GetInstance();

	// release database.
	DataBaseManager* pDataBaseMgr = DataBaseManager::GetInstance();
	pDataBaseMgr->CloseDB();
	delete pDataBaseMgr;

    // 清理掉翻译库
    MultiLanguageMgr* pMultiLanguageMgr = MultiLanguageMgr::GetInstance();
    delete pMultiLanguageMgr;

    // 清理和释放命令
    CommandManager* pCommandManager = CommandManager::GetInstance();
    delete pCommandManager;

    // 释放键盘输入管理器
    ValueInputManager* pValueInputManager = ValueInputManager::GetInstance();
    delete pValueInputManager;

    // 释放“切割节点模拟器”。
    CutNodeSimulator* pCutNodeSimulator = CutNodeSimulator::GetInstance();
    if (!pCutNodeSimulator->IsFinished())
    {
        pCutNodeSimulator->StopSimulation();
    }
    delete pCutNodeSimulator;
}

void ClApp::BeginSplash()
{
	// Bring up the splash screen in a secondary UI thread
	m_pSplashThread = (SplashThread*) AfxBeginThread(RUNTIME_CLASS(SplashThread), 
													 THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);

	ASSERT(m_pSplashThread->IsKindOf(RUNTIME_CLASS(SplashThread)));

	m_pSplashThread->SetBitmapToUse(IDB_SPLASHER);

	// Resume the thread now that we have set it up 
	m_pSplashThread->ResumeThread();

	// Simulate some slow loading process.
	Sleep(1000);
}

void ClApp::EndSplash()
{
	m_pSplashThread->HideSplash();
}
