#include "stdafx.h"
#include "SheetInfoPage.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "IntegerList.h"
#include "clUtilityResource.h"
#include "DateHelper.h"
#include "ImgData.h"
#include "LogMessage.h"
#include "LogMessageList.h"
#include "LogMgr.h"
#include "baseConst.h"
#include "clUILibResource.h"
#include "LineArc2DList.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"

#include "PatternList.h"
#include "Part.h"
#include "PartCamData.h"
#include "PartItem.h"
#include "PartBuilder.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "PartManager.h"
#include "clPartResource.h"

#include "PartPlacementList.h"
#include "Sheet.h"
#include "SheetLoader.h"
#include "SheetWriter.h"
#include "SheetProcessor.h"
#include "PartPlacementLoader.h"
#include "SheetManager.h"

#include "NestJobLoader.h"

#include "clDataManagerResource.h"
#include "SheetRepairDlg.h"
#include "DataItemTracker.h"

#include "ParamConfig.h"
#include "ParamConfigSelectDlg.h"
#include "ParamConfigLoader.h"
#include "ExpertLibItemLoader.h"
#include "clExpertLibResource.h"

#include "Material.h"
#include "MaterialThickness.h"
#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "MaterialSizeSelectDlg.h"
#include "MaterialProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const size_t MAX_TIP_TEXT_LENGTH = 1024;

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SheetInfoPage, CResizableDialog)

SheetInfoPage::SheetInfoPage() : CLResizableDialog(SheetInfoPage::IDD)
{
	m_pSheet.reset();
}

SheetInfoPage::~SheetInfoPage()
{
}

void SheetInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	// basic info group.
	DDX_Text(pDX, IDC_EDIT_SHEET_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_SHEET_COMMENT, m_strComment);
	DDX_Text(pDX, IDC_EDIT_CREATE_DATE, m_strCreateDate);
	DDX_Text(pDX, IDC_EDIT_MODIFY_DATE, m_strModifyDate);

	// Material Info group.
	DDX_Text(pDX, IDC_EDIT_MAT_TYPE, m_strMat);
	DDX_Text(pDX, IDC_EDIT_MAT_THICK, m_strMatThick);
	DDX_Text(pDX, IDC_EDIT_MAT_SIZE, m_strMatSize);

	// scheme info group.
	DDX_Text(pDX, IDC_EDIT_MACHINE, m_strExpLibItem);
	DDX_Text(pDX, IDC_EDIT_CONFIG, m_strParamConfig);

	DDX_Control(pDX, IDC_LIST_PART, m_partItemGrid);
	DDX_Control(pDX, IDC_LIST_NEST, m_nestGrid);
}

