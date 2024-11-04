#include "StdAfx.h"
#include "NestResultDlg.h"

#include "baseConst.h"
#include "UnitHelper.h"
#include "clUtilityResource.h"
#include "LineArc2DList.h"
#include "clBaseDataResource.h"
#include "ProductSaveInfoDlg.h"
#include "DataBaseManager.h"
#include "taoresource.h"

#include "PartCadData.h"
#include "PartItem.h"

#include "clPartLayoutResource.h"
#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "Sheet.h"
#include "clSheetResource.h"

#include "clDataImportExportResource.h"
#include "ExportManager.h"

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
#include "RemnantMatInfoDlg.h"
#include "NestResult.h"
#include "NestProcessor.h"
#include "AllResultDlg.h"
#include "LibraryData.h"

#include "ImpExpConfig.h"
#include "CLOptionInfo.h"
#include "ProductParam.h"

#include "MaterialProcessor.h"
#include "IMaterialSize.h"
#include "MaterialSizeList.h"
#include "NestConfigItem.h"

#include "clReportResource.h"
#include "ReportTpl.h"
#include "ReportTplList.h"
#include "ReportTplLoader.h"
#include "ReportData.h"
#include "ReportPage.h"
#include "ReportPageList.h"
#include "RptPageDrawer.h"
#include "LicenseManager.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(NestResultDlg, CDialogEx)

NestResultDlg::NestResultDlg(CWnd* pParent /*=NULL*/)
		: CLDialog(NestResultDlg::IDD, pParent)
{
	m_pNestJob.reset();
	m_iRetRstNum = 0;
	m_pGeom2DList_of_part.reset();
	m_pGeom2DList_of_mat.reset();
	m_pDashGeom2DList_of_mat.reset();
	m_bDlgInited = FALSE;
	m_bOpenSheet = TRUE;
	m_pNestResult.reset(new NestResult);
}

NestResultDlg::~NestResultDlg(void)
{
}

void NestResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// for "Usage of Material" group.
	DDX_Control(pDX, IDC_EDIT_TOTAL_MAT, m_ctrlTotalMatNum);
	DDX_Control(pDX, IDC_EDIT_USED_MAT, m_ctrlUsedMatNum);
	DDX_Text(pDX, IDC_EDIT_UTILIZATION, m_strUtilization);

	// for "Nested Part Statistics" group.
	DDX_Control(pDX, IDC_EDIT_SUBMIT_COUNT, m_ctrlSubmitPartNum);
	DDX_Control(pDX, IDC_EDIT_NESTED_COUNT, m_ctrlNestedPartNum);
	DDX_Control(pDX, IDC_LIST_PARTINFO, m_nestPartGrid);

	DDX_Control(pDX, IDC_LIST_SHEET, m_sheetGrid);
	DDX_Control(pDX, IDC_BTN_SAVE_REMNANT, m_ctrlSaveRemantMat);

	DDX_Control(pDX, IDC_STATIC_PREVIEW_SHT, m_previewWnd);
	DDX_Text(pDX, IDC_EDIT_PART_REGION_RECT, m_strPartRegionRect);
	DDX_Check(pDX, IDC_CHECK_OPEN_SHEET, m_bOpenSheet);

	DDX_Text(pDX, IDC_EDIT_STATUS, m_strStatus);
	DDX_Text(pDX, IDC_EDIT_RET_RST_NUM, m_strRetRstNum);
	DDX_Control(pDX, IDC_PIC_PROGRESS, m_ctrlTaskProgress);

	DDX_Control(pDX, IDC_BTN_EXPORT_RESULT, m_ctrlExportResult);
	DDX_Control(pDX, IDC_BTN_SAVE, m_ctrlSaveResult);
}

