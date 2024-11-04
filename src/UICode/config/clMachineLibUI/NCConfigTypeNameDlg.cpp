#include "StdAfx.h"
#include "NCConfigTypeNameDlg.h"

#include "baseConst.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(NCConfigTypeNameDlg, CDialog)

BEGIN_MESSAGE_MAP(NCConfigTypeNameDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

END_MESSAGE_MAP()

NCConfigTypeNameDlg::NCConfigTypeNameDlg(CWnd* pParent /*=NULL*/)
						: CLDialog(NCConfigTypeNameDlg::IDD, pParent)
{
}

NCConfigTypeNameDlg::~NCConfigTypeNameDlg(void)
{
}

BOOL NCConfigTypeNameDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void NCConfigTypeNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_NCCONFIG_NAME, m_strName);
}

void NCConfigTypeNameDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	OnOK();
}

END_CUTLEADER_NAMESPACE()
