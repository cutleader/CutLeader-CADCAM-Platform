#include "StdAfx.h"
#include "CreatePartGridDlg.h"

#include "baseConst.h"
#include "clGeometryBaseResource.h"
#include "GlViewPort.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"

#include "CLOptionInfo.h"
#include "SheetEditParam.h"

#include "LoopToolData.h"
#include "LoopCutFeature.h"
#include "PartCamData.h"
#include "PartItem.h"

#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartPlacementManager.h"
#include "clPartLayoutResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(CreatePartGridDlg, CDialogEx)

CreatePartGridDlg::CreatePartGridDlg(CWnd* pParent /*=NULL*/)
	: CLDialog(CreatePartGridDlg::IDD, pParent)
{
}

CreatePartGridDlg::~CreatePartGridDlg(void)
{
}

void CreatePartGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_COMCUT, m_bComCut);
	DDX_Control(pDX, IDC_EDIT_PART_GAP, m_ctrlPartDis);
	DDX_Control(pDX, IDC_EDIT_ROW_COUNT, m_ctrlRowNum);
	DDX_Control(pDX, IDC_EDIT_COLUMN_COUNT, m_ctrlColumnNum);
	DDX_Control(pDX, IDC_COMBO_GRID_DIR, m_ctrlGridDir);

	DDX_Control(pDX, IDC_BTN_PREVIEW, m_ctrlPreview);
	DDX_Control(pDX, IDC_BTN_OK, m_ctrlOK);
	DDX_Control(pDX, IDC_BTN_CANCEL, m_ctrlCancel);
}

BOOL CreatePartGridDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// 零件距离。
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	m_ctrlPartDis.Init(pSheetEditParam->GetPartDis(), 4, FALSE);

	// 行列数。
	m_ctrlRowNum.Init(2, 0, FALSE);
	m_ctrlColumnNum.Init(2, 0, FALSE);

	// 阵列方向。
	CString strGridDir;
	strGridDir = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RIGHT_TOP);
	m_ctrlGridDir.InsertString(0, strGridDir); // 右上。
	strGridDir = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RIGHT_BOTTOM);
	m_ctrlGridDir.InsertString(1, strGridDir); // 右下。
	strGridDir = MultiLanguageMgr::GetInstance()->TranslateString(IDS_LEFT_TOP);
	m_ctrlGridDir.InsertString(2, strGridDir); // 左上。
	strGridDir = MultiLanguageMgr::GetInstance()->TranslateString(IDS_LEFT_BOTTOM);
	m_ctrlGridDir.InsertString(3, strGridDir); // 左下。
	m_ctrlGridDir.SetCurSel(0);

	// 按钮提示。
	CString strTips;
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_GRID_PREVIEW);
	m_ctrlPreview.SetTooltip(strTips);
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_GRID_OK);
	m_ctrlOK.SetTooltip(strTips);
	strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_QuitCurrentAction);
	m_ctrlCancel.SetTooltip(strTips);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CreatePartGridDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_CHECK_COMCUT, OnCheckComcut)
	ON_BN_CLICKED(IDC_BTN_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_BTN_OK, OnOK)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnCancel)
END_MESSAGE_MAP()

BOOL CreatePartGridDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH CreatePartGridDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0 && pWnd->GetDlgCtrlID() != IDC_EDIT_GRID_GAP_MSG)
		return hbr;

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

void CreatePartGridDlg::OnCheckComcut()
{
	UpdateData(TRUE);

	if (m_bComCut)
		((CWnd*)GetDlgItem(IDC_EDIT_PART_GAP))->EnableWindow(FALSE);
	else
		((CWnd*)GetDlgItem(IDC_EDIT_PART_GAP))->EnableWindow(TRUE);
}

void CreatePartGridDlg::OnPreview()
{
	// 创建零件阵列。
	PartPlacementPtr pGridPartPmt = CreatePartGrid();
	if (pGridPartPmt)
	{
		m_pGridPartPmts->clear();
		m_pGridPartPmts->push_back(pGridPartPmt);

		// 重绘视图。
		CWnd::FromHandle(m_pViewPort->GetWnd())->Invalidate();
	}
}

void CreatePartGridDlg::OnOK()
{
	// 创建零件阵列。
	PartPlacementPtr pGridPartPmt = CreatePartGrid();
	if (pGridPartPmt)
	{
		m_pGridPartPmts->clear();
		m_pGridPartPmts->push_back(pGridPartPmt);
		m_bCreateGrid = TRUE;

		// 重绘视图。
		CWnd::FromHandle(m_pViewPort->GetWnd())->Invalidate();

		// 隐藏对话框。
		ShowWindow(SW_HIDE);
	}
}

void CreatePartGridDlg::OnCancel()
{
	m_bCanceled = TRUE;

	// 隐藏对话框。
	ShowWindow(SW_HIDE);
}

void CreatePartGridDlg::ShowPartGridDlg(GlViewPortPtr pViewPort, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances,
										PartPlacementPtr pBasePartPmt, PartPlacementListPtr pGridPartPmts)
{
	m_pViewPort = pViewPort;
	m_pPartPmts = pPartPlacements;
	m_pPartInsts = pPartInstances;
	m_pBasePartPmt = pBasePartPmt;
	m_pGridPartPmts = pGridPartPmts;
	m_bCanceled = FALSE;
	m_bCreateGrid = FALSE;

	if (!IsWindowVisible())
	{
		// 显示对话框。
		SetParent(CWnd::FromHandle(m_pViewPort->GetWnd()));
		ShowWindow(SW_SHOW);
	}

	// 调整位置。
	CRect parentRect, winRect;
	CWnd::FromHandle(m_pViewPort->GetWnd())->GetWindowRect(parentRect);
	GetWindowRect(winRect);
	MoveWindow(parentRect.Width()-winRect.Width()-7, 2, winRect.Width(), winRect.Height());
}

PartPlacementPtr CreatePartGridDlg::CreatePartGrid()
{
	PartPlacementPtr pGridPartPmt;

	UpdateData(TRUE);

	// 检查行列数。
	int iRowNum = (int)m_ctrlRowNum.Get_Number();
	int iColNum = (int)m_ctrlColumnNum.Get_Number();
	if (iRowNum == 0 || iColNum == 0 || (iRowNum < 2 && iColNum < 2))
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_GRID_XY_NUM);
		MessageBox(str, strProductName, MB_OK | MB_ICONINFORMATION);
		return pGridPartPmt;
	}

	// 零件间距。
	double dPartDis;
	if (m_bComCut)
	{
		LoopCutFeaturePtr pBoundaryLoopFeature = m_pBasePartPmt->GetPartItem()->GetCamData()->GetBoundaryLoopFeature();
		dPartDis = pBoundaryLoopFeature->GetLoopTool()->GetToolWidth();
	}
	else
	{
		dPartDis = m_ctrlPartDis.Get_Number();
	}

	// 创建阵列。
	PART_GRID_DIR iGridDir = (PART_GRID_DIR)m_ctrlGridDir.GetCurSel();
	pGridPartPmt = PartPlacementManager::BuildGridPartPmt(m_pBasePartPmt, dPartDis, iRowNum, iColNum, iGridDir);

	return pGridPartPmt;
}

END_CUTLEADER_NAMESPACE()
