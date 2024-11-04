#include "StdAfx.h"
#include "InsertPositionDlg.h"

#include "baseConst.h"
#include "PartPlacementEditAction.h"
#include "clPartLayoutResource.h"


BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(InsertPositionDlg, CDialogEx)

InsertPositionDlg::InsertPositionDlg(CWnd* pParent)
	: CLDialog(InsertPositionDlg::IDD, pParent)
{
}

InsertPositionDlg::~InsertPositionDlg(void)
{
}

void InsertPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_BEHIND, m_bInsertBehind);
    DDX_Control(pDX, IDC_BTN_QuitCurrentAction, m_ctrlQuitCurrentActionButton);
}

BOOL InsertPositionDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_bInsertBehind = TRUE;
	UpdateData(FALSE);

    // 按钮提示。
    CString strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_QuitCurrentAction);
    m_ctrlQuitCurrentActionButton.SetTooltip(strTips);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void InsertPositionDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(InsertPositionDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_CHECK_BEHIND, OnCheckBehind)
    ON_BN_CLICKED(IDC_BTN_QuitCurrentAction, OnQuitCurrentActionButton)
END_MESSAGE_MAP()

BOOL InsertPositionDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH InsertPositionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void InsertPositionDlg::OnCheckBehind()
{
	UpdateData(TRUE);
}

void InsertPositionDlg::OnQuitCurrentActionButton()
{
    PartPlacementEditAction::SendMessage2EnterPartPlacementOrLoopInstanceSelectAction();

    // 隐藏对话框。
    ShowWindow(SW_HIDE);
}

END_CUTLEADER_NAMESPACE()
