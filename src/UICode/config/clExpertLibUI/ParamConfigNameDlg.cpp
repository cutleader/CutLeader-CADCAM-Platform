#include "stdafx.h"
#include "ParamConfigNameDlg.h"

#include "baseConst.h"
#include "clExpertLibResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(ParamConfigNameDlg, CDialogEx)

ParamConfigNameDlg::ParamConfigNameDlg()
						 : CLDialog(ParamConfigNameDlg::IDD)
{
	m_strName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEW_PARAM_CONFIG);
}

ParamConfigNameDlg::~ParamConfigNameDlg()
{
}

void ParamConfigNameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_PARAM_CONFIG_NAME, m_strName);
}

BEGIN_MESSAGE_MAP(ParamConfigNameDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL ParamConfigNameDlg::OnInitDialog()
{
	__super::OnInitDialog();

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

void ParamConfigNameDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	OnOK();
}

END_CUTLEADER_NAMESPACE()