BEGIN_MESSAGE_MAP(SheetInfoPage, CResizableDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()

	ON_EN_KILLFOCUS(IDC_EDIT_SHEET_NAME, OnKillSheetName)
	ON_EN_KILLFOCUS(IDC_EDIT_SHEET_COMMENT, OnKillSheetComment)

	ON_BN_CLICKED(IDC_BTN_SEL_MAT, OnChangeMat)
	ON_BN_CLICKED(IDC_BTN_SEL_CONFIG, OnChangeScheme)
	ON_BN_CLICKED(IDC_BTN_REPAIR_SHEET, OnRepairSheet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL SheetInfoPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	/************************************************************************/
	// layout controls.

	// "basic info" group
	AddAnchor(IDC_STATIC_BASE_INFO, TOP_LEFT);
	// name
	AddAnchor(IDC_EDIT_SHEET_NAME, TOP_LEFT);
	AddAnchor(IDC_STATIC_SHEET_NAME, TOP_LEFT);
	// comment
	AddAnchor(IDC_EDIT_SHEET_COMMENT, TOP_LEFT);
	AddAnchor(IDC_STATIC_SHEET_COMMENT, TOP_LEFT);
	// date
	AddAnchor(IDC_STATIC_CREATE_DATE, TOP_LEFT);
	AddAnchor(IDC_EDIT_CREATE_DATE, TOP_LEFT);
	AddAnchor(IDC_STATIC_MODIFY_DATE, TOP_LEFT);
	AddAnchor(IDC_EDIT_MODIFY_DATE, TOP_LEFT);

	// "material" group.
	AddAnchor(IDC_STATIC_MAT, TOP_LEFT);
	// mat type
	AddAnchor(IDC_EDIT_MAT_TYPE, TOP_LEFT);
	AddAnchor(IDC_STATIC_MAT_TYPE, TOP_LEFT);
	// mat thickness
	AddAnchor(IDC_EDIT_MAT_THICK, TOP_LEFT);
	AddAnchor(IDC_STATIC_MAT_THICK, TOP_LEFT);
	// mat size
	AddAnchor(IDC_EDIT_MAT_SIZE, TOP_LEFT);
	AddAnchor(IDC_STATIC_MAT_SIZE, TOP_LEFT);
	//
	AddAnchor(IDC_BTN_SEL_MAT, TOP_LEFT);

	// "scheme" group.
	AddAnchor(IDC_STATIC_CONFIG, TOP_LEFT);
	// expert library item.
	AddAnchor(IDC_STATIC_EXPLIB_ITEM, TOP_LEFT);
	AddAnchor(IDC_EDIT_MACHINE, TOP_LEFT);
	// sheet scheme.
	AddAnchor(IDC_STATIC_SHEET_CONFIG, TOP_LEFT);
	AddAnchor(IDC_EDIT_CONFIG, TOP_LEFT);
	//
	AddAnchor(IDC_BTN_SEL_CONFIG, TOP_LEFT);

	// "preview" group.
	AddAnchor(IDC_STATIC_PREVIEWER, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_STATIC_PREVIEWPART, TOP_LEFT, TOP_RIGHT);

	// "involved parts" group.
	AddAnchor(IDC_STATIC_PARTS, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_LIST_PART, TOP_LEFT, TOP_RIGHT);

	// "relative nest jobs" group.
	AddAnchor(IDC_STATIC_NESTJOBS, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_NEST, TOP_LEFT, BOTTOM_RIGHT);

	//
	AddAnchor(IDC_BTN_REPAIR_SHEET, BOTTOM_RIGHT);
	/************************************************************************/

	//
	m_previewWnd.SubclassDlgItem(IDC_STATIC_PREVIEWPART, this);

	InitGridStyle();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void SheetInfoPage::OnDestroy()
{
	ReleaseGridItemData();
	CResizableDialog::OnDestroy();
}

void SheetInfoPage::OnPaint()
{
	CResizableDialog::OnPaint();

	UpdateCtrlStatus();
	PreviewSheet();
}

HBRUSH SheetInfoPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
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

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEWPART)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void SheetInfoPage::OnKillSheetName()
{
	UpdateData(TRUE);

	if (m_pSheet->GetName() != m_strName)
	{
		m_pSheet->SetName(m_strName);

		//
		m_pSheet->SetModified(TRUE);
		GetParent()->SendMessage(WM_DATA_MODIFIED, 0, 0);
	}
}

void SheetInfoPage::OnKillSheetComment()
{
	UpdateData(TRUE);

	if (m_pSheet->GetComment() != m_strComment)
	{
		m_pSheet->SetComment(m_strComment);

		//
		m_pSheet->SetModified(TRUE);
		GetParent()->SendMessage(WM_DATA_MODIFIED, 0, 0);
	}
}

void SheetInfoPage::OnChangeMat()
{
	// prepare some data.
	CString str, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// check whether the sheet is opened.
	if (DataItemTracker::GetInstance()->IsDataItemOpened(m_pSheet->GetID()))
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_OPENED);
		str.Replace(_T("WILLBEREPLACED1"), m_pSheet->GetName());
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	MaterialSizeSelectDlg matSizeSelDlg;
	if (matSizeSelDlg.DoModal() == IDOK)
	{
		IMaterialSizePtr pMaterialSize = matSizeSelDlg.GetSelMatSize();

		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CHANGE_MAT);
		if (MessageBox(str, NULL, MB_YESNO | MB_ICONWARNING) == IDYES)
		{
			// change the material of the sheet.
			SheetProcessor::ChangeMatSize(m_pSheet, pMaterialSize);

			// 显示材料信息。
			DataProperty dataProp;
			DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
			MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, pMaterialSize->GetID(), dataProp);
			m_strMat = dataProp.GetItem(0).GetValue().bstrVal;
			m_strMatThick.Format(FLOAT_FORMAT_4, dataProp.GetItem(1).GetValue().dblVal);
			m_strMatSize = pMaterialSize->GetName();
			m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pSheet->GetModifyTime());
		}

		UpdateData(FALSE);
	}
}

