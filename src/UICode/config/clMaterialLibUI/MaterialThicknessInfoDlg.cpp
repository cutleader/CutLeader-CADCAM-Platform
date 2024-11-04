#include "stdafx.h"
#include "MaterialThicknessInfoDlg.h"

#include "baseConst.h"
#include "MaterialThickness.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(MaterialThicknessInfoDlg, CDialog)

MaterialThicknessInfoDlg::MaterialThicknessInfoDlg(CWnd* pParent /*=NULL*/)
								: CLDialog(MaterialThicknessInfoDlg::IDD, pParent)
{
	m_pMatThick.reset();
}

MaterialThicknessInfoDlg::~MaterialThicknessInfoDlg()
{
}

void MaterialThicknessInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_MAT_THICKNESS, m_dMatThickness);
	DDX_Text(pDX, IDC_EDIT_MATNAME_COMMENT, m_strComment);
}

BEGIN_MESSAGE_MAP(MaterialThicknessInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

END_MESSAGE_MAP()

BOOL MaterialThicknessInfoDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_dMatThickness.Init(0.1, DIGITAL_NUM, FALSE);

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void MaterialThicknessInfoDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_pMatThick.reset(new MaterialThickness);
	CString strName;
	strName.Format(_T("%.2f"), m_dMatThickness.Get_Number());
	m_pMatThick->SetName(strName);
	m_pMatThick->SetThickness(m_dMatThickness.Get_Number());
	m_pMatThick->SetComment(m_strComment);

	OnOK();
}

END_CUTLEADER_NAMESPACE()
