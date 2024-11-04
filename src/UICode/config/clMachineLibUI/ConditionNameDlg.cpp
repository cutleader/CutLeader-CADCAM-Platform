#include "StdAfx.h"
#include "ConditionNameDlg.h"

#include "baseConst.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(ConditionNameDlg, CDialog)

BEGIN_MESSAGE_MAP(ConditionNameDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

END_MESSAGE_MAP()

ConditionNameDlg::ConditionNameDlg(CWnd* pParent /*=NULL*/)
						: CLDialog(ConditionNameDlg::IDD, pParent)
{
}

ConditionNameDlg::~ConditionNameDlg(void)
{
}

BOOL ConditionNameDlg::OnInitDialog()
{
	__super::OnInitDialog();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void ConditionNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_CONDNAME, m_strName);
}

void ConditionNameDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	OnOK();
}

END_CUTLEADER_NAMESPACE()