void SheetInfoPage::OnChangeScheme()
{
	// prepare some data.
	CString str, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// check whether the sheet is opened.
	if (DataItemTracker::GetInstance()->IsDataItemOpened(m_pSheet->GetID()))
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_OPENED);
		str.Replace(_T("WILLBEREPLACED1"), m_pSheet->GetName());
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	// the ExpertLibItem used by the sheet.
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iExpLibItemID = ExpertLibItemLoader::GetExpLibItemIDByName(pExpLibDBCon, m_strExpLibItem);

	ParamConfigSelectDlg dlg;
	dlg.SetCurData(iExpLibItemID, INVALID_ID);
	if (dlg.DoModal() == IDOK)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CHANGE_SHEETSCHEME);
		if (MessageBox(str, NULL, MB_YESNO | MB_ICONWARNING) == IDYES)
		{
			LONGLONG iConfigID = dlg.GetParamConfigID();

			// load the sheet with "SHEETLOAD_BASIC" option.
			DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(m_pSheet->GetID());
			SheetPtr pSheet = SheetLoader::LoadSheet(pShtDBCon, m_pSheet->GetID(), SHEETLOAD_BASIC);

			// change and save the scheme of the sheet.
			SheetProcessor::ChangeSheetConfig(pSheet, iConfigID);

			// update the UI.
			m_strParamConfig = ParamConfigLoader::GetConfigName(pExpLibDBCon, iConfigID);
			m_strExpLibItem = ParamConfigLoader::GetExpLibItemName(pExpLibDBCon, iConfigID);
			m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pSheet->GetModifyTime());
		}

		UpdateData(FALSE);
	}
}

void SheetInfoPage::OnRepairSheet()
{
	SheetRepairDlg dlg;
	dlg.SetSheet(m_pSheet->GetID());
	dlg.DoModal();

	// after close the repair dialog, we should re-display the sheet info.
	UpdateAfterRepair();

	// update the status of the "repair" button.
	if (SheetManager::VerifySheet(m_pSheet->GetID()))
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_SHEET))->EnableWindow(FALSE);
	else
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_SHEET))->EnableWindow(TRUE);
}

void SheetInfoPage::DisplaySheetInfo(SheetPtr pSheet) 
{
	m_pSheet = pSheet;

	// init the basic and scheme data.
	m_strName = m_pSheet->GetName();
	m_strComment = m_pSheet->GetComment();
	m_strCreateDate = DateHelper::GetStrFromTimeStamp(m_pSheet->GetCreateTime());
	m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pSheet->GetModifyTime());

	// init Material Info group.
	InitMatGroup();

	// init the "scheme info" group.
	InitSchemeGroup();

	// init the part grid.
	InitPartGridData();

	// init the nest grid.
	InitNestGridData();

	UpdateData(FALSE);

	// update the status of the "repair" button.
	if (SheetManager::VerifySheet(m_pSheet->GetID()))
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_SHEET))->EnableWindow(FALSE);
	else
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR_SHEET))->EnableWindow(TRUE);

	// preview sheet.
	PreviewSheet();
}

