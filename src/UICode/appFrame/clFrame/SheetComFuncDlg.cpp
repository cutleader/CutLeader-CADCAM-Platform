#include "stdafx.h"
#include "SheetComFuncDlg.h"

#include "baseConst.h"
#include "clGeometryFeatureResource.h"
#include "clCutFeatureResource.h"
#include "clPartLayoutResource.h"
#include "clCutSequenceResource.h"
#include "clPostProcessorResource.h"
#include "clBaseDataResource.h"
#include "ProductFolderManager.h"
#include "ProductFileDefine.h"
#include "IntegerList.h"
#include "ProcessMgr.h"
#include "clKnowledgeBaseConfigResource.h"
#include "LicenseManager.h"
#include "CLOptionSheet.h"
#include "clOptionResource.h"
#include "clExpertLibResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SheetComFuncDlg, CResizableDialog)

SheetComFuncDlg::SheetComFuncDlg() : CLResizableDialog(SheetComFuncDlg::IDD)
{
}

SheetComFuncDlg::~SheetComFuncDlg()
{
}

void SheetComFuncDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	// 零件布局功能。
	DDX_Control(pDX, IDC_BTN_PART_FROM_DB, m_btnPartFromDB);
	DDX_Control(pDX, IDC_BTN_PART_FROM_DXF, m_btnPartFromDxf);
	DDX_Control(pDX, IDC_BTN_PART_FROM_ASM_DXF, m_btnPartFromAsmDxf);
	DDX_Control(pDX, IDC_BTN_SELECT_PART, m_btnSelectPart);
	DDX_Control(pDX, IDC_BTN_CREATE_GRID, m_btnCreateGrid);
	DDX_Control(pDX, IDC_BTN_DELETE_PART, m_btnDeletePart);
	DDX_Control(pDX, IDC_BTN_MEASURE_DIS, m_btnMeasureDis);

	// 编辑工艺。
	DDX_Control(pDX, IDC_BTN_ASSIGN_CAM, m_ctrlAssignCAMBtn);
	DDX_Control(pDX, IDC_BTN_RESET_CAM, m_ctrlResetCAMBtn);
	DDX_Control(pDX, IDC_BTN_CHECK_INTF, m_ctrlCheckIntfBtn);
	DDX_Control(pDX, IDC_BTN_EDIT_LOOPTOOL, m_ctrlLoopToolBtn);
	DDX_Control(pDX, IDC_BTN_EDIT_LOOP_LEAD, m_ctrlLoopLeadBtn);
	DDX_Control(pDX, IDC_BTN_CHANGE_START_PT, m_ctrlStartPtBtn);
	DDX_Control(pDX, IDC_BTN_CHANGE_CUT_DIR, m_ctrlCutDirBtn);
	DDX_Control(pDX, IDC_BTN_CHANGE_CUT_SIDE, m_ctrlCutSideBtn);

	// 编辑工序。
	DDX_Control(pDX, IDC_BTN_AUTO_SEQ, m_btnAutoSeq);
	DDX_Control(pDX, IDC_BTN_CLEAR_SEQ, m_btnClearSeq);
	DDX_Control(pDX, IDC_BTN_DEL_SEQ, m_btnDeleteSeq);
	DDX_Control(pDX, IDC_BTN_SEQ_ORDER, m_btnOrderSeq);
	DDX_Control(pDX, IDC_BTN_ADD_LOOP_SEQ, m_btnAddLoopSeq);
	DDX_Control(pDX, IDC_BTN_GENERATE_REM_CUTLINE, m_btnGenerateRemCutLine);
	DDX_Control(pDX, IDC_BTN_SIMULATE_SEQ, m_btnSimulateSeq);
	DDX_Control(pDX, IDC_BTN_GENERATE_NC, m_btnGenerateNC);

	// 设置。
	DDX_Control(pDX, IDC_BTN_PARAM_CONFIG, m_ctrlParamBtn);
	DDX_Control(pDX, IDC_BTN_SYSTEM_CONFIG, m_ctrlSysSetBtn);
	DDX_Control(pDX, IDC_BTN_MATLIB, m_ctrlMatLibBtn);
}