BOOL NestResultDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// no result returned yet.
	m_strStatus = _T("Running");
	m_strRetRstNum.Format(_T("%d"), m_iRetRstNum);

	// init the progress bar.
	if (m_ctrlTaskProgress.Load(MAKEINTRESOURCE(IDR_GIF_PROGRESS), _T("Gif")))
	{
		m_ctrlTaskProgress.Draw();
	}

	// 表格的样式。
	InitListStyle();

	// 按钮提示。
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEN_SKELE_REMANT_TIP);
	m_ctrlSaveRemantMat.SetTooltip(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXPORT_NEST_JOB);
	m_ctrlExportResult.SetTooltip(str);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SAVE_NEST_JOB);
	m_ctrlSaveResult.SetTooltip(str);

	m_bDlgInited = TRUE;

	// 初始化视口。
	Rect2D geomRect(.0, 10.0, .0, 10.0);
	m_previewWnd.InitViewPort(geomRect);

	// check edition type.
    LicenseCheckResultType iLicenseCheckResult = LicenseManager::CheckLicense();
	if (iLicenseCheckResult == LicenseCheckResult_ultimateEdition)
	{
		((CWnd*)GetDlgItem(IDC_BTN_PREVIEW_RPT))->ShowWindow(SW_SHOW);
		((CWnd*)GetDlgItem(IDC_BTN_PRINT_RPT))->ShowWindow(SW_SHOW);
	}
	else
	{
		((CWnd*)GetDlgItem(IDC_BTN_PREVIEW_RPT))->ShowWindow(SW_HIDE);
		((CWnd*)GetDlgItem(IDC_BTN_PRINT_RPT))->ShowWindow(SW_HIDE);
	}

	// 只有内部使用时才能查看历史排版。
	if (CLOptionInfo::GetInstance()->GetProductParam()->IsUseInside())
	{
		((CWnd*)GetDlgItem(IDC_BTN_VIEW_ALL_RESULT))->ShowWindow(SW_SHOW);
	}
	else
	{
		((CWnd*)GetDlgItem(IDC_BTN_VIEW_ALL_RESULT))->ShowWindow(SW_HIDE);
	}

	// start the watcher.
	m_nestRstWatcher.Init(m_pNestProcessor, m_pNestResult, m_pLibraryData->m_pNestConfigItem->GetAllowedTime());
	m_nestRstWatcher.Start();

	// start the timer.
	m_timer = SetTimer(1, 1000, 0);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void NestResultDlg::Print(CDC* pDC, CPrintInfo* pInfo) 
{
	// the report page which will display.
	UINT iCurPageIndex = pInfo->m_nCurPage;
	ReportPageListPtr pReportPages = m_pReportData->GetReportPageList();
	ReportPagePtr pReportPage = pReportPages->at(iCurPageIndex-1);

	// draw the current report page.
	RptPageDrawer::DrawRptPage(pReportPage, pDC);
}

void NestResultDlg::PreparePrinting(CPrintInfo* pInfo)
{
	// set the report page count.
	ReportPageListPtr pReportPages = m_pReportData->GetReportPageList();
	pInfo->SetMaxPage(pReportPages->size());
}

CString NestResultDlg::GetWndText()
{
	CString str;
	GetWindowText(str);
	return str;
}

void NestResultDlg::SetWndFocus()
{
	SetFocus();
}

BEGIN_MESSAGE_MAP(NestResultDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()

	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SHEET, OnSelchangedSheet)

	ON_BN_CLICKED(IDC_BTN_SAVE_REMNANT, OnSaveRemnant)
	ON_BN_CLICKED(IDC_BTN_VIEW_ALL_RESULT, OnAllResult)
	ON_BN_CLICKED(IDC_BTN_STOP, OnStopNest)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_RPT, OnPreviewRpt)
	ON_BN_CLICKED(IDC_BTN_PRINT_RPT, OnPrintRpt)
	ON_BN_CLICKED(IDC_BTN_EXPORT_RESULT, OnExportResult)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnSaveResult)
	ON_BN_CLICKED(IDC_BTN_QUIT, OnQuit)