void SheetInfoPage::UpdateAfterRepair()
{
	// init Material Info group.
	InitMatGroup();

	// init the "scheme info" group.
	InitSchemeGroup();

	// init the part grid.
	InitPartGridData();

	UpdateData(FALSE);
}

void SheetInfoPage::UpdateCtrlStatus()
{
	// check whether the sheet is opened.
	if (DataItemTracker::GetInstance()->IsDataItemOpened(m_pSheet->GetID()))
	{
		((CWnd*)GetDlgItem(IDC_EDIT_SHEET_NAME))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_EDIT_SHEET_COMMENT))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_SEL_MAT))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_SEL_CONFIG))->EnableWindow(FALSE);
	}
	else
	{
		((CWnd*)GetDlgItem(IDC_EDIT_SHEET_NAME))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_EDIT_SHEET_COMMENT))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_BTN_SEL_MAT))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_BTN_SEL_CONFIG))->EnableWindow(TRUE);
	}
}

void SheetInfoPage::UpdateModifyDate()
{
	m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pSheet->GetModifyTime());
	UpdateData(FALSE);
}

void SheetInfoPage::InitGridStyle()
{
	CString str;

	/************************************************************************/
	// for "m_partGrid" grid.

	// Requires one never uses column 0
	m_partItemGrid.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol = new CGridColumnTraitText;
	m_partItemGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	// "name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_partItemGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 100, 0, pNameCol);
	/************************************************************************/

	/************************************************************************/
	// for "m_nestGrid" grid.

	// Requires one never uses column 0
	m_nestGrid.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	pNUMCol = new CGridColumnTraitText;
	m_nestGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	// "name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	pNameCol = new CGridColumnTraitText;
	m_nestGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 100, 0, pNameCol);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CREATE_DATE);
	CGridColumnTrait* pCreateDateCol = new CGridColumnTraitText;
	m_nestGrid.InsertColumnTrait(3, str, LVCFMT_LEFT, 120, 0, pCreateDateCol);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MODIFY_DATE);
	CGridColumnTrait* pModifyDateCol = new CGridColumnTraitText;
	m_nestGrid.InsertColumnTrait(4, str, LVCFMT_LEFT, 120, 0, pModifyDateCol);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMENT);
	CGridColumnTrait* pCommentCol = new CGridColumnTraitText;
	m_nestGrid.InsertColumnTrait(5, str, LVCFMT_LEFT, 120, 0, pCommentCol);
	/************************************************************************/
}

