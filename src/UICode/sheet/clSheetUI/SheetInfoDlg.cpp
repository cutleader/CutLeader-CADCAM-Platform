#include "stdafx.h"
#include "SheetInfoDlg.h"

#include "baseConst.h"
#include "baseConst.h"
#include "DataBaseManager.h"
#include "clBaseDataResource.h"

#include "Sheet.h"
#include "clSheetResource.h"
#include "SheetManager.h"

#include "ParamConfigSelectDlg.h"
#include "ParamConfigLoader.h"
#include "ParamConfig.h"
#include "ParamConfigLoader.h"
#include "DefaultParamConfig.h"
#include "ExpertLibItemLoader.h"

#include "Material.h"
#include "MaterialThickness.h"
#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "MaterialSizeSelectDlg.h"
#include "clMaterialLibResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(SheetInfoDlg, CDialogEx)

BEGIN_MESSAGE_MAP(SheetInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_SEL_MAC, OnSelectMachine)
	ON_BN_CLICKED(IDC_BTN_SEL_MAT, OnSelectMat)
END_MESSAGE_MAP()

void SheetInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_NAME, m_strSheetName);

	// Material Info group.
	DDX_Text(pDX, IDC_EDIT_MAT_TYPE, m_strMat);
	DDX_Text(pDX, IDC_EDIT_MAT_NAME, m_strMatThick);
	DDX_Text(pDX, IDC_EDIT_MAT_SIZE, m_strMatSize);

	// scheme info group.
	DDX_Text(pDX, IDC_EDIT_MACHINE, m_strExpLibItem);
	DDX_Text(pDX, IDC_EDIT_PARAM_CONFIG, m_strParamConfig);
}

SheetInfoDlg::SheetInfoDlg(CWnd* pParent /*=NULL*/)
				: CLDialog(SheetInfoDlg::IDD, pParent)
{
	m_iParamConfigID = INVALID_ID;
	m_iMaterialSizeID = INVALID_ID;
}

SheetInfoDlg::~SheetInfoDlg()
{
}

BOOL SheetInfoDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_strSheetName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEW_SHEET);

	/************************************************************************/
	// init Material Info group.

	// 默认材料。
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	m_iMaterialSizeID = MaterialLibLoader::GetDflMatSize(pMatLibDBCon);
	DataProperty dataProp;
	MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, m_iMaterialSizeID, dataProp);

	m_strMat = dataProp.GetItem(0).GetValue().bstrVal;
	m_strMatThick.Format(FLOAT_FORMAT_4, dataProp.GetItem(1).GetValue().dblVal);
	m_strMatSize = dataProp.GetItem(6).GetValue().bstrVal;
	/************************************************************************/

	/************************************************************************/
	// init the "scheme info" group.

	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	DefaultParamConfigPtr pDflParamConfig = ParamConfigLoader::LoadDflParamConfig(pExpLibDBCon);

	// expert library item name.
	LONGLONG iExpLibItemID = pDflParamConfig->GetExpLibItemID();
	m_strExpLibItem = ExpertLibItemLoader::GetExpLibItemName(pExpLibDBCon, iExpLibItemID);

	// get the sheet 参数配置名称.
	m_iParamConfigID = pDflParamConfig->GetParamConfigID();
	m_strParamConfig = ParamConfigLoader::GetConfigName(pExpLibDBCon, m_iParamConfigID);
	/************************************************************************/

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

void SheetInfoDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	// check the available material count.
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, m_iMaterialSizeID);
	if (pMaterialSize->GetAvailNum() < 1)
	{
		CString strProductName, strMsgContent;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_MAT_SIZE_NONE);
		MessageBox(strMsgContent, strProductName, MB_OK | MB_ICONINFORMATION);
		return;
	}

	OnOK();
}

void SheetInfoDlg::OnSelectMachine()
{
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pExpLibDBCon, m_iParamConfigID);

	ParamConfigSelectDlg dlg;
	dlg.SetCurData(iExpLibItemID, m_iParamConfigID);
	if (dlg.DoModal() == IDOK)
	{
		UpdateData(TRUE);

		// get ExpertLibItem.
		LONGLONG iExpLibItemID = dlg.GetExpLibItemID();
		m_strExpLibItem = ExpertLibItemLoader::GetExpLibItemName(pExpLibDBCon, iExpLibItemID);

		m_iParamConfigID = dlg.GetParamConfigID();
		m_strParamConfig = ParamConfigLoader::GetConfigName(pExpLibDBCon, m_iParamConfigID);

		UpdateData(FALSE);
	}
}

void SheetInfoDlg::OnSelectMat()
{
	MaterialSizeSelectDlg matSizeSelDlg;
	if (matSizeSelDlg.DoModal() == IDOK)
	{
		IMaterialSizePtr pMaterialSize = matSizeSelDlg.GetSelMatSize();
		m_iMaterialSizeID = pMaterialSize->GetID();

		DataProperty dataProp;
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, m_iMaterialSizeID, dataProp);
		m_strMat = dataProp.GetItem(0).GetValue().bstrVal;
		m_strMatThick.Format(FLOAT_FORMAT_4, dataProp.GetItem(1).GetValue().dblVal);
		m_strMatSize = pMaterialSize->GetName();

		UpdateData(FALSE);
	}
}

SheetPtr SheetInfoDlg::GetSheet()
{
	SheetPtr pSheet = SheetManager::CreateSheet(m_strSheetName, m_iMaterialSizeID, m_iParamConfigID);
	return pSheet;
}

END_CUTLEADER_NAMESPACE()