END_MESSAGE_MAP()

void NestResultDlg::OnPaint()
{
	PreviewSheet();

	CDialogEx::OnPaint();
}

void NestResultDlg::OnDestroy()
{
	ReleaseData_in_sheetGrid();
	m_previewWnd.ReleaseViewPort();

	CDialogEx::OnDestroy();
}

HBRUSH NestResultDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void NestResultDlg::OnTimer(UINT nIDEvent)
{
	// update the elapsed time.
	CString strElapsedTime;
	strElapsedTime.Format(_T("%d"), m_pNestResult->GetElapsedTime());
	((CWnd*)GetDlgItem(IDC_EDIT_COST_TIME))->SetWindowText(strElapsedTime);

	// whether we displayed all results and kill the timer.
	bool bFinished = false;
	if (m_pNestResult->TaskFinished() && m_iRetRstNum == m_pNestResult->GetNestResultCount())
	{
		bFinished = true;
	}

	if (bFinished)
	{
		// stop the timer.
		KillTimer(m_timer);

		m_strStatus = _T("Stopped");
		UpdateData(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_STOP))->EnableWindow(FALSE);
		if (m_iRetRstNum > 0)
		{
			((CWnd*)GetDlgItem(IDC_BTN_SAVE_REMNANT))->EnableWindow(TRUE);
			((CWnd*)GetDlgItem(IDC_BTN_PREVIEW_RPT))->EnableWindow(TRUE);
			((CWnd*)GetDlgItem(IDC_BTN_PRINT_RPT))->EnableWindow(TRUE);
			((CWnd*)GetDlgItem(IDC_CHECK_OPEN_SHEET))->EnableWindow(TRUE);
			((CWnd*)GetDlgItem(IDC_BTN_SAVE))->EnableWindow(TRUE);
			((CWnd*)GetDlgItem(IDC_BTN_EXPORT_RESULT))->EnableWindow(TRUE);
			((CWnd*)GetDlgItem(IDC_BTN_VIEW_ALL_RESULT))->EnableWindow(TRUE);
		}
		((CWnd*)GetDlgItem(IDC_BTN_QUIT))->EnableWindow(TRUE);

		// 停止进度条动画。
		m_ctrlTaskProgress.UnLoad();
		m_ctrlTaskProgress.ShowWindow(SW_HIDE);
	}
	else
	{
		if (m_iRetRstNum < m_pNestResult->GetNestResultCount())
		{
			// get the next result.
			m_iRetRstNum++;
			NestJobMaterialListPtr pNestJobMats = m_pNestResult->GetNestResultByIndex(m_iRetRstNum-1);
			if (!m_pNestJob)
			{
				m_pNestJob = NestJobMgr::CreateNestJob(m_pNestTask, pNestJobMats);
			}
			else
			{
				m_pNestJob->SetNestJobMatList(pNestJobMats);
			}

			// display the nest result.
			{
				m_strRetRstNum.Format(_T("%d"), m_iRetRstNum);
				DisplayNestResult();
			}
		}
	}
}

BOOL NestResultDlg::OnMouseWheel(UINT nFlags, short iDelta, CPoint pt)
{
	// the rect of the polygon view window.
	CRect polyWndRect;
	m_previewWnd.GetWindowRect(&polyWndRect);

	// check whether the current cursor is on the material view window.
	if (polyWndRect.PtInRect(pt))
		m_previewWnd.ZoomViewPort(iDelta, pt);

	return TRUE;
}

void NestResultDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// check whether the current cursor is on the material view window.
	CRect polyWndRect;
	m_previewWnd.GetWindowRect(&polyWndRect);
	ScreenToClient(&polyWndRect);
	if (polyWndRect.PtInRect(point))
	{
		m_previewWnd.SetFocus();

		if (nFlags & MK_MBUTTON)
		{
			CPoint ptOffSet = point - m_previewWnd.GetPanReferPt();
			m_previewWnd.PanViewPort(ptOffSet.x, ptOffSet.y);
			m_previewWnd.SetPanReferPt(point);
		}
	}
}

