#include "StdAfx.h"
#include "PatternLoopOffsetDlg.h"

#include "baseConst.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PatternLoopOffsetDlg, CDialogEx)

PatternLoopOffsetDlg::PatternLoopOffsetDlg()
	: CLDialog(PatternLoopOffsetDlg::IDD)
{
	m_nSide = 0;
	m_dOffsetVal = 0.1;
}

PatternLoopOffsetDlg::~PatternLoopOffsetDlg(void)
{
}

void PatternLoopOffsetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_INSIDE, m_nSide);
	DDX_Control(pDX, IDC_EDIT_OFFSET, m_ctrlOffset);
}

BOOL PatternLoopOffsetDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// display the params.
	m_ctrlOffset.Init(m_dOffsetVal, 4, FALSE);

	SetBackgroundColor(UI_COLOR);
	UpdateData(FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(PatternLoopOffsetDlg, CDialogEx)
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_INSIDE, OnInside)
	ON_BN_CLICKED(IDC_RADIO_OUTSIDE, OnOutside)
	ON_EN_KILLFOCUS(IDC_EDIT_OFFSET, OnKillOffset)
END_MESSAGE_MAP()

HBRUSH PatternLoopOffsetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void PatternLoopOffsetDlg::OnInside()
{
	UpdateData(TRUE);
}

void PatternLoopOffsetDlg::OnOutside()
{
	UpdateData(TRUE);
}

void PatternLoopOffsetDlg::OnKillOffset()
{
	m_dOffsetVal = m_ctrlOffset.Get_Number();
}

void PatternLoopOffsetDlg::OnBnClickedOk()
{
	OnOK();
}

END_CUTLEADER_NAMESPACE()
