#include "StdAfx.h"
#include "SheetGuideDlg.h"

#include "baseConst.h"
#include "clBaseDataResource.h"
#include "clPartLayoutResource.h"
#include "clCutSequenceResource.h"
#include "clPostProcessorResource.h"
#include "clFrameResource.h"
#include "UIManager.h"

BEGIN_CUTLEADER_NAMESPACE()

SheetGuideDlg::SheetGuideDlg(CWnd* pParent) : CLDialog(SheetGuideDlg::IDD, pParent)
{
	m_iSheetGuideState = SHEET_GUIDE_CREATE_SHEET;
}

SheetGuideDlg::~SheetGuideDlg(void)
{
}

void SheetGuideDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTN_PRE, m_ctrlPreBtn);
	DDX_Control(pDX, IDC_BTN_NEXT, m_ctrlPostBtn);
	DDX_Control(pDX, IDC_BTN_QUIT, m_ctrlQuitBtn);
}

BEGIN_MESSAGE_MAP(SheetGuideDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_PRE, OnPrevious)
	ON_BN_CLICKED(IDC_BTN_NEXT, OnNext)
	ON_BN_CLICKED(IDC_BTN_QUIT, OnQuitGuide)
END_MESSAGE_MAP()

BOOL SheetGuideDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_ctrlPreBtn.SetImage(IDB_PREVIOUS, IDB_PREVIOUS);
	m_ctrlPostBtn.SetImage(IDB_NEXT, IDB_NEXT);
	m_ctrlQuitBtn.SetImage(IDB_QUIT, IDB_QUIT);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void SheetGuideDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

BOOL SheetGuideDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

void SheetGuideDlg::OnPrevious()
{
	CString strProductName, strMsg, strPartPmtCategory, strSeqCategory;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	strPartPmtCategory = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT);
	strSeqCategory = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ);

	CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
	CView* pCurrentView = (CView*)pChildFrame->GetActiveView();

	if (m_iSheetGuideState == SHEET_GUIDE_ADD_DXFDWG)
	{
		m_iSheetGuideState = SHEET_GUIDE_CREATE_SHEET;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_0);
		::MessageBox(GetParent()->m_hWnd, strMsg, strProductName, MB_OK | MB_ICONINFORMATION);
	}
	else if (m_iSheetGuideState == SHEET_GUIDE_AUTO_SEQUENCE)
	{
		m_iSheetGuideState = SHEET_GUIDE_ADD_DXFDWG;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_1);
		UIManager::GetInstance()->GetRibbonBar()->HighlightBtn(strPartPmtCategory, IDC_PARTPMT_FROM_SGL_DXF, strMsg);
	}
	else if (m_iSheetGuideState == SHEET_GUIDE_VIEW_SEQUENCE)
	{
		m_iSheetGuideState = SHEET_GUIDE_AUTO_SEQUENCE;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_2);
		UIManager::GetInstance()->GetRibbonBar()->HighlightBtn(strSeqCategory, IDC_SEQ_AUTOSEQUENCE, strMsg);
	}
	else if (m_iSheetGuideState == SHEET_GUIDE_GENERATE_NC)
	{
		m_iSheetGuideState = SHEET_GUIDE_VIEW_SEQUENCE;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_3);
		UIManager::GetInstance()->GetRibbonBar()->HighlightBtn(strSeqCategory, IDC_SEQ_VIEWSEQUENCE, strMsg);
	}
}

void SheetGuideDlg::OnNext()
{
	CString strProductName, strMsg, strPartPmtCategory, strSeqCategory;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	strPartPmtCategory = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT);
	strSeqCategory = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ);

	CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
	CView* pCurrentView = (CView*)pChildFrame->GetActiveView();

	if (m_iSheetGuideState == SHEET_GUIDE_CREATE_SHEET)
	{
		m_iSheetGuideState = SHEET_GUIDE_ADD_DXFDWG;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_1);
		UIManager::GetInstance()->GetRibbonBar()->HighlightBtn(strPartPmtCategory, IDC_PARTPMT_FROM_SGL_DXF, strMsg);
	}
	else if (m_iSheetGuideState == SHEET_GUIDE_ADD_DXFDWG)
	{
		m_iSheetGuideState = SHEET_GUIDE_AUTO_SEQUENCE;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_2);
		UIManager::GetInstance()->GetRibbonBar()->HighlightBtn(strSeqCategory, IDC_SEQ_AUTOSEQUENCE, strMsg);
	}
	else if (m_iSheetGuideState == SHEET_GUIDE_AUTO_SEQUENCE)
	{
		m_iSheetGuideState = SHEET_GUIDE_VIEW_SEQUENCE;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_3);
		UIManager::GetInstance()->GetRibbonBar()->HighlightBtn(strSeqCategory, IDC_SEQ_VIEWSEQUENCE,  strMsg);
	}
	else if (m_iSheetGuideState == SHEET_GUIDE_VIEW_SEQUENCE)
	{
		m_iSheetGuideState = SHEET_GUIDE_GENERATE_NC;
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_4);
		UIManager::GetInstance()->GetRibbonBar()->HighlightBtn(strSeqCategory, IDC_MACROUTER_VIEW_NC, strMsg);
	}
	else if (m_iSheetGuideState == SHEET_GUIDE_GENERATE_NC)
	{
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_GUIDE_STEP_n);
		::MessageBox(GetParent()->m_hWnd, strMsg, strProductName, MB_OK | MB_ICONQUESTION);
	}
}

void SheetGuideDlg::OnQuitGuide()
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHT_QUIT_GUIDE);

	CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
	CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
	CView* pCurrentView = (CView*)pChildFrame->GetActiveView();
	if (::MessageBox(GetParent()->m_hWnd, strMsg, strProductName, MB_YESNO | MB_ICONINFORMATION) == IDYES)
	{
		::PostMessage(pCurrentView->m_hWnd, WM_SHT_GUIDE_QUIT, NULL, NULL);
		ShowWindow(SW_HIDE);
	}
}

END_CUTLEADER_NAMESPACE()
