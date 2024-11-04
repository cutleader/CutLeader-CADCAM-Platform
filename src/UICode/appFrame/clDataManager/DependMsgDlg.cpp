#include "stdafx.h"
#include "DependMsgDlg.h"

#include "baseConst.h"
#include "clUtilityResource.h"
#include "clUILibResource.h"

#include "DependNode.h"
#include "DependNodeList.h"
#include "DependViewDlg.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(DependMsgDlg, CDialog)

BEGIN_MESSAGE_MAP(DependMsgDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_YES, OnYes)
	ON_BN_CLICKED(IDC_BTN_NO, OnNo)
	ON_BN_CLICKED(IDC_BTN_VIEW_DEPEND, OnViewDetail)

	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

END_MESSAGE_MAP()

DependMsgDlg::DependMsgDlg(CWnd* pParent /*=NULL*/)
								: CLDialog(DependMsgDlg::IDD, pParent)
{
}

DependMsgDlg::~DependMsgDlg()
{
}

void DependMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
}

BOOL DependMsgDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//
	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void DependMsgDlg::OnViewDetail() 
{
	DependViewDlg dlg;
	dlg.SetDependNodeList(m_pDependNodeList);
	dlg.DoModal();
}

void DependMsgDlg::OnYes()
{
	EndDialog(IDC_BTN_YES);
}

void DependMsgDlg::OnNo()
{
	OnOK();
}

void DependMsgDlg::Init(CString strMsg, DependNodeListPtr pDependNodeList)
{
	m_strMsg = strMsg;
	m_pDependNodeList = pDependNodeList;
}

BOOL DependMsgDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH DependMsgDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
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

END_CUTLEADER_NAMESPACE()