BEGIN_MESSAGE_MAP(SheetComFuncDlg, CResizableDialog)
	ON_WM_VSCROLL()
	ON_WM_SIZE()

	// 零件布局功能。
	ON_BN_CLICKED(IDC_BTN_PART_FROM_DB, OnAddPartFromDB)
	ON_BN_CLICKED(IDC_BTN_PART_FROM_DXF, OnAddPartFromDxf)
	ON_BN_CLICKED(IDC_BTN_PART_FROM_ASM_DXF, OnAddPartFromAsmDxf)
	ON_BN_CLICKED(IDC_BTN_SELECT_PART, OnSelectPart)
	ON_BN_CLICKED(IDC_BTN_CREATE_GRID, OnCreateGrid)
	ON_BN_CLICKED(IDC_BTN_DELETE_PART, OnDeletePart)
	ON_BN_CLICKED(IDC_BTN_MEASURE_DIS, OnMeasureDis)

	// 编辑工艺。
	ON_BN_CLICKED(IDC_BTN_ASSIGN_CAM, OnAssignCam)
	ON_BN_CLICKED(IDC_BTN_RESET_CAM, OnResetCam)
	ON_BN_CLICKED(IDC_BTN_CHECK_INTF, OnCheckIntf)
	ON_BN_CLICKED(IDC_BTN_EDIT_LOOPTOOL, OnEditLoopTool)
	ON_BN_CLICKED(IDC_BTN_EDIT_LOOP_LEAD, OnLoopLead)
	ON_BN_CLICKED(IDC_BTN_CHANGE_START_PT, OnStartPt)
	ON_BN_CLICKED(IDC_BTN_CHANGE_CUT_DIR, OnCutDir)
	ON_BN_CLICKED(IDC_BTN_CHANGE_CUT_SIDE, OnCutSide)

	// 编辑工序。
	ON_BN_CLICKED(IDC_BTN_AUTO_SEQ, OnAutoSeq)
	ON_BN_CLICKED(IDC_BTN_CLEAR_SEQ, OnClearSeq)
	ON_BN_CLICKED(IDC_BTN_DEL_SEQ, OnDeleteSeq)
	ON_BN_CLICKED(IDC_BTN_SEQ_ORDER, OnOrderSeq)
	ON_BN_CLICKED(IDC_BTN_ADD_LOOP_SEQ, OnAddLoopSeq)
	ON_BN_CLICKED(IDC_BTN_GENERATE_REM_CUTLINE, OnGenerateRemCutLine)
	ON_BN_CLICKED(IDC_BTN_SIMULATE_SEQ, OnSimulateSeq)
	ON_BN_CLICKED(IDC_BTN_GENERATE_NC, OnGenerateNC)

	// 设置。
	ON_BN_CLICKED(IDC_BTN_PARAM_CONFIG, OnParamConfig)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_CONFIG, OnSystemConfig)
	ON_BN_CLICKED(IDC_BTN_MATLIB, OnMatLib)
END_MESSAGE_MAP()

