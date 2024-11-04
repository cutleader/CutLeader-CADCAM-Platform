#include "StdAfx.h"
#include "NestJobSelDlg.h"

#include "baseConst.h"
#include "clUtilityResource.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"
#include "ProductItemLoader.h"
#include "ImgData.h"

#include "clGeometryFeatureResource.h"
#include "PartItem.h"

#include "Sheet.h"
#include "clSheetResource.h"
#include "SheetLoader.h"

#include "NestPart.h"
#include "NestPartList.h"
#include "NestTask.h"
#include "NestMaterial.h"
#include "NestMaterialList.h"
#include "NestJobMaterialList.h"
#include "NestJobMaterial.h"
#include "NestJob.h"
#include "NestJobMgr.h"
#include "clNestResource.h"
#include "NestJobLoader.h"

#include "MaterialLibLoader.h"
#include "MaterialProcessor.h"
#include "IMaterialSize.h"
#include "clMaterialLibResource.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(NestJobSelDlg, CDialogEx)

NestJobSelDlg::NestJobSelDlg(CWnd* pParent /*=NULL*/)
		: CLDialog(NestJobSelDlg::IDD, pParent)
{
	m_pImgData.reset();
	m_bDlgInited = FALSE;
}

NestJobSelDlg::~NestJobSelDlg(void)
{
}

void NestJobSelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_NESTJOB, m_listNestJob);

	// for "Usage of Material" group.
	DDX_Control(pDX, IDC_EDIT_USED_MAT, m_ctrlUsedMatNum);
	DDX_Text(pDX, IDC_EDIT_UTILIZATION, m_strUtilization);
	DDX_Control(pDX, IDC_LIST_MAT, m_matGrid);

	// for "Nested Part Statistics" group.
	DDX_Control(pDX, IDC_EDIT_SUBMIT_COUNT, m_ctrlSubmitPartNum);
	DDX_Control(pDX, IDC_EDIT_NESTED_COUNT, m_ctrlNestedPartNum);
	DDX_Control(pDX, IDC_LIST_PARTINFO, m_nestPartGrid);

	DDX_Control(pDX, IDC_LIST_SHEET, m_sheetGrid);

	DDX_Control(pDX, IDC_STATIC_PREVIEW_SHT, m_previewWnd);
}

BOOL NestJobSelDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// 表格的样式。
	InitListStyle();

	// 列出所有“排版作业”。
	vector<DataProperty> nestJobInfoList;
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	ProductItemLoader::GetDataPropList(pMainDBCon, nestJobInfoList, DATANODE_NESTJOB);
	for (unsigned int i = 0; i < nestJobInfoList.size(); i++)
	{
		DataProperty prop = nestJobInfoList[i];
		m_listNestJob.AddString(prop.GetItem(1).GetValue().bstrVal);
		LONGLONG kkk = prop.GetItem(0).GetValue().llVal;
		m_listNestJob.SetItemData(i, (DWORD_PTR)(new LONGLONG(prop.GetItem(0).GetValue().llVal)));
	}
	m_listNestJob.SetCurSel(0);

	// 显示第一个排版作业。
	LONGLONG* pNestJobID = (LONGLONG*)m_listNestJob.GetItemData(0);	
	m_pNestJob = NestJobLoader::LoadNestJob(pMainDBCon, *pNestJobID, TRUE, true, SHEETLOAD_BASIC);
	DisplayNestJob();

	// 启用事件。
	m_bDlgInited = TRUE;

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BEGIN_MESSAGE_MAP(NestJobSelDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()

	ON_LBN_SELCHANGE(IDC_LIST_NESTJOB, OnSelchangedNestJob)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SHEET, OnSelchangedSheet)

	ON_BN_CLICKED(IDOK, OnOk)
END_MESSAGE_MAP()

void NestJobSelDlg::OnPaint()
{
	CDialogEx::OnPaint();

	ImgData::DrawImgData(m_pImgData, &m_previewWnd);
}

