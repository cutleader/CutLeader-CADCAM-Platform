#include "StdAfx.h"
#include "GeomOptParamDlg.h"

#include "baseConst.h"
#include "clUtilityResource.h"

#include "ImpExpConfig.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(GeomOptParamDlg, CDialogEx)

GeomOptParamDlg::GeomOptParamDlg()
: CLDialog(GeomOptParamDlg::IDD)
{
}

GeomOptParamDlg::GeomOptParamDlg(ImpExpConfigPtr pImpExpConfig)
				: CLDialog(GeomOptParamDlg::IDD)
{
	m_pImpExpConfig = pImpExpConfig;
}

GeomOptParamDlg::~GeomOptParamDlg(void)
{
}

void GeomOptParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_ALLOW_OFFSET, m_ctrlAllowOffset);
	DDX_Control(pDX, IDC_EDIT_MERGE_GEOM_SIZE, m_dPatFilterSize);
	DDX_Radio(pDX, IDC_RADIO_MOREARC, m_nMoreArc);
}

BOOL GeomOptParamDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_ctrlAllowOffset.Init(m_pImpExpConfig->GetAllowOffset(), 4, FALSE);
	m_dPatFilterSize.Init(m_pImpExpConfig->GetPatFilterSize(), 4, FALSE);
	if (m_pImpExpConfig->IsMoreArc())
	{
		m_nMoreArc = 0;
	}
	else
	{
		m_nMoreArc = 1;
	}

	UpdateData(FALSE);
	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BEGIN_MESSAGE_MAP(GeomOptParamDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL GeomOptParamDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH GeomOptParamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	// here, message edit box will display the bk color same as the dlg.
	if (_tcsicmp(classname, _T("EDIT")) == 0 && pWnd->GetDlgCtrlID() != IDC_EDIT_MSG)
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

void GeomOptParamDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_pImpExpConfig)
	{
		if (m_pImpExpConfig->GetAllowOffset() != m_ctrlAllowOffset.Get_Number())
		{
			m_pImpExpConfig->SetAllowOffset(m_ctrlAllowOffset.Get_Number());
			m_pImpExpConfig->SetModified(TRUE);
		}

		if (m_pImpExpConfig->GetPatFilterSize() != m_dPatFilterSize.Get_Number())
		{
			m_pImpExpConfig->SetPatFilterSize(m_dPatFilterSize.Get_Number());
			m_pImpExpConfig->SetModified(TRUE);
		}

		bool bMoreArc = true;
		if (m_nMoreArc == 1)
		{
			bMoreArc = false;
		}
		if (bMoreArc != m_pImpExpConfig->IsMoreArc())
		{
			m_pImpExpConfig->IsMoreArc(bMoreArc);
			m_pImpExpConfig->SetModified(TRUE);
		}
	}

	OnOK();
}

END_CUTLEADER_NAMESPACE()