BOOL SheetComFuncDlg::OnInitDialog()
{
	__super::OnInitDialog();

	/************************************************************************/	
	// 按钮图片和提示。

	CString strTip;

	// 零件布局功能。
	m_btnPartFromDB.SetImage(IDB_ADD_PART_FROM_DB_24, IDB_ADD_PART_FROM_DB_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_FROM_DB_TIP);
	m_btnPartFromDB.SetTooltip(strTip);
	m_btnPartFromDxf.SetImage(IDB_ADD_PART_FROM_DXF_24, IDB_ADD_PART_FROM_DXF_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_FROM_DXF_TIP);
	m_btnPartFromDxf.SetTooltip(strTip);
	m_btnPartFromAsmDxf.SetImage(IDB_ADD_PART_FROM_ASM_DXF_24, IDB_ADD_PART_FROM_ASM_DXF_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_FROM_ASM_DXF_TIP);
	m_btnPartFromAsmDxf.SetTooltip(strTip);
	m_btnSelectPart.SetImage(IDB_SELECT_PART_24, IDB_SELECT_PART_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_SELECT);
	m_btnSelectPart.SetTooltip(strTip);
	m_btnCreateGrid.SetImage(IDB_CREATE_PART_GRID_24, IDB_CREATE_PART_GRID_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_CREATE_GRID);
	m_btnCreateGrid.SetTooltip(strTip);
	m_btnDeletePart.SetImage(IDB_DELETE_PART_24, IDB_DELETE_PART_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMAND_DEL_PARTPMT);
	m_btnDeletePart.SetTooltip(strTip);
	m_btnMeasureDis.SetImage(IDB_MEASURE_DIS_24, IDB_MEASURE_DIS_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MEASURE_DISTANCE_TIP);
	m_btnMeasureDis.SetTooltip(strTip);

	// 编辑工艺。
	m_ctrlAssignCAMBtn.SetImage(IDB_ASSIGN_CAM_24, IDB_ASSIGN_CAM_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ASSIGN_CUTFEATURE);
	m_ctrlAssignCAMBtn.SetTooltip(strTip);
	m_ctrlResetCAMBtn.SetImage(IDB_RESET_CAM_24, IDB_RESET_CAM_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RESET_CUTFEATURE);
	m_ctrlResetCAMBtn.SetTooltip(strTip);
	m_ctrlCheckIntfBtn.SetImage(IDB_CHECK_INTF_24, IDB_CHECK_INTF_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CHECK_INTERFERE);
	m_ctrlCheckIntfBtn.SetTooltip(strTip);
	//
	m_ctrlLoopToolBtn.SetImage(IDB_EDIT_LOOPTOOL_24, IDB_EDIT_LOOPTOOL_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EDIT_LOOPTOOL_TIP);
	m_ctrlLoopToolBtn.SetTooltip(strTip);
	//
	m_ctrlLoopLeadBtn.SetImage(IDB_LOOP_LEAD_24, IDB_LOOP_LEAD_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EDIT_LOOP_LEAD);
	m_ctrlLoopLeadBtn.SetTooltip(strTip);
	m_ctrlStartPtBtn.SetImage(IDB_LOOP_START_PT_24, IDB_LOOP_START_PT_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EDIT_LOOP_START_PT);
	m_ctrlStartPtBtn.SetTooltip(strTip);
	m_ctrlCutDirBtn.SetImage(IDB_CUT_DIR_24, IDB_CUT_DIR_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CHANGE_CUTDIR);
	m_ctrlCutDirBtn.SetTooltip(strTip);
	m_ctrlCutSideBtn.SetImage(IDB_CUT_SIDE_24, IDB_CUT_SIDE_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SWITCH_CUTSIDE);
	m_ctrlCutSideBtn.SetTooltip(strTip);

	// 编辑工序。
	m_btnAutoSeq.SetImage(IDB_ASSIGN_SEQ_24, IDB_ASSIGN_SEQ_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_AUTOSEQUENCE);
	m_btnAutoSeq.SetTooltip(strTip);
	m_btnClearSeq.SetImage(IDB_CLEAR_SEQ_24, IDB_CLEAR_SEQ_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_CLEARALLSEQUENCE);
	m_btnClearSeq.SetTooltip(strTip);
	m_btnDeleteSeq.SetImage(IDB_DELETE_SEQ_24, IDB_DELETE_SEQ_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REMOVESEQUENCE);
	m_btnDeleteSeq.SetTooltip(strTip);
	m_btnOrderSeq.SetImage(IDB_ORDER_SEQ_24, IDB_ORDER_SEQ_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_SORTSEQUENCE);
	m_btnOrderSeq.SetTooltip(strTip);
	m_btnAddLoopSeq.SetImage(IDB_ADD_LOOP_SEQ_24, IDB_ADD_LOOP_SEQ_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_ADD_LOOP_SEQ);
	m_btnAddLoopSeq.SetTooltip(strTip);
	//
	m_btnGenerateRemCutLine.SetImage(IDB_REM_CUTLINE_24, IDB_REM_CUTLINE_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_AUTO_REM_CUTLINE_TIP);
	m_btnGenerateRemCutLine.SetTooltip(strTip);
	//
	m_btnSimulateSeq.SetImage(IDB_SIMULATE_SEQ_24, IDB_SIMULATE_SEQ_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_VIEWSEQUENCE);
	m_btnSimulateSeq.SetTooltip(strTip);
    if (LicenseManager::CheckLicense() == LicenseCheckResult_ultimateEdition || LicenseManager::CheckLicense() == LicenseCheckResult_professionalEdition ||
        LicenseManager::CheckLicense() == LicenseCheckResult_stdEdition)
    {
        m_btnGenerateNC.SetImage(IDB_GENERATE_NC_24, IDB_GENERATE_NC_24);
        strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MACROUTER_VIEW_NC);
        m_btnGenerateNC.SetTooltip(strTip);
    }
    else
    {
        m_btnGenerateNC.ShowWindow(SW_HIDE);
    }

	// 设置。
	m_ctrlParamBtn.SetImage(IDB_PARAM_CONFIG_24, IDB_PARAM_CONFIG_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CAM_PARAM);
	m_ctrlParamBtn.SetTooltip(strTip);
	m_ctrlSysSetBtn.SetImage(IDB_SYS_CONFIG_24, IDB_SYS_CONFIG_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OPTIONS);
	m_ctrlSysSetBtn.SetTooltip(strTip);
	m_ctrlMatLibBtn.SetImage(IDB_MATLIB_24, IDB_MATLIB_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MATLIB_MGR);
	m_ctrlMatLibBtn.SetTooltip(strTip);
	/************************************************************************/

	/************************************************************************/
	// about scroll bar.

	// get the original size
	GetWindowRect(m_rcOriginalRect);

	// initial scroll position
	m_nScrollPos = 0; 
	/************************************************************************/

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void SheetComFuncDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nDelta;
	int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(max(nMaxPos/20,5), nMaxPos-m_nScrollPos);
		break;
	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/20,5), m_nScrollPos);
		break;
	case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(max(nMaxPos/10,5), nMaxPos-m_nScrollPos);
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;
	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/10,5), m_nScrollPos);
		break;

	default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT, m_nScrollPos, TRUE);
	ScrollWindow(0, -nDelta);

	CResizableDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void SheetComFuncDlg::OnSize(UINT nType, int cx, int cy) 
{
	m_nCurHeight = cy;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; 
	si.nMin = 0;
	si.nMax = m_rcOriginalRect.Height();
	si.nPage = cy;
	si.nPos = m_nScrollPos;
	SetScrollInfo(SB_VERT, &si, TRUE);

	CResizableDialog::OnSize(nType, cx, cy);
}

