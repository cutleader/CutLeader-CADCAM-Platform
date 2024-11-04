#include "StdAfx.h"
#include "DefaultParamConfigSelectDlg.h"

#include "baseConst.h"
#include "DataBaseManager.h"

#include "clExpertLibResource.h"
#include "DefaultParamConfig.h"
#include "ExpertLibItemLoader.h"
#include "ParamConfigLoader.h"
#include "ParamConfigSelectDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(DefaultParamConfigSelectDlg, CDialogEx)

DefaultParamConfigSelectDlg::DefaultParamConfigSelectDlg()
				: CLDialog(DefaultParamConfigSelectDlg::IDD)
{
}

DefaultParamConfigSelectDlg::~DefaultParamConfigSelectDlg(void)
{
}

void DefaultParamConfigSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_EXPLIB_ITEM_NAME, m_strExpLibItemName);
	DDX_Text(pDX, IDC_EDIT_CONFIG, m_strConfigName);
}

BEGIN_MESSAGE_MAP(DefaultParamConfigSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SELECT_CONFIG, OnSelectConfig)
END_MESSAGE_MAP()

BOOL DefaultParamConfigSelectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// Display the DefaultParamConfig
	DisplayDflParamConfig();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void DefaultParamConfigSelectDlg::OnSelectConfig()
{
	ParamConfigSelectDlg dlg;
	dlg.SetCurData(m_pDflParamConfig->GetExpLibItemID(), m_pDflParamConfig->GetParamConfigID());
	if (dlg.DoModal() == IDOK)
	{
		DBConnectPtr pDBConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();

		// update the ExpertLibItem.
		LONGLONG iExpLibItemID = dlg.GetExpLibItemID();
		m_strExpLibItemName = ExpertLibItemLoader::GetExpLibItemName(pDBConnect, iExpLibItemID);
		m_pDflParamConfig->SetExpLibItemID(iExpLibItemID);

		// ¡°²ÎÊýÅäÖÃ¡±
		LONGLONG iParamConfigID = dlg.GetParamConfigID();
		m_strConfigName = ParamConfigLoader::GetConfigName(pDBConnect, iParamConfigID);
		m_pDflParamConfig->SetParamConfigID(iParamConfigID);

		UpdateData(FALSE);
	}
}

void DefaultParamConfigSelectDlg::OnBnClickedOk()
{
	OnOK();
}

void DefaultParamConfigSelectDlg::DisplayDflParamConfig()
{
	DBConnectPtr pDBConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	m_strExpLibItemName = ExpertLibItemLoader::GetExpLibItemName(pDBConnect, m_pDflParamConfig->GetExpLibItemID());
	m_strConfigName = ParamConfigLoader::GetConfigName(pDBConnect, m_pDflParamConfig->GetParamConfigID());

	UpdateData(FALSE);
}

END_CUTLEADER_NAMESPACE()
