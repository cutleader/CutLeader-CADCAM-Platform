#include "StdAfx.h"
#include "DistanceDetectDlg.h"

#include "baseConst.h"
#include "DataBaseManager.h"
#include "LogMgr.h"
#include "clPartLayoutResource.h"
#include "PartPlacementEditAction.h"

#include "CLOptionInfo.h"
#include "SheetEditParam.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(DistanceDetectDlg, CDialogEx)

DistanceDetectDlg::DistanceDetectDlg(CWnd* pParent)
	: CLDialog(DistanceDetectDlg::IDD, pParent)
{
}

DistanceDetectDlg::~DistanceDetectDlg(void)
{
}

void DistanceDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_COMCUT, m_bComCut);
	DDX_Control(pDX, IDC_EDIT_PART_GAP, m_ctrlPartGap);
	DDX_Control(pDX, IDC_EDIT_SHT_MARGIN, m_ctrlShtMargin);
    DDX_Control(pDX, IDC_BTN_QuitCurrentAction, m_ctrlQuitCurrentActionButton);
}

BOOL DistanceDetectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// 创建该对话框时从系统设置获得数据。
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	m_ctrlPartGap.Init(pSheetEditParam->GetPartDis(), 4, FALSE);
	m_ctrlShtMargin.Init(pSheetEditParam->GetSheetMargin(), 4, FALSE);

	m_bComCut = FALSE;
	UpdateData(FALSE);

    // 按钮提示。
    CString strTips = MultiLanguageMgr::GetInstance()->TranslateString(IDS_QuitCurrentAction);
    m_ctrlQuitCurrentActionButton.SetTooltip(strTips);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void DistanceDetectDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

BEGIN_MESSAGE_MAP(DistanceDetectDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_CHECK_COMCUT, OnCheckComcut)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnApply)
    ON_BN_CLICKED(IDC_BTN_QuitCurrentAction, OnQuitCurrentActionButton)
END_MESSAGE_MAP()

BOOL DistanceDetectDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH DistanceDetectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void DistanceDetectDlg::OnCheckComcut()
{
	UpdateData(TRUE);

	if (m_bComCut)
		((CWnd*)GetDlgItem(IDC_EDIT_PART_GAP))->EnableWindow(FALSE);
	else
		((CWnd*)GetDlgItem(IDC_EDIT_PART_GAP))->EnableWindow(TRUE);
}

void DistanceDetectDlg::OnApply()
{
	UpdateData(TRUE);

	// 更新数据
    PartAttachConfig partAttachConfig = GetPartAttachConfig();
	m_pPartPmtMoveBase->InitPartAttachConfig(partAttachConfig);
	m_pPartPmtMoveBase->UpdateBeforeMov();
}

PartAttachConfig DistanceDetectDlg::GetPartAttachConfig()
{
	UpdateData(TRUE);

    PartAttachConfig partAttachConfig(m_bComCut, m_ctrlPartGap.Get_Number(), m_ctrlShtMargin.Get_Number());
    return partAttachConfig;
}

void DistanceDetectDlg::OnQuitCurrentActionButton()
{
    PartPlacementEditAction::SendMessage2EnterPartPlacementOrLoopInstanceSelectAction();

    // 隐藏对话框。
    ShowWindow(SW_HIDE);
}

END_CUTLEADER_NAMESPACE()