void SheetComFuncDlg::OnAddPartFromDB()
{
	m_pSheetView->OnPartPmtAddPart();
}

void SheetComFuncDlg::OnAddPartFromDxf()
{
	m_pSheetView->OnPartPmtAddSglDxf();
}

void SheetComFuncDlg::OnAddPartFromAsmDxf()
{
	m_pSheetView->OnPartPmtAddAsmDxf();
}

void SheetComFuncDlg::OnSelectPart()
{
	m_pSheetView->OnPartPmtSelect();
}

void SheetComFuncDlg::OnCreateGrid()
{
	m_pSheetView->OnPartPmtGrid();
}

void SheetComFuncDlg::OnDeletePart()
{
	m_pSheetView->OnPartPmtDelete();
}

void SheetComFuncDlg::OnMeasureDis()
{
	m_pSheetView->OnMeasureDis();
}

void SheetComFuncDlg::OnAssignCam()
{
	m_pSheetView->OnAssignCutFeature();
}

void SheetComFuncDlg::OnResetCam()
{
	m_pSheetView->OnResetCutFeature();
}

void SheetComFuncDlg::OnCheckIntf()
{
	m_pSheetView->OnCheckInterfere();
}

void SheetComFuncDlg::OnEditLoopTool()
{
	m_pSheetView->OnEditLoopTool();
}

