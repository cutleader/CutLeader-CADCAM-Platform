#include "stdafx.h"
#include "PartInfoDlg.h"

#include "baseConst.h"
#include "DateHelper.h"
#include "RandomGen.h"
#include "DataBaseManager.h"

#include "PartCamData.h"
#include "PartCamDataList.h"
#include "PartCadData.h"
#include "Part.h"
#include "clPartResource.h"

#include "ExpertLibItemLoader.h"
#include "ParamConfigLoader.h"
#include "ParamConfigSelectDlg.h"
#include "ParamConfigLoader.h"
#include "DefaultParamConfig.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(PartInfoDlg, CDialogEx)

BEGIN_MESSAGE_MAP(PartInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SELECTSCHEME, OnSelectScheme)
END_MESSAGE_MAP()

void PartInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_NEWPART_EDIT_NAME, m_strPartName);
	DDX_Text(pDX, IDC_EDIT_EXPLIB_ITEM, m_strExpLibItem);
	DDX_Text(pDX, IDC_EDIT_CONFIG, m_strParamConfig);
}

PartInfoDlg::PartInfoDlg(CWnd* pParent /*=NULL*/)
						 : CLDialog(PartInfoDlg::IDD, pParent)
{
	m_iParamConfigID = INVALID_ID;
	m_strPartName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEW_PART);
}

PartInfoDlg::~PartInfoDlg()
{
}

BOOL PartInfoDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// Ĭ�ϵġ��������á���
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	DefaultParamConfigPtr pDflParamConfig = ParamConfigLoader::LoadDflParamConfig(pExpLibDBCon);

	// ר�ҿ����ơ�
	LONGLONG iExpLibItemID = pDflParamConfig->GetExpLibItemID();
	m_strExpLibItem = ExpertLibItemLoader::GetExpLibItemName(pExpLibDBCon, iExpLibItemID);

	// ����������Ϣ��
	m_iParamConfigID = pDflParamConfig->GetParamConfigID();
	m_strParamConfig = ParamConfigLoader::GetConfigName(pExpLibDBCon, m_iParamConfigID);

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void PartInfoDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	/************************************************************************/
	// create the Part.

	m_pPart.reset(new Part);
	m_pPart->SetName(m_strPartName);

	// init cad data.
	PartCadDataPtr pPartCadData(new PartCadData);
	m_pPart->SetCadData(pPartCadData);

	// init cam data.
	PartCamDataPtr pPartCamData(new PartCamData);
	pPartCamData->SetParamConfigID(m_iParamConfigID);
	PartCamDataListPtr pPartCamDataList(new PartCamDataList);
	pPartCamDataList->push_back(pPartCamData);
	m_pPart->SetCamDataList(pPartCamDataList);
	/************************************************************************/

	OnOK();
}

void PartInfoDlg::OnSelectScheme()
{
	DBConnectPtr pDBConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pDBConnect, m_iParamConfigID);

	ParamConfigSelectDlg dlg;
	dlg.SetCurData(iExpLibItemID, m_iParamConfigID);
	if (dlg.DoModal() == IDOK)
	{
		// get ExpertLibItem.
		LONGLONG iExpLibItemID = dlg.GetExpLibItemID();
		m_strExpLibItem = ExpertLibItemLoader::GetExpLibItemName(pDBConnect, iExpLibItemID);

		// get part scheme.
		m_iParamConfigID = dlg.GetParamConfigID();
		m_strParamConfig = ParamConfigLoader::GetConfigName(pDBConnect, m_iParamConfigID);

		UpdateData(FALSE);
	}
}

END_CUTLEADER_NAMESPACE()