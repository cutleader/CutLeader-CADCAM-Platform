#include "stdafx.h"
#include "NestJobInfoPage.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clUtilityResource.h"
#include "DateHelper.h"

#include "baseConst.h"
#include "clUILibResource.h"
#include "LineArc2DList.h"
#include "DataBaseManager.h"
#include "ImgData.h"

#include "PatternList.h"
#include "Part.h"
#include "PartCadData.h"
#include "PartCamData.h"
#include "PartItem.h"
#include "PartBuilder.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "PartManager.h"
#include "clPartResource.h"

#include "Sheet.h"
#include "SheetLoader.h"
#include "SheetWriter.h"
#include "SheetProcessor.h"
#include "SheetManager.h"

#include "NestPart.h"
#include "NestPartList.h"
#include "NestTask.h"
#include "NestTaskLoader.h"
#include "NestJobMaterialList.h"
#include "NestJobMaterial.h"
#include "NestJob.h"
#include "NestJobLoader.h"
#include "clNestResource.h"
#include "NestJobInfoDlg.h"
#include "NestJobMgr.h"

#include "clDataManagerResource.h"

#include "ParamConfig.h"
#include "ParamConfigLoader.h"

#include "IMaterialSize.h"
#include "MaterialLibLoader.h"
#include "MaterialProcessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const size_t MAX_TIP_TEXT_LENGTH = 1024;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(NestJobInfoPage, CResizableDialog)

NestJobInfoPage::NestJobInfoPage() : CLResizableDialog(NestJobInfoPage::IDD)
{
	m_bIgnoreSelectionChange_of_grid = FALSE;
}

NestJobInfoPage::~NestJobInfoPage()
{
}

void NestJobInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	/************************************************************************/
	// basic info group.

	DDX_Text(pDX, IDC_EDIT_JOB_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_JOB_COMMENT, m_strComment);

	DDX_Text(pDX, IDC_EDIT_CREATE_DATE, m_strCreateDate);
	DDX_Text(pDX, IDC_EDIT_MODIFY_DATE, m_strModifyDate);
	/************************************************************************/

	DDX_Control(pDX, IDC_LIST_SHEET, m_sheetGrid);
}

BEGIN_MESSAGE_MAP(NestJobInfoPage, CResizableDialog)
	ON_WM_PAINT()
	ON_WM_DESTROY()

	ON_EN_KILLFOCUS(IDC_EDIT_JOB_NAME, OnKillNestJobName)
	ON_EN_KILLFOCUS(IDC_EDIT_JOB_COMMENT, OnKillNestJobComment)
	ON_BN_CLICKED(IDC_BTN_MORE_NEST_INFO, OnMoreNestInfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SHEET, OnSelchangedJobSheet)
END_MESSAGE_MAP()

BOOL NestJobInfoPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	/************************************************************************/
	// layout controls.

	// "basic info" group
	AddAnchor(IDC_STATIC_BASE_INFO, TOP_LEFT, TOP_RIGHT);
	// name
	AddAnchor(IDC_EDIT_JOB_NAME, TOP_LEFT);
	AddAnchor(IDC_STATIC_NAME, TOP_LEFT);
	// comment
	AddAnchor(IDC_EDIT_JOB_COMMENT, TOP_LEFT);
	AddAnchor(IDC_STATIC_COMMENT, TOP_LEFT);
	// date
	AddAnchor(IDC_STATIC_CREATE_DATE, TOP_LEFT);
	AddAnchor(IDC_EDIT_CREATE_DATE, TOP_LEFT);
	AddAnchor(IDC_STATIC_MODIFY_DATE, TOP_LEFT);
	AddAnchor(IDC_EDIT_MODIFY_DATE, TOP_LEFT);

	// "sheet" group.
	AddAnchor(IDC_STATIC_SHEET_IN_NEST, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_SHEET, TOP_LEFT, BOTTOM_RIGHT);

	// "preview" group.
	AddAnchor(IDC_STATIC_PREVIEWER, TOP_RIGHT);
	AddAnchor(IDC_STATIC_SHEET, TOP_RIGHT);

	//
	AddAnchor(IDC_BTN_MORE_NEST_INFO, BOTTOM_RIGHT);
	/************************************************************************/

	m_previewWnd.SubclassDlgItem(IDC_STATIC_SHEET, this);

	InitGridStyle();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void NestJobInfoPage::OnPaint()
{
	CDialogEx::OnPaint();

	ImgData::DrawImgData(m_pImgData, &m_previewWnd);
}

void NestJobInfoPage::OnDestroy()
{
	ReleaseGridItemData();
	CResizableDialog::OnDestroy();
}

void NestJobInfoPage::OnKillNestJobName()
{
	UpdateData(TRUE);

	if (m_pNestJob)
	{
		if (m_pNestJob->GetName() != m_strName)
		{
			m_pNestJob->SetName(m_strName);
			m_pNestJob->SetModified(TRUE);
			GetParent()->SendMessage(WM_DATA_MODIFIED, 0, 0);
		}
	}
}

void NestJobInfoPage::OnKillNestJobComment()
{
	UpdateData(TRUE);

	if (m_pNestJob->GetComment() != m_strComment)
	{
		m_pNestJob->SetComment(m_strComment);
		m_pNestJob->SetModified(TRUE);
		GetParent()->SendMessage(WM_DATA_MODIFIED, 0, 0);
	}
}