void SheetComFuncDlg::OnLoopLead()
{
	m_pSheetView->OnEditLoopLead();
}

void SheetComFuncDlg::OnStartPt()
{
	m_pSheetView->OnEditLoopStartPt();
}

void SheetComFuncDlg::OnCutDir()
{
	m_pSheetView->OnChangeDirection();
}

void SheetComFuncDlg::OnCutSide()
{
	m_pSheetView->OnSwitchCutSide();
}

void SheetComFuncDlg::OnAutoSeq()
{
	m_pSheetView->OnAutoSequence();
}

void SheetComFuncDlg::OnClearSeq()
{
	m_pSheetView->OnClearAllSequence();
}

void SheetComFuncDlg::OnDeleteSeq()
{
	m_pSheetView->OnRemoveSequence();
}

void SheetComFuncDlg::OnOrderSeq()
{
	m_pSheetView->OnSortSequence();
}

void SheetComFuncDlg::OnAddLoopSeq()
{
	m_pSheetView->OnAddLoopSeq();
}

void SheetComFuncDlg::OnGenerateRemCutLine()
{
	m_pSheetView->OnAutoRemCutline();
}

void SheetComFuncDlg::OnSimulateSeq()
{
	m_pSheetView->OnViewSequence();
}

void SheetComFuncDlg::OnGenerateNC()
{
	m_pSheetView->OnViewNC();
}

void SheetComFuncDlg::OnParamConfig()
{
	m_pSheetView->SetParamConfig();
}

void SheetComFuncDlg::OnSystemConfig()
{
	CLOptionSheet clOptionSheet;
	clOptionSheet.DoModal();
}

void SheetComFuncDlg::OnMatLib()
{
	/************************************************************************/
	// first, make sure one instance of cam config app is running.

	int iProcessID = INVALID_ID;

	// get the exe path of cam config app.
	CString strCAMConfigPath = ProductFolderManager::GetProductPath();
	strCAMConfigPath += _T(KB_CONFIG_EXE_FILE);

	// check whether cam config app is running.
	BOOL bRunning = FALSE;
	IntegerListPtr pProcessIDList = ProcessMgr::IsProcessRun(strCAMConfigPath);
	if (pProcessIDList->size() > 0)
	{
		bRunning = TRUE;
		iProcessID = (int)pProcessIDList->at(0);
	}

	// if no instance of cam config app is running, run it.
	if (!bRunning)
		iProcessID = ProcessMgr::RunProcess(strCAMConfigPath);
	/************************************************************************/

	/************************************************************************/
	// we should send msg to that instance to activate the proper doc.

	// get the main frame of the running app.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	HWND hMainWnd = ProcessMgr::GetProcessMainWnd(iProcessID, str);

	// send msg to the main frame of the running app.
	WndMsg wndMsg;
	wndMsg.iWndMsgType = WNDMSG_ACTIVE_DOC;
	wndMsg.iDocNameRes = IDS_MATLIB_MGR;
	ProcessMgr::SendCopyMsg(hMainWnd, wndMsg);
	/************************************************************************/
}

END_CUTLEADER_NAMESPACE()
