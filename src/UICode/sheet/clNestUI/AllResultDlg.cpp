#include "StdAfx.h"
#include "AllResultDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(AllResultDlg, CDialogEx)

AllResultDlg::AllResultDlg()
		: CLDialog(AllResultDlg::IDD)
{
	m_iNestResultCount = 0;
	m_iSelectedResultIndex = -1;
}

AllResultDlg::~AllResultDlg(void)
{
}

void AllResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_RESULT_LIST, m_resultList);
}

BOOL AllResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (unsigned int i = 0; i < m_iNestResultCount; i++)
	{
		CString str;
		str.Format(_T("Nest Result %d"), i+1);
		m_resultList.InsertString(i, str);
	}
	if (m_iNestResultCount > 0)
	{
		m_resultList.SetCurSel(0);
	}

	UpdateData(FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(AllResultDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_OPEN, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL AllResultDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(RGB(239, 247, 253));
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH AllResultDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	pDC->SetBkColor(RGB(239, 247, 253));

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(RGB(239, 247, 253));

	return (HBRUSH) m_brush;
}

void AllResultDlg::OnBnClickedOk()
{
	m_iSelectedResultIndex = m_resultList.GetCurSel();
	if (m_iSelectedResultIndex < 0)
	{
		AfxMessageBox(_T("No nest result is selected."));
	}
	else
	{
		OnOK();
	}
}

END_CUTLEADER_NAMESPACE()