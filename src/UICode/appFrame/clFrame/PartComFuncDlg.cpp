#include "stdafx.h"
#include "PartComFuncDlg.h"

#include "baseConst.h"
#include "clGeometryFeatureResource.h"
#include "clCutFeatureResource.h"

#include "clOptionResource.h"
#include "clExpertLibResource.h"
#include "CLOptionSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartComFuncDlg, CResizableDialog)

PartComFuncDlg::PartComFuncDlg() : CLResizableDialog(PartComFuncDlg::IDD)
{
}

PartComFuncDlg::~PartComFuncDlg()
{
}

void PartComFuncDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	// 画图功能。
	DDX_Control(pDX, IDC_BTN_DRAW_LINE, m_ctrlDrawLineBtn);
	DDX_Control(pDX, IDC_BTN_DRAW_POLY_LINE, m_ctrlDrawPolyLineBtn);
	DDX_Control(pDX, IDC_BTN_DRAW_RECT, m_ctrlDrawRectBtn);
	DDX_Control(pDX, IDC_BTN_DRAW_ARC, m_ctrlDrawArcBtn);
	DDX_Control(pDX, IDC_BTN_DRAW_CIRCLE, m_ctrlDrawCircleBtn);

	// 图形编辑功能。
	DDX_Control(pDX, IDC_BTN_SEL_GEOM, m_ctrlSelGeomBtn);
	DDX_Control(pDX, IDC_BTN_DEL_GEOM, m_ctrlDelGeomBtn);

	// 编辑工艺。
	DDX_Control(pDX, IDC_BTN_ASSIGN_CAM, m_ctrlAssignCAMBtn);
	DDX_Control(pDX, IDC_BTN_RESET_CAM, m_ctrlResetCAMBtn);
	DDX_Control(pDX, IDC_BTN_CHECK_INTF, m_ctrlCheckIntfBtn);
	DDX_Control(pDX, IDC_BTN_EDIT_LOOPTOOL, m_ctrlLoopToolBtn);
	DDX_Control(pDX, IDC_BTN_EDIT_LOOP_LEAD, m_ctrlLoopLeadBtn);
	DDX_Control(pDX, IDC_BTN_CHANGE_START_PT, m_ctrlStartPtBtn);
	DDX_Control(pDX, IDC_BTN_CHANGE_CUT_DIR, m_ctrlCutDirBtn);
	DDX_Control(pDX, IDC_BTN_CHANGE_CUT_SIDE, m_ctrlCutSideBtn);

	// 设置。
	DDX_Control(pDX, IDC_BTN_PARAM_CONFIG, m_ctrlParamBtn);
	DDX_Control(pDX, IDC_BTN_SYSTEM_CONFIG, m_ctrlSysSetBtn);
}

BEGIN_MESSAGE_MAP(PartComFuncDlg, CResizableDialog)
	ON_WM_VSCROLL()
	ON_WM_SIZE()

	// 画图功能。
	ON_BN_CLICKED(IDC_BTN_DRAW_LINE, OnDrawLine)
	ON_BN_CLICKED(IDC_BTN_DRAW_POLY_LINE, OnDrawPolyLine)
	ON_BN_CLICKED(IDC_BTN_DRAW_RECT, OnDrawRect)
	ON_BN_CLICKED(IDC_BTN_DRAW_ARC, OnDrawArc)
	ON_BN_CLICKED(IDC_BTN_DRAW_CIRCLE, OnDrawCircle)

	// 图形编辑功能。
	ON_BN_CLICKED(IDC_BTN_SEL_GEOM, OnSelGeom)
	ON_BN_CLICKED(IDC_BTN_DEL_GEOM, OnDelGeom)

	// 编辑工艺。
	ON_BN_CLICKED(IDC_BTN_ASSIGN_CAM, OnAssignCam)
	ON_BN_CLICKED(IDC_BTN_RESET_CAM, OnResetCam)
	ON_BN_CLICKED(IDC_BTN_CHECK_INTF, OnCheckIntf)
	ON_BN_CLICKED(IDC_BTN_EDIT_LOOPTOOL, OnEditLoopTool)
	ON_BN_CLICKED(IDC_BTN_EDIT_LOOP_LEAD, OnLoopLead)
	ON_BN_CLICKED(IDC_BTN_CHANGE_START_PT, OnStartPt)
	ON_BN_CLICKED(IDC_BTN_CHANGE_CUT_DIR, OnCutDir)
	ON_BN_CLICKED(IDC_BTN_CHANGE_CUT_SIDE, OnCutSide)

	// 设置。
	ON_BN_CLICKED(IDC_BTN_PARAM_CONFIG, OnParamConfig)
	ON_BN_CLICKED(IDC_BTN_SYSTEM_CONFIG, OnSystemConfig)
