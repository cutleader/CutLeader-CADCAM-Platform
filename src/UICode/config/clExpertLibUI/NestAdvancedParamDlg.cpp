#include "StdAfx.h"
#include "NestAdvancedParamDlg.h"

#include "clExpertLibResource.h"
#include "NestConfigItem.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(NestAdvancedParamDlg, CDialogEx)

NestAdvancedParamDlg::NestAdvancedParamDlg(void)
		: CLDialog(NestAdvancedParamDlg::IDD)
{
}

NestAdvancedParamDlg::NestAdvancedParamDlg(NestConfigItemPtr pNestConfigItem)
		: CLDialog(NestAdvancedParamDlg::IDD)
{
	m_pNestConfigItem = pNestConfigItem;
}

NestAdvancedParamDlg::~NestAdvancedParamDlg(void)
{
}

void NestAdvancedParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_GEN_GRID, m_bCreateGrid);
	DDX_Check(pDX, IDC_CHECK_GRID_NEST, m_bOnlyUseGridNest);
	DDX_Control(pDX, IDC_EDIT_ROT_STEP, m_dPartRotStep);
	DDX_Control(pDX, IDC_EDIT_ALLOW_TIME, m_ctrlAllowedNestTime);
	DDX_Control(pDX, IDC_EDIT_EVAL_FACTOR, m_ctrlEvaluateFactor);
}

BEGIN_MESSAGE_MAP(NestAdvancedParamDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL NestAdvancedParamDlg::OnInitDialog()
{
	__super::OnInitDialog();

	m_bOnlyUseGridNest = m_pNestConfigItem->OnlyUseGridNest();
	m_bCreateGrid = m_pNestConfigItem->IsCreateGrid();
	m_dPartRotStep.Init(m_pNestConfigItem->GetPartRotStep(), 4, FALSE);
	m_ctrlAllowedNestTime.Init(m_pNestConfigItem->GetAllowedTime(), 0, FALSE);
	m_ctrlEvaluateFactor.Init(m_pNestConfigItem->GetEvalFactor(), 0, FALSE);

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BOOL NestAdvancedParamDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(RGB(239, 247, 253));
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH NestAdvancedParamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	pDC->SetBkColor(RGB(239, 247, 253));

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(RGB(239, 247, 253));

	return (HBRUSH) m_brush;
}

void NestAdvancedParamDlg::OnBnClickedOk()
{
	UpdateData(TRUE);

	if (m_bOnlyUseGridNest != m_pNestConfigItem->OnlyUseGridNest())
	{
		m_pNestConfigItem->OnlyUseGridNest(m_bOnlyUseGridNest);
		m_pNestConfigItem->SetModified(TRUE);
	}
	if (m_bCreateGrid != m_pNestConfigItem->IsCreateGrid())
	{
		m_pNestConfigItem->IsCreateGrid(m_bCreateGrid);
		m_pNestConfigItem->SetModified(TRUE);
	}

	if (fabs(m_dPartRotStep.Get_Number() - m_pNestConfigItem->GetPartRotStep()) > GEOM_TOLERANCE)
	{
		m_pNestConfigItem->SetPartRotStep(m_dPartRotStep.Get_Number());
		m_pNestConfigItem->SetModified(TRUE);
	}
	if ((int)m_ctrlAllowedNestTime.Get_Number() != m_pNestConfigItem->GetAllowedTime())
	{
		m_pNestConfigItem->SetAllowedTime((int)m_ctrlAllowedNestTime.Get_Number());
		m_pNestConfigItem->SetModified(TRUE);
	}
	if ((int)m_ctrlEvaluateFactor.Get_Number() != m_pNestConfigItem->GetEvalFactor())
	{
		m_pNestConfigItem->SetEvalFactor((int)m_ctrlEvaluateFactor.Get_Number());
		m_pNestConfigItem->SetModified(TRUE);
	}

	OnOK();
}

END_CUTLEADER_NAMESPACE()