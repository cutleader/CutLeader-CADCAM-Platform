#include "StdAfx.h"
#include "GridCutParamDlg.h"

#include "baseConst.h"
#include "GridCutParam.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(GridCutParamDlg, CDialogEx)

GridCutParamDlg::GridCutParamDlg()
	: CLDialog(GridCutParamDlg::IDD)
{
}

GridCutParamDlg::GridCutParamDlg(GridCutParamPtr pGridCutParam)
				: CLDialog(GridCutParamDlg::IDD)
{
	m_pGridCutParam = pGridCutParam;
}

GridCutParamDlg::~GridCutParamDlg(void)
{
}

void GridCutParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_VERTICAL_4_INNER, m_bVertical_4_inner);
	DDX_Check(pDX, IDC_CHECK_CCW_4_OUTER, m_bCCW_4_outer);
}

BEGIN_MESSAGE_MAP(GridCutParamDlg, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL GridCutParamDlg::OnInitDialog()
{
	__super::OnInitDialog();

	DisplayGridCutParam();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void GridCutParamDlg::DisplayGridCutParam()
{
	m_bVertical_4_inner = m_pGridCutParam->IsVertical_4_inner();
	m_bCCW_4_outer = m_pGridCutParam->IsCCW_4_outer();
	UpdateData(FALSE);
}

BOOL GridCutParamDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect r;
    GetClientRect(&r);
    if ((HBRUSH)m_brush == NULL)
        m_brush.CreateSolidBrush(RGB(239, 247, 253));
    pDC->FillRect(&r, &m_brush);

    return TRUE;
}

HBRUSH GridCutParamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void GridCutParamDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_pGridCutParam)
	{
		if (m_pGridCutParam->IsVertical_4_inner() != m_bVertical_4_inner)
		{
			m_pGridCutParam->IsVertical_4_inner(m_bVertical_4_inner);
			m_pGridCutParam->SetModified(TRUE);
		}

		if (m_pGridCutParam->IsCCW_4_outer() != m_bCCW_4_outer)
		{
			m_pGridCutParam->IsCCW_4_outer(m_bCCW_4_outer);
			m_pGridCutParam->SetModified(TRUE);
		}
	}

	OnOK();
}

END_CUTLEADER_NAMESPACE()