END_MESSAGE_MAP()

BOOL PartComFuncDlg::OnInitDialog()
{
	__super::OnInitDialog();

	/************************************************************************/	
	// 按钮图片和提示。

	CString strTip;

	// 画图功能按钮。
	m_ctrlDrawLineBtn.SetImage(IDB_DRAW_LINE_24, IDB_DRAW_LINE_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY_LINE_TIP);
	m_ctrlDrawLineBtn.SetTooltip(strTip);
	m_ctrlDrawPolyLineBtn.SetImage(IDB_DRAW_POLYLINE_24, IDB_DRAW_POLYLINE_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY_MULTILINE_TIP);
	m_ctrlDrawPolyLineBtn.SetTooltip(strTip);
	m_ctrlDrawRectBtn.SetImage(IDB_DRAW_RECT_24, IDB_DRAW_RECT_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY_RECTHOLE_TIP);
	m_ctrlDrawRectBtn.SetTooltip(strTip);
	m_ctrlDrawArcBtn.SetImage(IDB_DRAW_ARC_24, IDB_DRAW_ARC_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY_ARC_TIP);
	m_ctrlDrawArcBtn.SetTooltip(strTip);
	m_ctrlDrawCircleBtn.SetImage(IDB_DRAW_CIRCLE_24, IDB_DRAW_CIRCLE_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY_ROUNDHOLE_TIP);
	m_ctrlDrawCircleBtn.SetTooltip(strTip);

	// 图形编辑功能。
	m_ctrlSelGeomBtn.SetImage(IDB_SEL_GEOM_24, IDB_SEL_GEOM_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SELECT_GEOM);
	m_ctrlSelGeomBtn.SetTooltip(strTip);
	m_ctrlDelGeomBtn.SetImage(IDB_DEL_GEOM_24, IDB_DEL_GEOM_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DELETE_GEOM_TIP);
	m_ctrlDelGeomBtn.SetTooltip(strTip);

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

	// 设置。
	m_ctrlParamBtn.SetImage(IDB_PARAM_CONFIG_24, IDB_PARAM_CONFIG_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CAM_PARAM);
	m_ctrlParamBtn.SetTooltip(strTip);
	m_ctrlSysSetBtn.SetImage(IDB_SYS_CONFIG_24, IDB_SYS_CONFIG_24);
	strTip = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OPTIONS);
	m_ctrlSysSetBtn.SetTooltip(strTip);
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

void PartComFuncDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void PartComFuncDlg::OnSize(UINT nType, int cx, int cy) 
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

void PartComFuncDlg::OnDrawLine()
{
	m_pPartView->OnDrawStandardline();
}

void PartComFuncDlg::OnDrawPolyLine()
{
	m_pPartView->OnDrawMultiplelines();
}

void PartComFuncDlg::OnDrawRect()
{
	m_pPartView->OnDrawRectangleHole();
}

void PartComFuncDlg::OnDrawArc()
{
	m_pPartView->OnDrawArc3pointsarc();
}

void PartComFuncDlg::OnDrawCircle()
{
	m_pPartView->OnDrawRoundHole();
}

void PartComFuncDlg::OnSelGeom()
{
	m_pPartView->OnSelectPattern();
}

void PartComFuncDlg::OnDelGeom()
{
	m_pPartView->OnDeletePattern();
}

void PartComFuncDlg::OnAssignCam()
{
	m_pPartView->OnAssignCutFeature();
}

void PartComFuncDlg::OnResetCam()
{
	m_pPartView->OnResetCutFeature();
}

void PartComFuncDlg::OnCheckIntf()
{
	m_pPartView->OnCheckInterfere();
}

void PartComFuncDlg::OnEditLoopTool()
{
	m_pPartView->OnEditLoopTool();
}

void PartComFuncDlg::OnLoopLead()
{
	m_pPartView->OnEditLoopLead();
}

void PartComFuncDlg::OnStartPt()
{
	m_pPartView->OnEditLoopStartPt();
}

void PartComFuncDlg::OnCutDir()
{
	m_pPartView->OnChangeDirection();
}

void PartComFuncDlg::OnCutSide()
{
	m_pPartView->OnSwitchCutSide();
}

void PartComFuncDlg::OnParamConfig()
{
	m_pPartView->SetParamConfig();
}

void PartComFuncDlg::OnSystemConfig()
{
	CLOptionSheet clOptionSheet;
	clOptionSheet.DoModal();
}

END_CUTLEADER_NAMESPACE()
