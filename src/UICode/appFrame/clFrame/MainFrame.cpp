#include "stdafx.h"
#include "MainFrame.h"

#include "CommandManager.h"
#include "ICommand.h"
#include "CommandList.h"
#include "ProcessMgr.h"

#include "PartDocument.h"
#include "BarManager.h"
#include "DocManager.h"
#include "UIManager.h"
#include "cLeaderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

const int iMaxUserToolbars = 10;

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_COPYDATA()

	ON_MESSAGE(WM_CHILDFRM_CLOSE, OnChildFrmClose)
	ON_REGISTERED_MESSAGE(AFX_WM_ON_BEFORE_SHOW_RIBBON_ITEM_MENU, OnShowRibbonItemMenu)
END_MESSAGE_MAP()

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	UINT nKeyCode = pMsg->wParam;

	// ¸´ÖÆºÍ³·Ïú¡£
	if ((nKeyCode == _T('C') || nKeyCode == _T('V') || nKeyCode == _T('Z') || nKeyCode == _T('S')) && 
		(::GetKeyState(VK_CONTROL) & 0x8000))
	{
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		return TRUE;
	}

	return CMDIFrameWndEx::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext) 
{
	if (!CMDIFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
		return FALSE;

	// enable customization button for all user toolbars
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// set the visual style to be used the by the visual manager
	CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);

	// set the Tab style.
	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE;
	mdiTabParams.m_bActiveTabCloseButton = TRUE;
	mdiTabParams.m_bTabIcons = TRUE;
	mdiTabParams.m_bAutoColor = FALSE;
	mdiTabParams.m_bDocumentMenu = TRUE;
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	// enable docking for global.
	EnableDocking(CBRS_ALIGN_ANY);

	// init the UI manager
	UIManager::GetInstance()->Init(GetDockingManager(), this);

	CDockingManager::SetDockingMode(DT_SMART);
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	return 0;
}

void CMainFrame::OnClose()
{
	// check whether some documents have been modified.
	CDocManager* pDocManager = AfxGetApp()->m_pDocManager;
	POSITION pos = pDocManager->GetFirstDocTemplatePosition();
	while (pos)
	{
		CMultiDocTemplate* pDocTemp = (CMultiDocTemplate*)pDocManager->GetNextDocTemplate(pos);
		if (pDocTemp)
		{
			POSITION docPos = pDocTemp->GetFirstDocPosition();
			while (docPos)
			{
				CDocument* pDoc = pDocTemp->GetNextDoc(docPos);
				CLeaderDoc* pTmpDoc = dynamic_cast<CLeaderDoc*>(pDoc);
				pTmpDoc->SetModifyStatus();
			}
		}
	} 

	CMDIFrameWndEx::OnClose();
}

BOOL CMainFrame::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct) 
{
	WndMsg* pWndMsg = (WndMsg*)(pCopyDataStruct->lpData);
	if (pWndMsg->iWndMsgType == WNDMSG_ACTIVE_MAIN)
	{
		ShowWindow(SW_SHOWMAXIMIZED);
		::SetForegroundWindow(m_hWnd);
	}

	return CMDIFrameWndEx::OnCopyData(pWnd, pCopyDataStruct);
}

LRESULT CMainFrame::OnChildFrmClose(WPARAM wp, LPARAM lp)
{
	if (!MDIGetActive())
	{
		UIManager::GetInstance()->GetBarManager()->HideAllBar();
		UIManager::GetInstance()->GetStatusBar()->Hide();
	}

	return 0;
}

LRESULT CMainFrame::OnShowRibbonItemMenu(WPARAM, LPARAM lp)
{
	CMDIChildWnd* pChild = (CMDIChildWnd*)GetActiveFrame();
	if (pChild)
	{
		CView* pView = pChild->GetActiveView();
		CLeaderView* pTmpView = dynamic_cast<CLeaderView*>(pView);
		if (pTmpView)
			pTmpView->UpdateUndoRedoItem((CMFCRibbonBaseElement*)lp);
	}

	return 0;
}

END_CUTLEADER_NAMESPACE()