void NestJobSelDlg::OnDestroy()
{
	// 释放排版作业ID。
	for (int i = 0; i < m_listNestJob.GetCount(); i++)
	{
		DWORD_PTR pItemData = m_listNestJob.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}

	// 释放板材ID。
	for (int i = 0; i < m_sheetGrid.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_sheetGrid.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}

	CDialogEx::OnDestroy();
}

HBRUSH NestJobSelDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_SHT)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void NestJobSelDlg::OnSelchangedNestJob()
{
	int iIndex = m_listNestJob.GetCurSel();
	if (m_bDlgInited && iIndex >= 0)
	{
		// 加载排版任务。
		LONGLONG* pNestJobID = (LONGLONG*)m_listNestJob.GetItemData(iIndex);
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		m_pNestJob = NestJobLoader::LoadNestJob(pMainDBCon, *pNestJobID, TRUE, true, SHEETLOAD_BASIC);

		// 禁用事件。
		m_bDlgInited = FALSE;

		// 显示
		DisplayNestJob();

		// 启用事件。
		m_bDlgInited = TRUE;
	}
}

void NestJobSelDlg::OnSelchangedSheet(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_bDlgInited && pNMListView->iItem >= 0)
	{
		// 预览板材。
		InitPreviewData(pNMListView->iItem);
		Invalidate();
	}

	*pResult = 0;
}

void NestJobSelDlg::OnOk()
{
	if (m_nestPartGrid.GetItemCount() == 0 || m_matGrid.GetItemCount() == 0)
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_INVALID_NEST_JOB);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	CDialogEx::OnOK();
}

void NestJobSelDlg::InitListStyle()
{
	CString str;

	/************************************************************************/
	// 材料列表。

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	m_matGrid.InsertColumn(0, str, LVCFMT_LEFT, 40);

	// "material name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	m_matGrid.InsertColumn(1, str, LVCFMT_LEFT, 80);

	// "width"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_WIDTH);
	m_matGrid.InsertColumn(2, str, LVCFMT_LEFT, 90);

	// "height"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROP_HEIGHT);
	m_matGrid.InsertColumn(3, str, LVCFMT_LEFT, 90);

	// "matsize type" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_TYPE);
	m_matGrid.InsertColumn(4, str, LVCFMT_LEFT, 90);
	/************************************************************************/

	/************************************************************************/
	// 零件列表。

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	m_nestPartGrid.InsertColumn(0, str, LVCFMT_LEFT, 40);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	m_nestPartGrid.InsertColumn(1, str, LVCFMT_LEFT, 80);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SUBMITTED_NUM);
	m_nestPartGrid.InsertColumn(2, str, LVCFMT_LEFT, 120);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NEST_ROT_ANG);
	m_nestPartGrid.InsertColumn(3, str, LVCFMT_LEFT, 100);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_NESTED_NUM);
	m_nestPartGrid.InsertColumn(4, str, LVCFMT_LEFT, 90);

	m_nestPartGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/

	/************************************************************************/
	// sheet grid.

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	m_sheetGrid.InsertColumn(0, str, LVCFMT_LEFT, 40);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	m_sheetGrid.InsertColumn(1, str, LVCFMT_LEFT, 80);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COUNT);
	m_sheetGrid.InsertColumn(2, str, LVCFMT_LEFT, 50);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_USED);
	m_sheetGrid.InsertColumn(3, str, LVCFMT_LEFT, 80);

	m_sheetGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/
}

