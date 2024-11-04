#include "StdAfx.h"
#include "PartGridGapDlg.h"

#include "baseConst.h"
#include "DataBaseManager.h"

#include "clPartLayoutResource.h"

#include "CLOptionInfo.h"
#include "SheetEditParam.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartGridGapDlg, CDialogEx)

PartGridGapDlg::PartGridGapDlg(CWnd* pParent /*=NULL*/)
	: CLDialog(PartGridGapDlg::IDD, pParent)
{
}

PartGridGapDlg::~PartGridGapDlg(void)
{
}

void PartGridGapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_COMCUT, m_bComCut);
	DDX_Control(pDX, IDC_EDIT_PART_GAP, m_ctrlPartDis);
}

BOOL PartGridGapDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// Áã¼þ¾àÀë¡£
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	m_ctrlPartDis.Init(pSheetEditParam->GetPartDis(), 4, FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BEGIN_MESSAGE_MAP(PartGridGapDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_CHECK_COMCUT, OnCheckComcut)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL PartGridGapDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH PartGridGapDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void PartGridGapDlg::OnCheckComcut()
{
	UpdateData(TRUE);

	if (m_bComCut)
		((CWnd*)GetDlgItem(IDC_EDIT_PART_GAP))->EnableWindow(FALSE);
	else
		((CWnd*)GetDlgItem(IDC_EDIT_PART_GAP))->EnableWindow(TRUE);
}

void PartGridGapDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (!m_bComCut)
	{
		m_dPartDis = m_ctrlPartDis.Get_Number();
	}

	OnOK();
}

double PartGridGapDlg::GetPartDis(BOOL& bEnableComcut)
{ 
    bEnableComcut = m_bComCut;
    return m_dPartDis;
}

END_CUTLEADER_NAMESPACE()
