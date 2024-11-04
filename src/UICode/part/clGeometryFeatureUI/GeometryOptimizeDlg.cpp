#include "StdAfx.h"
#include "GeometryOptimizeDlg.h"

#include "baseConst.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(GeometryOptimizeDlg, CDialogEx)

GeometryOptimizeDlg::GeometryOptimizeDlg()
	: CLDialog(GeometryOptimizeDlg::IDD)
{
}

GeometryOptimizeDlg::~GeometryOptimizeDlg(void)
{
}

void GeometryOptimizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_ALLOW_OFFSET, m_ctrlAllowOffset);
}

BOOL GeometryOptimizeDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// display the params.
	m_ctrlAllowOffset.Init(m_dAllowOffset, 4, FALSE);

	SetBackgroundColor(UI_COLOR);
	UpdateData(FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(GeometryOptimizeDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	
	ON_EN_KILLFOCUS(IDC_EDIT_ALLOW_OFFSET, OnKillAllowOffset)
END_MESSAGE_MAP()

void GeometryOptimizeDlg::OnKillAllowOffset()
{
	m_dAllowOffset = m_ctrlAllowOffset.Get_Number();
}

void GeometryOptimizeDlg::OnBnClickedOk()
{
	OnOK();
}

END_CUTLEADER_NAMESPACE()