void NestJobSelDlg::DisplayNestJob()
{
	NestTaskPtr pNestTask = m_pNestJob->GetNestTask();
	NestMaterialListPtr pNestMatList = pNestTask->GetNestMatList();
	NestJobMaterialListPtr pNestJobMats = m_pNestJob->GetNestJobMatList();

	/************************************************************************/
	// init "Usage of Material" group.

	// calculate the count of the used materials.
	int iUsedMatCount = 0;
	for (unsigned int i = 0; i < pNestJobMats->size(); i++)
		iUsedMatCount += pNestJobMats->at(i)->GetCount();
	m_ctrlUsedMatNum.Init(iUsedMatCount, 0, FALSE);

	// calculate the utilization of material.
	map<LONGLONG, map<LONGLONG, int>> partCountDataInSheets; // 板材上不同零件的数量信息。map<板材id, map<零件id, 零件数量>>
	double dUtilization = NestJobMgr::CalcMatUtilization_fromDB(m_pNestJob.get(), partCountDataInSheets);
	if (dUtilization == .0)
		m_strUtilization = _T("0");
	else
		m_strUtilization.Format(_T("%.1f"), dUtilization*100);

	// 材料信息。
	m_matGrid.DeleteAllItems();
	for (unsigned int k = 0; k < pNestMatList->size(); k++)
	{
		NestMaterialPtr pNestMat = pNestMatList->at(k);

		// 检查材料在不在。
		if (!pNestMat)
			continue;

		// insert a new row.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = k;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		m_matGrid.InsertItem(&lvItem);

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, k+1);
		m_matGrid.SetItemText(k, 0, strNum);

		// 材料名称。
		IMaterialSizePtr pMaterialSize = pNestMat->GetMatSize();
		m_matGrid.SetItemText(k, 1, pMaterialSize->GetName());

		// "material width" column
		CString strMatLen;
		strMatLen.Format(FLOAT_FORMAT_2, pMaterialSize->GetWidth());
		m_matGrid.SetItemText(k, 2, strMatLen);

		// "material height" column
		CString strMatWidth;
		strMatWidth.Format(FLOAT_FORMAT_2, pMaterialSize->GetHeight());
		m_matGrid.SetItemText(k, 3, strMatWidth);

		// "matsize type" column
		CString strMatSizeType;
		if (pMaterialSize->GetType() == MAT_SIZE_RECT)
			strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_RECT);
		else if (pMaterialSize->GetType() == MAT_SIZE_POLY)
			strMatSizeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_POLY);
		m_matGrid.SetItemText(k, 4, strMatSizeType);
	}
	/************************************************************************/

	// 显示零件统计信息。
	{
		NestPartListPtr pNestPartList = pNestTask->GetNestPartList();

		// 提交的零件总数。
		int iSubmitPartCount = 0;
		for (unsigned int i = 0; i < pNestPartList->size(); i++)
			iSubmitPartCount += pNestPartList->at(i)->GetNestCount();
		m_ctrlSubmitPartNum.Init(iSubmitPartCount, 0, FALSE);

		// 每个零件的排版情况。
		m_nestPartGrid.DeleteAllItems();
		int iNestedPartCount = 0; // 已排零件的总数。
		for (unsigned int i = 0; i < pNestPartList->size(); i++)
		{
			NestPartPtr pNestPart = pNestPartList->at(i);

			// 判断零件是不是被删除。
			const PartItem* pPartItem = pNestPart->GetPartItem();
			if (!pPartItem)
				continue;

			// insert a new row.
			int iCount = m_nestPartGrid.GetItemCount();
			m_nestPartGrid.InsertItem(iCount, _T(""));

			// "num" column
			CString strNum;
			strNum.Format(INT_FORMAT, iCount+1);
			m_nestPartGrid.SetItemText(iCount, 0, strNum);

			// the part name column
			CString strPartName = pPartItem->GetPartName();
			m_nestPartGrid.SetItemText(iCount, 1, strPartName);

			// the part count column
			CString strPartCount;
			strPartCount.Format(INT_FORMAT, pNestPart->GetNestCount());
			m_nestPartGrid.SetItemText(iCount, 2, strPartCount);

			// 旋转角度。
			CString strRotAng = NestPart::GetRotStyleName(pNestPart->GetRotStyle());
			m_nestPartGrid.SetItemText(iCount, 3, strRotAng);

			// the nested count.
			int iNestedCount = NestJobMgr::GetNestedPartCount_fromDB(m_pNestJob.get(), pPartItem->GetPartID(), partCountDataInSheets);
			iNestedPartCount += iNestedCount;
			CString strNestedCount;
			strNestedCount.Format(INT_FORMAT, iNestedCount);
			m_nestPartGrid.SetItemText(iCount, 4, strNestedCount);
		}
		m_nestPartGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		// 已排零件的总数。
		m_ctrlNestedPartNum.Init(iNestedPartCount, 0, FALSE);
	}

	// 显示各个板材。
	{
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();

		// 清理列表。
		for (int i = 0; i < m_sheetGrid.GetItemCount(); i++)
		{
			DWORD_PTR pItemData = m_sheetGrid.GetItemData(i);
			delete (LONGLONG*)pItemData;
		}
		m_sheetGrid.DeleteAllItems();

		// 初始化列表。
		map<LONGLONG, IMaterialSizePtr> matSizeData; // 材料id对应的材料信息。<材料id, 材料数据>
		for (unsigned int i = 0; i < pNestJobMats->size(); i++)
		{
			NestJobMaterialPtr pNestJobMat = pNestJobMats->at(i);

			// 板材还在不在。
			SheetPtr pSheet = pNestJobMat->GetSheet();
			if (!pSheet)
				continue;

			// init a row.
			int iRowCount = m_sheetGrid.GetItemCount();
			LVITEM lvItem;
			lvItem.mask = LVIF_TEXT | LVIF_PARAM;
			lvItem.iItem = iRowCount;
			lvItem.iSubItem = 0;
			lvItem.pszText = _T("");
			lvItem.lParam = (LPARAM)(new LONGLONG(pSheet->GetID()));
			iRowCount = m_sheetGrid.InsertItem(&lvItem);

			// "Num" column.
			CString strNum;
			strNum.Format(_T("%d"), i+1);
			m_sheetGrid.SetItemText(iRowCount, 0, strNum);

			// "sheet Name" column.
			m_sheetGrid.SetItemText(iRowCount, 1, pSheet->GetName());

			// "Sheet count" column.
			CString strSheetCount;
			strSheetCount.Format(_T("%d"), pNestJobMat->GetCount());
			m_sheetGrid.SetItemText(iRowCount, 2, strSheetCount);

			// "Material Used" column.
			IMaterialSizePtr pMaterialSize;
			map<LONGLONG, IMaterialSizePtr>::iterator iter = matSizeData.find(pSheet->GetMaterialSizeID());
			if (iter == matSizeData.end())
			{
				pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, pSheet->GetMaterialSizeID());
				matSizeData[pSheet->GetMaterialSizeID()] = pMaterialSize;
			}
			else
			{
				pMaterialSize = iter->second;
			}
			m_sheetGrid.SetItemText(iRowCount, 3, pMaterialSize->GetName());
		}

		// 选择第一行。
		m_sheetGrid.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_sheetGrid.SetFocus();
	}

	// 更新预览数据。
	if (m_sheetGrid.GetItemCount() > 0)
	{
		InitPreviewData(0);
	}
	else
	{
		m_pImgData.reset(); // 清空预览。
	}

	// 刷新显示。
	Invalidate();
	UpdateData(FALSE);
}

void NestJobSelDlg::InitPreviewData(int iRow)
{
	LONGLONG* pSheetID = (LONGLONG*)m_sheetGrid.GetItemData(iRow);
	DBConnectPtr pShtDBCon = DataBaseManager::ConnectSheetDB(*pSheetID);
	m_pImgData = SheetLoader::LoadPreviewData(pShtDBCon);
}

END_CUTLEADER_NAMESPACE()
