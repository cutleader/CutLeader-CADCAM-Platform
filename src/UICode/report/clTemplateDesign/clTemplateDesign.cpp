#include "stdafx.h"
#include "afxwinappex.h"
#include "clTemplateDesign.h"

#include "RandomGen.h"
#include "UnitHelper.h"
#include "ProductFolderManager.h"
#include "ProductFileDefine.h"
#include "DataBaseManager.h"
#include "MultiLanguageMgr.h"
#include "ProductFileManager.h"

#include "clReportResource.h"
#include "TplDesignConfig.h"
#include "TplDesignConfigLoader.h"
#include "ProductParam.h"
#include "TplDesignConfigDlg.h"
#include "ReportTplDlg.h"

#include "ChildFrm.h"
#include "tplDesignDoc.h"
#include "tplDesignView.h"
#include "MainFrm.h"

#include "CLOptionInfo.h"
#include "CLOptionLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ClTemplateDesignApp, CWinAppEx)
	ON_COMMAND(IDC_NEW_TPL_PAGE, OnNewReportTpl)
	ON_COMMAND(IDC_MGR_TPL_PAGE, OnOpenReportTpl)
	ON_COMMAND(IDC_TPL_SAVE, OnSaveReportTpl)

	ON_COMMAND(IDC_TPL_PREFERENCE, OnPreference)
	ON_COMMAND(IDC_MGR_REPORT_TPL, OnMgrReportTpl)
END_MESSAGE_MAP()

ClTemplateDesignApp::ClTemplateDesignApp()
{
}

ClTemplateDesignApp theApp;

BOOL ClTemplateDesignApp::InitInstance()
{
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
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(0);  // Load standard INI file options

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// do some initializations when the application starts.
	Init();

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	m_pDocTemplate = new CMultiDocTemplate(IDR_tplDesignTYPE,
										   RUNTIME_CLASS(CtplDesignDoc),
										   RUNTIME_CLASS(CChildFrame), // custom MDI child frame
										   RUNTIME_CLASS(CtplDesignView));
	if (!m_pDocTemplate)
		return FALSE;
	AddDocTemplate(m_pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int ClTemplateDesignApp::ExitInstance()
{
	RandomGen::UnInitialize();

	return CWinAppEx::ExitInstance();
}

void ClTemplateDesignApp::OnNewReportTpl()
{
	// open the empty report template.
	m_pDocTemplate->OpenDocumentFile(NULL);
}

void ClTemplateDesignApp::OnOpenReportTpl()
{
	// open the empty report template.
	m_pDocTemplate->OpenDocumentFile(_T(""));
}

void ClTemplateDesignApp::OnSaveReportTpl()
{
	CtplDesignDoc* pDoc = GetActiveDocument();
	if (pDoc)
		pDoc->OnSaveDocument();
}

void ClTemplateDesignApp::OnPreference()
{
	TplDesignConfigDlg dlg;
	dlg.DoModal();
}

void ClTemplateDesignApp::OnMgrReportTpl()
{
	ReportTplDlg dlg;
	dlg.DoModal();
}

void ClTemplateDesignApp::Init()
{
	// init the unit conversion resolution value.
	CClientDC dc(CWnd::FromHandle(GetDesktopWindow()));
	UnitHelper::Init(dc.GetDeviceCaps(LOGPIXELSX));

	// init the random value manager.
	RandomGen::Initialize((unsigned long long)time(NULL));

	// init the preference information
	TplDesignConfigPtr pTplDesignConfig = TplDesignConfigLoader::LoadTplDesignConfig();
	TplDesignConfig::Init(pTplDesignConfig);

	/************************************************************************/
	// ������Դ��

	// init the preference information.
	CString strPath = ProductFolderManager::GetOptionPath();
	strPath += USER_OPTION_FILE;
	CLOptionInfoPtr pCLOptionInfo = CLOptionLoader::LoadCLOption(strPath);
	CLOptionInfo::Init(pCLOptionInfo);

	// ������Դdll
	ProductParamPtr pProductParam = pCLOptionInfo->GetProductParam();
    HINSTANCE hInstRes = ::LoadLibrary(_T(CL_RES_FILE));
	if (hInstRes)
    {
		AfxSetResourceHandle(hInstRes);
    }

    // ���ص�ǰ���Զ�Ӧ�ķ������ݣ���һ�е�����Ҫ�ڼ�����Դ֮��
    MultiLanguageMgr::GetInstance()->LoadTranslationData(ProductFileManager::GetTranslationLibPath(), pProductParam->GetLang());
	/************************************************************************/

	// init the db connection.
	DataBaseManager::GetInstance()->InitDB();
}

void ClTemplateDesignApp::UnInit()
{
	// release database.
	DataBaseManager* pDataBaseMgr = DataBaseManager::GetInstance();
	pDataBaseMgr->CloseDB();
	delete pDataBaseMgr;

    // ����������
    MultiLanguageMgr* pMultiLanguageMgr = MultiLanguageMgr::GetInstance();
    delete pMultiLanguageMgr;
}

CtplDesignDoc* ClTemplateDesignApp::GetActiveDocument()
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();

	// get the active MDI child window.
	CMDIChildWnd *pChild = (CMDIChildWnd*)pFrame->GetActiveFrame();

	// get the active view attached to the active MDI child window.
	CtplDesignView *pView = (CtplDesignView*)pChild->GetActiveView();
	if (pView == NULL)
		return NULL;

	CtplDesignDoc* pDoc = pView->GetDocument();
	return pDoc;
}
