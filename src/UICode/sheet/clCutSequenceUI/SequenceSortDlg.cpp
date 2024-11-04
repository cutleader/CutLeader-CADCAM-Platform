#include "StdAfx.h"
#include "SequenceSortDlg.h"

#include "baseConst.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SequenceSortDlg, CDialogEx)

SequenceSortDlg::SequenceSortDlg()
		: CLDialog(SequenceSortDlg::IDD)
{
	m_bSortFromStart = FALSE;
}

SequenceSortDlg::~SequenceSortDlg(void)
{
}

void SequenceSortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_START, m_bSortFromStart);
}

BEGIN_MESSAGE_MAP(SequenceSortDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL SequenceSortDlg::OnInitDialog()
{
	__super::OnInitDialog();
	
	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

HBRUSH SequenceSortDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_MAT)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void SequenceSortDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	OnOK();
}

END_CUTLEADER_NAMESPACE()