void NestResultDlg::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_previewWnd.SetPanReferPt(point);
}

void NestResultDlg::OnSelchangedSheet(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_bDlgInited && pNMListView->iItem >= 0)
	{
		InitPreviewData(pNMListView->iItem);
		PreviewSheet();
	}

	*pResult = 0;
}

void NestResultDlg::OnSaveRemnant()
{
	// get the selected row.
	int iIndex = INVALID_IDX, iSelCount = 0;
	for (int i = 0; i < m_sheetGrid.GetItemCount(); i++)
	{
		if (m_sheetGrid.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			iIndex = i;
			iSelCount++;
		}
	}

	CString strProductName, str;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	if (iIndex == INVALID_IDX || iSelCount != 1)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SELECT_SHEET);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
	}
	else
	{
		// the selected sheet object.
		LONGLONG* pSheetID = (LONGLONG*)m_sheetGrid.GetItemData(iIndex);
		NestJobMaterialPtr pNestJobMat = m_pNestJob->GetNestJobMatList()->GetNestJobMat(*pSheetID);
		SheetPtr pSheet = pNestJobMat->GetSheet();

		// display the remnant material info which generate from the sheet.
		RemnantMatInfoDlg dlg(this);
		dlg.Init(pSheet);
		if (dlg.DoModal() == IDC_BTN_SAVE)
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_REM_MAT_SAVED);
			MessageBox(str, strProductName, MB_OK | MB_ICONINFORMATION);
		}
	}
}

void NestResultDlg::OnAllResult()
{
	AllResultDlg dlg;
	dlg.Init(m_pNestResult->GetNestResultCount());
	if (dlg.DoModal() == IDOK)
	{
		unsigned int iSelectedResultIndex = dlg.GetSelectedResultIndex();
		NestJobMaterialListPtr pNestJobMats = m_pNestResult->GetNestResultByIndex(iSelectedResultIndex);
		m_pNestJob->SetNestJobMatList(pNestJobMats);
		DisplayNestResult();
	}
}

void NestResultDlg::OnStopNest()
{
	CString strMsg, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_STOP_NEST);
	if (MessageBox(strMsg, strProductName, MB_YESNO|MB_ICONQUESTION) == IDYES)
	{
		// tell the nest kernel to stop nesting.
		m_pNestProcessor->StopNest();
	}
}

void NestResultDlg::OnPreviewRpt()
{
	if (GenerateRpt())
		PreviewData();
}

void NestResultDlg::OnPrintRpt()
{
	if (GenerateRpt())
		PrintData();
}

void NestResultDlg::OnExportResult()
{
	// 导出位置
	CString strFolder;
	{
		BROWSEINFO bi;
		memset(&bi, 0, sizeof(BROWSEINFO));
		bi.hwndOwner = GetSafeHwnd();
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		CString strTitle;
		strTitle = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NEST_EXPORT_FOLDER);
		bi.lpszTitle = strTitle;
		LPITEMIDLIST idl = SHBrowseForFolder(&bi);
		if (idl == NULL)
			return;
		SHGetPathFromIDList(idl, strFolder.GetBuffer(MAX_PATH));
		strFolder.ReleaseBuffer();
	}

	if (ExportManager::ExportNestJob(m_pNestJob, strFolder))
	{
		CString strProductName, strMsg;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXP_NEST_SUCCESS);
		MessageBox(strMsg, strProductName, MB_OK | MB_ICONINFORMATION);
	}
}