void SheetInfoPage::InitPartGridData()
{
	// 得到板材中不同零件信息。
	DBConnectPtr pShtDBCon = DataBaseManager::GetInstance()->ConnectSheetDB(m_pSheet->GetID());
	vector<DataProperty> partInfoList = PartPlacementLoader::LoadPartInfoList(pShtDBCon);

	// 显示数据。
	ReleaseGridItemData();
	m_partItemGrid.DeleteAllItems();
	for (unsigned int i = 0; i < partInfoList.size(); i++)
	{
		DataProperty prop = partInfoList[i];

		// num column.
		CString str;
		str.Format(_T("%d"), i+1);
		m_partItemGrid.InsertItem(i, str);
		m_partItemGrid.SetItemText(i, 1, str);

		// part name
		VARIANT v = prop.GetItem(1).GetValue();
		m_partItemGrid.SetItemText(i, 2, (LPCTSTR)v.bstrVal);
	}

	m_partItemGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void SheetInfoPage::InitNestGridData()
{
	m_nestGrid.DeleteAllItems();

	// get the nest job which included the sheet.
	vector<DataProperty> nestJobInfoList;
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	if (NestJobLoader::GetJobInfo(pMainDBCon, m_pSheet->GetID(), nestJobInfoList))
	{
		for (unsigned int i = 0; i < nestJobInfoList.size(); i++)
		{
			DataProperty prop = nestJobInfoList.at(i);

			/************************************************************************/			
			// insert a row.

			// NO.
			CString str;
			str.Format(_T("%d"), i+1);
			m_nestGrid.InsertItem(i, str);
			m_nestGrid.SetItemText(i, 1, str);

			// job name
			VARIANT varJobName = prop.GetItem(0).GetValue();
			m_nestGrid.SetItemText(i, 2, (LPCTSTR)varJobName.bstrVal);

			// create date
			VARIANT varCreateDate = prop.GetItem(2).GetValue();
			m_nestGrid.SetItemText(i, 3, DateHelper::GetStrFromTimeStamp(varCreateDate.llVal));

			// modify date
			VARIANT varModDate = prop.GetItem(3).GetValue();
			m_nestGrid.SetItemText(i, 4, DateHelper::GetStrFromTimeStamp(varModDate.llVal));

			// job name
			VARIANT varJobComment = prop.GetItem(1).GetValue();
			m_nestGrid.SetItemText(i, 5, (LPCTSTR)varJobComment.bstrVal);
			/************************************************************************/
		}
	}

	m_nestGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void SheetInfoPage::PreviewSheet()
{
	ImgData::DrawImgData(m_pSheet->GetPreviewData(), &m_previewWnd);
}

void SheetInfoPage::InitMatGroup()
{
	// for XMessage.
	CString strTaskName;
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_VERIFY_SHEET);

	DataProperty matLibProp;
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	if (MaterialLibLoader::GetMatSizeProp(pMatLibDBCon, m_pSheet->GetMaterialSizeID(), matLibProp))
	{
		m_strMat = matLibProp.GetItem(0).GetValue().bstrVal;
		m_strMatThick.Format(FLOAT_FORMAT_4, matLibProp.GetItem(1).GetValue().dblVal);
		m_strMatSize = matLibProp.GetItem(6).GetValue().bstrVal;
	}
	else
	{
		m_strMat = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_strMatThick = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_strMatSize = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);

		// disable the button.
		((CWnd*)GetDlgItem(IDC_BTN_SEL_MAT))->EnableWindow(FALSE);

		// display the warning msg.
		CString strMsgContent;
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_MAT_LOST);
		strMsgContent.Replace(_T("WILLBEREPLACED1"), m_strName);
		LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_WARNING, strTaskName, strMsgContent));
		LogMgr::WriteLog(pLogMsg);
	}
}

void SheetInfoPage::InitSchemeGroup()
{
	// for XMessage.
	CString strTaskName;
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_VERIFY_SHEET);

	DataProperty expLibProp;
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	if (ParamConfigLoader::GetParamConfigInfo(pExpLibDBCon, m_pSheet->GetParamConfigID(), expLibProp))
	{
		m_strExpLibItem = expLibProp.GetItem(3).GetValue().bstrVal;
		m_strParamConfig = expLibProp.GetItem(2).GetValue().bstrVal;

		// disable the button.
		((CWnd*)GetDlgItem(IDC_BTN_SEL_CONFIG))->EnableWindow(TRUE);
	}
	else
	{
		m_strExpLibItem = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_strParamConfig = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);

		// disable the button.
		((CWnd*)GetDlgItem(IDC_BTN_SEL_CONFIG))->EnableWindow(FALSE);

		// display the warning msg.
		CString strMsgContent;
		strMsgContent = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SHEET_SCHEME_LOST);
		strMsgContent.Replace(_T("WILLBEREPLACED1"), m_strName);
		LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_WARNING, strTaskName, strMsgContent));
		LogMgr::WriteLog(pLogMsg);
	}
}

void SheetInfoPage::ReleaseGridItemData()
{
	for (int i = 0; i < m_partItemGrid.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_partItemGrid.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

END_CUTLEADER_NAMESPACE()
