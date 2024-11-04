#include "stdafx.h"
#include "ParamConfigDlg.h"

#include "baseConst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_CUTLEADER_NAMESPACE()

ParamConfigDlg::ParamConfigDlg(CWnd* pParent)
			: CLResizableDialog(ParamConfigDlg::IDD, pParent)
{
}

ParamConfigDlg::~ParamConfigDlg()
{
}

BEGIN_MESSAGE_MAP(ParamConfigDlg, CResizableDialog)
	ON_WM_SIZE()

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()

void ParamConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
}

BOOL ParamConfigDlg::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	// layout the controls on the dialog.
	AddAnchor(IDC_STATIC_CHILDAREA, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT);

	// 创建子对话框。
	m_paramConfigPage.Create(IDD_clExpertLibUI_ParamConfig, this);
	PlaceSubDlg();

	// 显示子对话框数据。
	m_paramConfigPage.DisplayPage(m_iParamConfigID, m_iCurrentIndex);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void ParamConfigDlg::OnSize(UINT nType, int cx, int cy)
{
	CResizableDialog::OnSize(nType, cx, cy);

	PlaceSubDlg();
}

void ParamConfigDlg::OnBnClickedOk()
{
	m_paramConfigPage.SaveModification(FALSE);

	CResizableDialog::OnOK();
}

void ParamConfigDlg::OnBnClickedCancel()
{
	m_paramConfigPage.SaveModification(TRUE);

	CResizableDialog::OnCancel();
}

void ParamConfigDlg::Init(LONGLONG iParamConfigID, int iCurrentIndex)
{ 
	m_iParamConfigID = iParamConfigID;
	m_iCurrentIndex = iCurrentIndex;
}

void ParamConfigDlg::PlaceSubDlg()
{
	CWnd* pWndPos = GetDlgItem(IDC_STATIC_CHILDAREA);
	if (pWndPos)
	{
		CRect pagePos;
		pWndPos->GetWindowRect(&pagePos);
		ScreenToClient(&pagePos);

		m_paramConfigPage.SetWindowPos(pWndPos, pagePos.left, pagePos.top, pagePos.Width(), pagePos.Height(), 0);
		m_paramConfigPage.ShowWindow(SW_SHOW);
	}
}

END_CUTLEADER_NAMESPACE()
