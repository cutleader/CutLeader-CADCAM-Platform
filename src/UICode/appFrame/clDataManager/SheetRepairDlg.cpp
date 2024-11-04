#include "StdAfx.h"
#include "SheetRepairDlg.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clUtilityResource.h"
#include "DateHelper.h"

#include "DBConnect.h"
#include "DataBaseManager.h"
#include "ProductItemWriter.h"

#include "LogMessage.h"
#include "LogMessageList.h"
#include "baseConst.h"
#include "clBaseDataResource.h"

#include "Part.h"
#include "PartCadData.h"
#include "PartCamData.h"
#include "PartItem.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "PartBuilder.h"
#include "clPartResource.h"
#include "PartManager.h"

#include "Sheet.h"
#include "SheetLoader.h"
#include "SheetWriter.h"
#include "SheetManager.h"
#include "SheetProcessor.h"
#include "clDataManagerResource.h"

#include "ParamConfigSelectDlg.h"
#include "ParamConfig.h"
#include "clExpertLibResource.h"
#include "ExpertLibItemLoader.h"
#include "ParamConfigLoader.h"

#include "MaterialSizeSelectDlg.h"
#include "MaterialThickness.h"
#include "IMaterialSize.h"
#include "Material.h"
#include "MaterialLibLoader.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SheetRepairDlg, CDialogEx)

SheetRepairDlg::SheetRepairDlg()
				: CLDialog(SheetRepairDlg::IDD)
{
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_REPAIR);
	m_iSheetID = INVALID_ID;
}

SheetRepairDlg::~SheetRepairDlg(void)
{
}

void SheetRepairDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);

	DDX_Text(pDX, IDC_EDIT_EXPLIB_ITEM, m_strExpLibItem);
	DDX_Text(pDX, IDC_EDIT_SHEET_SCHEME, m_strConfig);

	DDX_Text(pDX, IDC_EDIT_MAT_TYPE, m_strMat);
	DDX_Text(pDX, IDC_EDIT_MAT_NAME, m_strMatThick);
	DDX_Text(pDX, IDC_EDIT_MAT_SIZE, m_strMatSize);
}

BOOL SheetRepairDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// init the data.
	InitMatData();
	InitSchemeData();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(SheetRepairDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_REPAIR_SCHEME, OnRepairScheme)
	ON_BN_CLICKED(IDC_BTN_REPAIR_MAT, OnRepairMat)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void SheetRepairDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
}

BOOL SheetRepairDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH SheetRepairDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
	{
		// here, message edit box will display the bk color same as the dlg.
		if (pWnd->GetDlgCtrlID() != IDC_EDIT_MSG)
		{
			// for "Invalid Data" edit box, show red bk color.
			CString strTmp;
			strTmp = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
			CString strText;
			pWnd->GetWindowText(strText);
			if (strText == strTmp)
			{
				pDC->SetBkMode(TRANSPARENT);
				return ::CreateSolidBrush(RGB(255, 0, 0)); 
			}
			else
				return hbr;
		}
	}

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void SheetRepairDlg::OnRepairScheme()
{
	ParamConfigSelectDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();

		// 得到选中的“参数配置”。
		LONGLONG iExpLibItemID = dlg.GetExpLibItemID();
		m_strExpLibItem = ExpertLibItemLoader::GetExpLibItemName(pExpLibDBCon, iExpLibItemID);
		LONGLONG iParamConfigID = dlg.GetParamConfigID();
		m_strConfig = ParamConfigLoader::GetConfigName(pExpLibDBCon, iParamConfigID);

		// 修改板材的“参数配置”。
		DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(m_iSheetID);
		SheetPtr pSheet = SheetLoader::LoadSheet(pShtDBCon, m_iSheetID);
		SheetProcessor::ChangeSheetConfig(pSheet, iParamConfigID);
		SheetProcessor::CheckLoopCutFeature(pSheet); // 处理空的“回路起切特征”

		UpdateData(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_SCHEME))->EnableWindow(FALSE);
	}
}

void SheetRepairDlg::OnRepairMat()
{
	MaterialSizeSelectDlg matSizeSelDlg;
	if (matSizeSelDlg.DoModal() == IDOK)
	{
		IMaterialSizePtr pMaterialSize = matSizeSelDlg.GetSelMatSize();

		// change the material of the sheet.
		DBConnectPtr pDBConnect = DataBaseManager::ConnectSheetDB(m_iSheetID);
		SheetPtr pSheet = SheetLoader::LoadSheet(pDBConnect, m_iSheetID, SHEETLOAD_BASIC);
		SheetProcessor::ChangeMatSize(pSheet, pMaterialSize);

		// 显示材料信息。
		DataProperty dataProp;
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, pMaterialSize->GetID(), dataProp);
		m_strMat = dataProp.GetItem(0).GetValue().bstrVal;
		m_strMatThick.Format(FLOAT_FORMAT_4, dataProp.GetItem(1).GetValue().dblVal);
		m_strMatSize = pMaterialSize->GetName();
		UpdateData(FALSE);

		// since we have repaired the material info, disable the button.
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_MAT))->EnableWindow(FALSE);
	}
}

void SheetRepairDlg::OnBnClickedOk()
{
	CString str, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_REPAIR);

	// check whether user has repaired the sheet.
	if (!SheetManager::VerifySheet(m_iSheetID))
	{
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	OnOK();
}

void SheetRepairDlg::InitMatData()
{
	DataProperty matLibProp;
	DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(m_iSheetID);
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	if (MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, SheetLoader::GetMaterialSizeID(pShtDBCon), matLibProp))
	{
		m_strMat = matLibProp.GetItem(0).GetValue().bstrVal;
		m_strMatThick.Format(FLOAT_FORMAT_4, matLibProp.GetItem(1).GetValue().dblVal);
		m_strMatSize = matLibProp.GetItem(6).GetValue().bstrVal;

		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_MAT))->EnableWindow(FALSE);
	}
	else
	{
		m_strMat = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_strMatThick = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_strMatSize = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);

		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_MAT))->EnableWindow(TRUE);
	}
}

void SheetRepairDlg::InitSchemeData()
{
	DataProperty expLibProp;
	DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(m_iSheetID);
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	if (ParamConfigLoader::GetParamConfigInfo(pExpLibDBCon, SheetLoader::GetParamConfigID(pShtDBCon), expLibProp))
	{
		m_strExpLibItem = expLibProp.GetItem(3).GetValue().bstrVal;
		m_strConfig = expLibProp.GetItem(2).GetValue().bstrVal;

		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_SCHEME))->EnableWindow(FALSE);
	}
	else
	{
		m_strExpLibItem = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_strConfig = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);

		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_SCHEME))->EnableWindow(TRUE);
	}
}

END_CUTLEADER_NAMESPACE()
