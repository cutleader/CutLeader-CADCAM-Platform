#include "StdAfx.h"
#include "ApplyPartDlg.h"

#include "baseConst.h"
#include "clPartLayoutResource.h"
#include "PartPlacementEditAction.h"


BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(ApplyPartDlg, CDialogEx)

ApplyPartDlg::ApplyPartDlg(CWnd* pParent)
	: CLDialog(ApplyPartDlg::IDD, pParent)
{
}

ApplyPartDlg::~ApplyPartDlg(void)
{
}

void ApplyPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_PART, m_bApplyPart);
    DDX_Control(pDX, IDC_BTN_QuitCurrentAction, m_ctrlQuitCurrentActionButton);
}

BOOL ApplyPartDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_bApplyPart = TRUE;
	UpdateData(FALSE);

    // 按钮提示。
    CString strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_QuitCurrentAction);
    m_ctrlQuitCurrentActionButton.SetTooltip(strTips);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void ApplyPartDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(ApplyPartDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_CHECK_PART, OnCheckPart)
    ON_BN_CLICKED(IDC_BTN_QuitCurrentAction, OnQuitCurrentActionButton)
END_MESSAGE_MAP()

BOOL ApplyPartDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH ApplyPartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void ApplyPartDlg::OnCheckPart()
{
	UpdateData(TRUE);
}

void ApplyPartDlg::OnQuitCurrentActionButton()
{
    PartPlacementEditAction::SendMessage2EnterPartPlacementOrLoopInstanceSelectAction();

    // 隐藏对话框。
    ShowWindow(SW_HIDE);
}

END_CUTLEADER_NAMESPACE()
