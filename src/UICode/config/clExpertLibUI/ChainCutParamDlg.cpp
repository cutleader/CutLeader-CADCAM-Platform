#include "StdAfx.h"
#include "ChainCutParamDlg.h"

#include "baseConst.h"
#include "ChainCutParam.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(ChainCutParamDlg, CDialogEx)

ChainCutParamDlg::ChainCutParamDlg()
	: CLDialog(ChainCutParamDlg::IDD)
{
}

ChainCutParamDlg::ChainCutParamDlg(ChainCutParamPtr pChainCutParam)
				: CLDialog(ChainCutParamDlg::IDD)
{
	m_pChainCutParam = pChainCutParam;
}

ChainCutParamDlg::~ChainCutParamDlg(void)
{
}

void ChainCutParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_AVOID_ALONG_CONTOUR, m_bCutAlongContour);
	DDX_Control(pDX, IDC_EDIT_AVOID_DISTANCE, m_dAvoidDis);
}

BEGIN_MESSAGE_MAP(ChainCutParamDlg, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL ChainCutParamDlg::OnInitDialog()
{
	__super::OnInitDialog();

	DisplayChainCutParam();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void ChainCutParamDlg::DisplayChainCutParam()
{
	m_bCutAlongContour = m_pChainCutParam->IsCutAlongContour();
	m_dAvoidDis.Init(m_pChainCutParam->GetAvoidDis(), 4, FALSE);
	UpdateData(FALSE);
}

BOOL ChainCutParamDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect r;
    GetClientRect(&r);
    if ((HBRUSH)m_brush == NULL)
        m_brush.CreateSolidBrush(RGB(239, 247, 253));
    pDC->FillRect(&r, &m_brush);

    return TRUE;
}

HBRUSH ChainCutParamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void ChainCutParamDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_pChainCutParam)
	{
		if (m_bCutAlongContour != m_pChainCutParam->IsCutAlongContour())
		{
			m_pChainCutParam->IsCutAlongContour(m_bCutAlongContour);
			m_pChainCutParam->SetModified(TRUE);
		}

		if (m_pChainCutParam->GetAvoidDis() != m_dAvoidDis.Get_Number())
		{
			m_pChainCutParam->SetAvoidDis(m_dAvoidDis.Get_Number());
			m_pChainCutParam->SetModified(TRUE);
		}
	}

	OnOK();
}

END_CUTLEADER_NAMESPACE()