void NestResultDlg::OnSaveResult()
{
	UpdateData(TRUE);

	ProductSaveInfoDlg dlg;
	CString strTaskName;
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_SAVE_NESTJOB);
	dlg.Init(strTaskName, m_pWorkspace, m_pNestJob->GetName());
	if (dlg.DoModal() == IDOK)
	{
		m_pParentFolder = dlg.GetSelectFolder();
		m_strJobName = dlg.GetName();

		// 用任务名称替换板材名称
		for (unsigned int i = 0; i < m_pNestJob->GetNestJobMatList()->size(); i++)
		{
			NestJobMaterialPtr pNestJobMat = m_pNestJob->GetNestJobMatList()->at(i);
			SheetPtr pSheet = pNestJobMat->GetSheet();
			CString strSheetName = pSheet->GetName();
			CString str;
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET);
			strSheetName.Replace(str, m_strJobName);
			pSheet->SetName(strSheetName);
		}

		// 是否要炸开零件放置。
		if (!m_pLibraryData->m_pNestConfigItem->IsCreateGrid())
		{
			NestJobMgr::ExplodePartPmts_in_sheets(m_pNestJob);
		}

		EndDialog(IDC_BTN_SAVE);
	}
}

void NestResultDlg::OnQuit()
{
	CDialogEx::OnOK();
}

void NestResultDlg::Init(NestTaskPtr pNestTask, NestProcessorPtr pNestProcessor, FolderNodePtr pWorkspace, const AreaInfo& areaInfo, LibraryDataPtr pLibraryData)
{
	m_pWorkspace = pWorkspace;
	m_pNestTask = pNestTask;
	m_pNestProcessor = pNestProcessor;
	m_pLibraryData = pLibraryData;
	m_areaInfo = areaInfo;
}

FolderNodePtr NestResultDlg::GetSaveFolder(CString& strJobName) const
{
	strJobName = m_strJobName;
	return m_pParentFolder;
}

void NestResultDlg::InitListStyle()
{
	CString str;

	/************************************************************************/
	// nest part grid.

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	m_nestPartGrid.InsertColumn(0, str, LVCFMT_LEFT, 40);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	m_nestPartGrid.InsertColumn(1, str, LVCFMT_LEFT, 100);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTMGR_NEST_PRIORITY);
	m_nestPartGrid.InsertColumn(2, str, LVCFMT_LEFT, 100);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SUBMITTED_NUM);
	m_nestPartGrid.InsertColumn(3, str, LVCFMT_LEFT, 100);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_NESTED_NUM);
	m_nestPartGrid.InsertColumn(4, str, LVCFMT_LEFT, 100);

	m_nestPartGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/


	/************************************************************************/
	// sheet grid.

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	m_sheetGrid.InsertColumn(0, str, LVCFMT_LEFT, 40);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_NAME);
	m_sheetGrid.InsertColumn(1, str, LVCFMT_LEFT, 160);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_QUANTITY);
	m_sheetGrid.InsertColumn(2, str, LVCFMT_LEFT, 80);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATSIZE_USED);
	m_sheetGrid.InsertColumn(3, str, LVCFMT_LEFT, 110);

	m_sheetGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/
}

