#include "StdAfx.h"
#include "SpreadSamePartDlg.h"

#include "baseConst.h"
#include "LogMgr.h"
#include "clPartLayoutResource.h"
#include "PartPlacementEditAction.h"


BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SpreadSamePartDlg, CDialogEx)

SpreadSamePartDlg::SpreadSamePartDlg(CWnd* pParent)
	: CLDialog(SpreadSamePartDlg::IDD, pParent)
{
}

SpreadSamePartDlg::~SpreadSamePartDlg(void)
{
}

void SpreadSamePartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_SPREAD, m_bSpread);
    DDX_Control(pDX, IDC_BTN_QuitCurrentAction, m_ctrlQuitCurrentActionButton);
}

BOOL SpreadSamePartDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_bSpread = FALSE;
	UpdateData(FALSE);

    // 按钮提示。
    CString strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_QuitCurrentAction);
    m_ctrlQuitCurrentActionButton.SetTooltip(strTips);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void SpreadSamePartDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
}

BEGIN_MESSAGE_MAP(SpreadSamePartDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_CHECK_SPREAD, OnCheckSpread)
    ON_BN_CLICKED(IDC_BTN_QuitCurrentAction, OnQuitCurrentActionButton)
END_MESSAGE_MAP()

BOOL SpreadSamePartDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH SpreadSamePartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void SpreadSamePartDlg::OnCheckSpread()
{
	UpdateData(TRUE);
    NotifyAllObserver(INVALID_ID, VARIANT()); // 通知所有监听者
}

void SpreadSamePartDlg::OnQuitCurrentActionButton()
{
    PartPlacementEditAction::SendMessage2EnterPartPlacementOrLoopInstanceSelectAction();

    // 隐藏对话框。
    ShowWindow(SW_HIDE);
}

END_CUTLEADER_NAMESPACE()
