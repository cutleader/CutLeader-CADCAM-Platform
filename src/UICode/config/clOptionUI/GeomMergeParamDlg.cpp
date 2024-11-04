#include "StdAfx.h"
#include "GeomMergeParamDlg.h"

#include "baseConst.h"
#include "clUtilityResource.h"

#include "ImpExpConfig.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(GeomMergeParamDlg, CDialogEx)

GeomMergeParamDlg::GeomMergeParamDlg()
	: CLDialog(GeomMergeParamDlg::IDD)
{
}

GeomMergeParamDlg::GeomMergeParamDlg(ImpExpConfigPtr pImpExpConfig)
				: CLDialog(GeomMergeParamDlg::IDD)
{
	m_pImpExpConfig = pImpExpConfig;
}

GeomMergeParamDlg::~GeomMergeParamDlg(void)
{
}

void GeomMergeParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_MERGE_COUNT, m_ctrlMergeCount);
	DDX_Control(pDX, IDC_EDIT_MERGE_DISTANCE, m_ctrlMergeDis);
}

BOOL GeomMergeParamDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_ctrlMergeCount.Init(m_pImpExpConfig->GetMergeItemCount(), 0, FALSE);
	m_ctrlMergeDis.Init(m_pImpExpConfig->GetMergeLineDis(), 4, FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BEGIN_MESSAGE_MAP(GeomMergeParamDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void GeomMergeParamDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_pImpExpConfig)
	{
		if (m_pImpExpConfig->GetMergeItemCount() != (int)m_ctrlMergeCount.Get_Number())
		{
			m_pImpExpConfig->SetMergeItemCount((int)m_ctrlMergeCount.Get_Number());
			m_pImpExpConfig->SetModified(TRUE);
		}

		if (m_pImpExpConfig->GetMergeLineDis() != m_ctrlMergeDis.Get_Number())
		{
			m_pImpExpConfig->SetMergeLineDis(m_ctrlMergeDis.Get_Number());
			m_pImpExpConfig->SetModified(TRUE);
		}
	}

	OnOK();
}

END_CUTLEADER_NAMESPACE()