void NestJobInfoPage::OnMoreNestInfo()
{
	// 加载排版任务
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	LONGLONG iTaskID = NestJobLoader::GetNestTaskID(pMainDBCon, m_pNestJob->GetID());	
	NestTaskPtr pNestTask = NestTaskLoader::LoadNestTask(pMainDBCon, iTaskID, false);
	m_pNestJob->SetNestTask(pNestTask);

	NestJobInfoDlg dlg;
	dlg.SetNestJob(m_pNestJob);
	dlg.DoModal();
}

void NestJobInfoPage::OnSelchangedJobSheet(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (!m_bIgnoreSelectionChange_of_grid)
	{
		// 预览板材。
		InitPreviewData(pNMListView->iItem);
		Invalidate();
	}

	*pResult = 0;
}

void NestJobInfoPage::DisplayNestJobInfo(NestJobPtr pNestJob)
{
	m_pNestJob = pNestJob;

	// init the basic info.
	m_strName = m_pNestJob->GetName();
	m_strComment = m_pNestJob->GetComment();
	m_strCreateDate = DateHelper::GetStrFromTimeStamp(m_pNestJob->GetCreateTime());
	m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pNestJob->GetModifyTime());

	UpdateData(FALSE);

	// init the grid.
	m_bIgnoreSelectionChange_of_grid = TRUE;
	InitSheetGridData();
	m_bIgnoreSelectionChange_of_grid = FALSE;

	// 更新预览数据。
	if (m_sheetGrid.GetItemCount() > 0)
	{
		InitPreviewData(0);
	}
	else
	{
		m_pImgData.reset(); // 清空预览。
	}

	// we do not change anything yet.
	m_pNestJob->SetModified(FALSE);
}

void NestJobInfoPage::UpdateModifyDate()
{
	m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pNestJob->GetModifyTime());
	UpdateData(FALSE);
}

void NestJobInfoPage::InitGridStyle()
{
	CString str;

	/************************************************************************/
	// for "m_sheetGrid" grid.

	// Requires one never uses column 0
	m_sheetGrid.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol = new CGridColumnTraitText;
	m_sheetGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	// "name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_sheetGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 100, 0, pNameCol);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_QUANTITY);
	CGridColumnTrait* pQuanCol = new CGridColumnTraitText;
	m_sheetGrid.InsertColumnTrait(3, str, LVCFMT_LEFT, 80, 0, pQuanCol);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_USED);
	CGridColumnTrait* pMatCol = new CGridColumnTraitText;
	m_sheetGrid.InsertColumnTrait(4, str, LVCFMT_LEFT, 100, 0, pMatCol);

	m_sheetGrid.SetExtendedStyle(m_sheetGrid.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	/************************************************************************/
}

void NestJobInfoPage::InitSheetGridData()
{
	m_sheetGrid.DeleteAllItems();

	NestJobMaterialListPtr pNestJobMats = m_pNestJob->GetNestJobMatList();
	for (unsigned int i = 0; i < pNestJobMats->size(); i++)
	{
		NestJobMaterialPtr pNestJobMat = pNestJobMats->at(i);

		// the sheet.
		SheetPtr pSheet = pNestJobMat->GetSheet();

		// insert a row into grid.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		if (pSheet)
			lvItem.lParam = (LPARAM)(new LONGLONG(pSheet->GetID()));
		else
			lvItem.lParam = (LPARAM)(new LONGLONG(INVALID_ID));
		m_sheetGrid.InsertItem(&lvItem);

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, i+1);
		m_sheetGrid.SetItemText(i, 1, strNum);

		// "sheet name" column
		CString strSheetName;
		strSheetName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		if (pSheet)
			strSheetName = pSheet->GetName();
		m_sheetGrid.SetItemText(i, 2, strSheetName);

		// the quantity of sheets.
		int iSheetCount = pNestJobMat->GetCount();
		CString strSheetCount;
		strSheetCount.Format(INT_FORMAT, iSheetCount);
		m_sheetGrid.SetItemText(i, 3, strSheetCount);

		// the material occupied.
		CString strMatSizeName;
		strMatSizeName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		if (pSheet)
		{
			DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
			IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, pSheet->GetMaterialSizeID());
			if (pMaterialSize)
				strMatSizeName = pMaterialSize->GetName();
		}
		m_sheetGrid.SetItemText(i, 4, strMatSizeName);
	}

	// 选择第一行。
	if (m_sheetGrid.GetItemCount() > 0)
	{
		m_sheetGrid.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_sheetGrid.SetFocus();
	}

	m_sheetGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void NestJobInfoPage::ReleaseGridItemData()
{
	for (int i = 0; i < m_sheetGrid.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_sheetGrid.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void NestJobInfoPage::InitPreviewData(int iRow)
{
	LONGLONG* pSheetID = (LONGLONG*)m_sheetGrid.GetItemData(iRow);
	DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(*pSheetID);
	if (pShtDBCon)
	{
		m_pImgData = SheetLoader::LoadPreviewData(pShtDBCon);
	}
}

END_CUTLEADER_NAMESPACE()
