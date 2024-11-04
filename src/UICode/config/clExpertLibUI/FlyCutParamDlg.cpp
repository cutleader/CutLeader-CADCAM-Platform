#include "StdAfx.h"
#include "FlyCutParamDlg.h"

#include "baseConst.h"
#include "FlyCutParam.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(FlyCutParamDlg, CDialogEx)

FlyCutParamDlg::FlyCutParamDlg()
	: CLDialog(FlyCutParamDlg::IDD)
{
}

FlyCutParamDlg::FlyCutParamDlg(FlyCutParamPtr pFlyCutParam)
				: CLDialog(FlyCutParamDlg::IDD)
{
	m_pFlyCutParam = pFlyCutParam;
}

FlyCutParamDlg::~FlyCutParamDlg(void)
{
}

void FlyCutParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_VERTICAL_CUT, m_bVerticalCut);
	DDX_Control(pDX, IDC_EDIT_MAX_TURN_RADIUS, m_dMax_turning_radius);
}

BEGIN_MESSAGE_MAP(FlyCutParamDlg, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL FlyCutParamDlg::OnInitDialog()
{
	__super::OnInitDialog();

	DisplayFlyCutParam();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void FlyCutParamDlg::DisplayFlyCutParam()
{
	m_bVerticalCut = m_pFlyCutParam->IsVerticalCut();
	m_dMax_turning_radius.Init(m_pFlyCutParam->GetMax_turing_radius(), 4, FALSE);
	UpdateData(FALSE);
}

BOOL FlyCutParamDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect r;
    GetClientRect(&r);
    if ((HBRUSH)m_brush == NULL)
        m_brush.CreateSolidBrush(RGB(239, 247, 253));
    pDC->FillRect(&r, &m_brush);

    return TRUE;
}

HBRUSH FlyCutParamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void FlyCutParamDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_pFlyCutParam)
	{
		if (m_bVerticalCut != m_pFlyCutParam->IsVerticalCut())
		{
			m_pFlyCutParam->IsVerticalCut(m_bVerticalCut);
			m_pFlyCutParam->SetModified(TRUE);
		}

		if (m_pFlyCutParam->GetMax_turing_radius() != m_dMax_turning_radius.Get_Number())
		{
			m_pFlyCutParam->SetMax_turing_radius(m_dMax_turning_radius.Get_Number());
			m_pFlyCutParam->SetModified(TRUE);
		}
	}

	OnOK();
}

END_CUTLEADER_NAMESPACE()
