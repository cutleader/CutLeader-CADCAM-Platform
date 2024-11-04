#include "StdAfx.h"
#include "SelectPartOrLoopActionBar.h"

#include "baseConst.h"

#include "clPartLayoutResource.h"
#include "PartPlacementEditAction.h"


BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SelectPartOrLoopActionBar, CDialogEx)

SelectPartOrLoopActionBar::SelectPartOrLoopActionBar(CWnd* pParent /*=NULL*/)
	: CLDialog(SelectPartOrLoopActionBar::IDD, pParent)
{
    m_bSelectLoopInstance = FALSE;
    m_bAutoSelectSamePartsOrLoops = FALSE;
    m_bHideCutSequence = FALSE;
}

SelectPartOrLoopActionBar::~SelectPartOrLoopActionBar(void)
{
}

void SelectPartOrLoopActionBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

    DDX_Check(pDX, IDC_CHECK_SelectLoopInstance, m_bSelectLoopInstance);
    DDX_Check(pDX, IDC_CHECK_AutoSelectSamePartsOrLoops, m_bAutoSelectSamePartsOrLoops);
	DDX_Check(pDX, IDC_CHECK_HideCutSequence, m_bHideCutSequence);
}

BOOL SelectPartOrLoopActionBar::OnInitDialog()
{
	__super::OnInitDialog();

    UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BEGIN_MESSAGE_MAP(SelectPartOrLoopActionBar, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

    ON_BN_CLICKED(IDC_CHECK_SelectLoopInstance, OnSelectLoopInstance)
    ON_BN_CLICKED(IDC_CHECK_AutoSelectSamePartsOrLoops, OnAutoSelectSamePartsOrLoops)
	ON_BN_CLICKED(IDC_CHECK_HideCutSequence, OnHideCutSequence)
END_MESSAGE_MAP()

BOOL SelectPartOrLoopActionBar::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH SelectPartOrLoopActionBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void SelectPartOrLoopActionBar::OnSelectLoopInstance()
{
    UpdateData(TRUE);
    if (m_bSelectLoopInstance)
    {
        PartPlacementEditAction::SendMessage2EnterLoopInstanceSelectAction();
    }
    else
    {
        PartPlacementEditAction::SendMessage2EnterPartPlacementSelectAction();
    }
}

void SelectPartOrLoopActionBar::OnAutoSelectSamePartsOrLoops()
{
    UpdateData(TRUE);
}

void SelectPartOrLoopActionBar::OnHideCutSequence()
{
	UpdateData(TRUE);

    // 得到当前视图
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
    CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
    CView* pView = pChildFrame->GetActiveView();

	if (m_bHideCutSequence)
    {
        pView->PostMessage(WM_HideOrShowCutPath, 0, 0);
    }
	else
    {
        pView->PostMessage(WM_HideOrShowCutPath, 1, 0);
    }
}

END_CUTLEADER_NAMESPACE()