void NestResultDlg::DisplayNestResult()
{
	/************************************************************************/
	// init "Usage of Material" group.

	NestMaterialListPtr pNestMatList = m_pNestTask->GetNestMatList();

	// calculate the total material count.
	int iTotalMatCount = 0;
	for (unsigned int i = 0; i < pNestMatList->size(); i++)
		iTotalMatCount += pNestMatList->at(i)->GetMatSize()->GetAvailNum();
	m_ctrlTotalMatNum.Init(iTotalMatCount, 0, FALSE);

	// calculate the count of the used materials.
	int iUsedMatCount = 0;
	for (unsigned int i = 0; i < m_pNestJob->GetNestJobMatList()->size(); i++)
		iUsedMatCount += m_pNestJob->GetNestJobMatList()->at(i)->GetCount();
	m_ctrlUsedMatNum.Init(iUsedMatCount, 0, FALSE);

	// calculate the utilization of material.
	double dUtilization = NestJobMgr::CalcMatUtilizationEx(m_pNestJob, m_areaInfo, m_pLibraryData->m_pMatSizeList);
	if (dUtilization == .0)
		m_strUtilization = _T("0");
	else
		m_strUtilization.Format(_T("%.1f"), dUtilization*100);
	/************************************************************************/


	/************************************************************************/
	// 显示零件统计信息。

	NestPartListPtr pNestPartList = m_pNestTask->GetNestPartList();

	// calculate the count of the submitted parts.
	int iSubmitPartCount = 0;
	for (unsigned int i = 0; i < pNestPartList->size(); i++)
		iSubmitPartCount += pNestPartList->at(i)->GetNestCount();
	m_ctrlSubmitPartNum.Init(iSubmitPartCount, 0, FALSE);

	// calculate the count of the nested parts.
	int iNestedPartCount = 0;
	for (unsigned int i = 0; i < m_pNestJob->GetNestJobMatList()->size(); i++)
	{
		NestJobMaterialPtr pNestJobMat = m_pNestJob->GetNestJobMatList()->at(i);
		SheetPtr pSheet = pNestJobMat->GetSheet();
		int iSheetCount = pNestJobMat->GetCount();

		// go through each PartPlacement object in the sheet.
		PartPlacementListPtr pPartPlacements = pSheet->GetPartPlacements();
		for (unsigned int j = 0; j < pPartPlacements->size(); j++)
		{
			PartPlacementPtr pPartPlacement = pPartPlacements->at(j);

			// part count in the PartPlacement object.
			int iPartCount = 1;
			if (pPartPlacement->IsGrid())
				iPartCount = pPartPlacement->GetRowCount()*pPartPlacement->GetColCount();
			iNestedPartCount += iSheetCount*iPartCount;
		}
	}
	m_ctrlNestedPartNum.Init(iNestedPartCount, 0, FALSE);

	// go through each nest part.
	m_nestPartGrid.DeleteAllItems();
	for (unsigned int i = 0; i < pNestPartList->size(); i++)
	{
		NestPartPtr pNestPart = pNestPartList->at(i);

		// insert a new row.
		int iCount = m_nestPartGrid.GetItemCount();
		m_nestPartGrid.InsertItem(iCount, _T(""));

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, iCount+1);
		m_nestPartGrid.SetItemText(iCount, 0, strNum);

		// the part name column
		CString strPartName = pNestPart->GetPartItem()->GetPartName();
		m_nestPartGrid.SetItemText(iCount, 1, strPartName);

		// the part priority column
		CString strPartPriority;
		strPartPriority.Format(INT_FORMAT, pNestPart->GetPriority().m_iNestPriority);
		m_nestPartGrid.SetItemText(iCount, 2, strPartPriority);

		// the part count column
		CString strPartCount;
		strPartCount.Format(INT_FORMAT, pNestPart->GetNestCount());
		m_nestPartGrid.SetItemText(iCount, 3, strPartCount);

		// the nested count.
		CString strNestedCount;
		const PartItem* pPartItem = pNestPart->GetPartItem();
		strNestedCount.Format(INT_FORMAT, NestJobMgr::GetNestedPartCount(m_pNestJob.get(), pPartItem->GetPartID()));
		m_nestPartGrid.SetItemText(iCount, 4, strNestedCount);
	}

	m_nestPartGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/


	/************************************************************************/
	// 显示各个板材。
    ReleaseData_in_sheetGrid();
	m_sheetGrid.DeleteAllItems();
	for (unsigned int i = 0; i < m_pNestJob->GetNestJobMatList()->size(); i++)
	{
		NestJobMaterialPtr pNestJobMat = m_pNestJob->GetNestJobMatList()->at(i);
		SheetPtr pSheet = pNestJobMat->GetSheet();

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
		IMaterialSizePtr pMaterialSize = m_pLibraryData->m_pMatSizeList->GetItemByID(pSheet->GetMaterialSizeID());
		m_sheetGrid.SetItemText(iRowCount, 3, pMaterialSize->GetName());
	}

	// 选择第一行。
	m_sheetGrid.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	m_sheetGrid.SetFocus();
	/************************************************************************/


	// 初始化预览数据。
	if (m_sheetGrid.GetItemCount() > 0)
		InitPreviewData(0);

	UpdateData(FALSE);
}

