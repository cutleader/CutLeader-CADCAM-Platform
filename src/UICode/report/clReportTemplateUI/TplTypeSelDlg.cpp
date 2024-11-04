#include "StdAfx.h"
#include "TplTypeSelDlg.h"

#include "baseConst.h"
#include "clReportResource.h"
#include "ReportTpl.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(TplTypeSelDlg, CDialogEx)

TplTypeSelDlg::TplTypeSelDlg(void) : CLDialog(TplTypeSelDlg::IDD)
{
}

TplTypeSelDlg::~TplTypeSelDlg(void)
{
}

void TplTypeSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_TPL_TYPE, m_ctrlTplType);
}

BEGIN_MESSAGE_MAP(TplTypeSelDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()

void TplTypeSelDlg::OnOK()
{
	m_tplType = (REPORT_TPL_TYPE)m_ctrlTplType.GetItemData(m_ctrlTplType.GetCurSel());

	CDialogEx::OnOK();
}

BOOL TplTypeSelDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	/************************************************************************/	
	// init the combo box.

	vector<REPORT_TPL_TYPE> tplTypeList = ReportTpl::GetAllTplType();
	for (unsigned int i = 0; i < tplTypeList.size(); i++)
	{
		REPORT_TPL_TYPE tplType = tplTypeList.at(i);

		CString strTplTypeName = ReportTpl::GetReportTplTypeName(tplType);
		m_ctrlTplType.AddString(strTplTypeName);
		m_ctrlTplType.SetItemData(i, (int)tplType);
	}

	m_ctrlTplType.SetCurSel(0);
	/************************************************************************/

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

END_CUTLEADER_NAMESPACE()