void NestResultDlg::InitPreviewData(int iRow)
{
	LONGLONG* pSheetID = (LONGLONG*)m_sheetGrid.GetItemData(iRow);
	NestJobMaterialPtr pNestJobMat = m_pNestJob->GetNestJobMatList()->GetNestJobMat(*pSheetID);
	SheetPtr pSheet = pNestJobMat->GetSheet();

	// content of the sheet.
	m_pGeom2DList_of_part = pSheet->Calculate2DLineArcs();

	// get the line/arcs of the material border.
	IMaterialSizePtr pMaterialSize = m_pLibraryData->m_pMatSizeList->GetItemByID(pSheet->GetMaterialSizeID());
	m_pGeom2DList_of_mat = MaterialProcessor::GetBorderLineArcs(pMaterialSize.get());

	// get the useless region of the sheet.
	m_pDashGeom2DList_of_mat = MaterialProcessor::GetUselessRegions(pMaterialSize.get());
}

void NestResultDlg::PreviewSheet()
{
	if (m_pGeom2DList_of_mat && m_pGeom2DList_of_mat->size() > 0 && m_previewWnd.m_hWnd)
	{
		// 调整视口以适合板材外框。
		Rect2D sheetRect = m_pGeom2DList_of_mat->GetRectBox();
		m_previewWnd.FitRect(sheetRect);

		// 设置图形。
		LineArc2DListPtr pSolidGeom2DList(new LineArc2DList);
		pSolidGeom2DList->insert(pSolidGeom2DList->end(), m_pGeom2DList_of_part->begin(), m_pGeom2DList_of_part->end());
		pSolidGeom2DList->insert(pSolidGeom2DList->end(), m_pGeom2DList_of_mat->begin(), m_pGeom2DList_of_mat->end());
		m_previewWnd.SetSolidGeom2DList(pSolidGeom2DList);
		if (m_pDashGeom2DList_of_mat)
			m_previewWnd.SetDashGeom2DList(m_pDashGeom2DList_of_mat);

		// 绘制。
		m_previewWnd.Invalidate();

		// the rect of part region.
		Rect2D partsRect = m_pGeom2DList_of_part->GetRectBox();
		m_strPartRegionRect.Format(_T("%.3f * %.3f"), partsRect.GetWidth(), partsRect.GetHeight());
		UpdateData(FALSE);
	}
}

BOOL NestResultDlg::GenerateRpt()
{
	// 加载报表模板。
	ImpExpConfigPtr pImpExpConfig = CLOptionInfo::GetInstance()->GetImpExpConfig();
	LONGLONG iNestRptTplID = pImpExpConfig->GetNestRptTplID();
	ReportTplPtr pReportTpl = ReportTplLoader::LoadReportTpl(iNestRptTplID);
	if (pReportTpl)
	{
		CClientDC dc(this);
		UnitHelper::Init(dc.GetDeviceCaps(LOGPIXELSX));
		m_pReportData = NestJobMgr::GenerateNestRpt(m_pNestJob, pReportTpl);
	}
	else
	{
		CString str, strProductName;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_WRONG_DFL_TPL);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	return TRUE;
}

void NestResultDlg::ReleaseData_in_sheetGrid()
{
	for (int i = 0; i < m_sheetGrid.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_sheetGrid.GetItemData(i);
        delete (LONGLONG*)pItemData;
	}
}

END_CUTLEADER_NAMESPACE()