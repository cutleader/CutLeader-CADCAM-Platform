#include "stdafx.h"
#include "BoxEntityPropDlg.h"

#include "baseConst.h"
#include "baseConst.h"
#include "clReportResource.h"

#include "BoxTplEntity.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(BoxEntityPropDlg, CDialogEx)

BoxEntityPropDlg::BoxEntityPropDlg()
					: CLDialog(BoxEntityPropDlg::IDD)
{
}

BoxEntityPropDlg::~BoxEntityPropDlg(void)
{

}

void BoxEntityPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);

	DDX_Check(pDX, IDC_CHECK_FILL, m_bFill);
	DDX_Control(pDX, IDC_STATIC_FILL_COLOR, m_ctrlFillColor);

	DDX_Check(pDX, IDC_CHECK_BORDER, m_bBorder);
	DDX_Control(pDX, IDC_EDIT_BORDER_THICKNESS, m_iBorderThickness);
	DDX_Control(pDX, IDC_COMBO_BORDER_STYLE, m_ctrlBorderStyle);
	DDX_Control(pDX, IDC_STATIC_BORDER_COLOR, m_ctrlBorderColor);
}

BOOL BoxEntityPropDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	/************************************************************************/
	// init some style.

	CString text;
	text = MultiLanguageMgr::GetInstance()->TranslateString( IDS_LINE_STYLE_1 );
	m_ctrlBorderStyle.SetItemData( m_ctrlBorderStyle.AddString( text ), PS_SOLID );
	text = MultiLanguageMgr::GetInstance()->TranslateString( IDS_LINE_STYLE_2 );
	m_ctrlBorderStyle.SetItemData( m_ctrlBorderStyle.AddString( text ), PS_DASH );
	text = MultiLanguageMgr::GetInstance()->TranslateString( IDS_LINE_STYLE_3 );
	m_ctrlBorderStyle.SetItemData( m_ctrlBorderStyle.AddString( text ), PS_DOT );
	/************************************************************************/

	/************************************************************************/
	// init the dialog content.

	m_strName = m_pBoxTplEntity->GetName();

	// about fill.
	m_bFill = m_pBoxTplEntity->IsFill();
	m_ctrlFillColor.SetColor( m_pBoxTplEntity->GetFillColor());

	// about border.
	m_bBorder = m_pBoxTplEntity->IsShowBorder();
	m_iBorderThickness.Init(m_pBoxTplEntity->GetBorderThickness(), 0, FALSE);
	m_ctrlBorderColor.SetColor( m_pBoxTplEntity->GetBorderColor());
	for( int t = 0 ; t < m_ctrlBorderStyle.GetCount() ; t++ )
		if( m_ctrlBorderStyle.GetItemData( t ) == static_cast< DWORD >( m_pBoxTplEntity->GetBorderStyle() ) )
			m_ctrlBorderStyle.SetCurSel( t );
	/************************************************************************/

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

BEGIN_MESSAGE_MAP(BoxEntityPropDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_FILL_COLOR, OnFillColor)
	ON_BN_CLICKED(IDC_BTN_BORDER_COLOR, OnBorderColor)

	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)

END_MESSAGE_MAP()

BOOL BoxEntityPropDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH BoxEntityPropDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	// here, message edit box will display the bk color same as the dlg.
	if (_tcsicmp(classname, _T("EDIT")) == 0 && pWnd->GetDlgCtrlID() != IDC_EDIT_MSG)
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

void BoxEntityPropDlg::OnFillColor()
{
	CColorDialog dlg( m_pBoxTplEntity->GetFillColor() );
	if( dlg.DoModal() == IDOK )
	{
		m_ctrlFillColor.SetColor( dlg.GetColor() );
	}
}

void BoxEntityPropDlg::OnBorderColor()
{
	CColorDialog dlg( m_pBoxTplEntity->GetBorderColor() );
	if( dlg.DoModal() == IDOK )
	{
		m_ctrlBorderColor.SetColor( dlg.GetColor() );
	}
}

void BoxEntityPropDlg::OnOK() 
{
	UpdateData(TRUE);

	// the picture name.
	m_pBoxTplEntity->SetName(m_strName);

	// for fill.
	m_pBoxTplEntity->IsFill(m_bFill);
	m_pBoxTplEntity->SetFillColor(m_ctrlFillColor.GetColor());

	// for border.
	m_pBoxTplEntity->IsShowBorder(m_bBorder);
	m_pBoxTplEntity->SetBorderThickness((int)m_iBorderThickness.Get_Number());
	LINE_STYLE_TYPE borderLineStyle = (LINE_STYLE_TYPE)m_ctrlBorderStyle.GetItemData( m_ctrlBorderStyle.GetCurSel() );
	m_pBoxTplEntity->SetBorderStyle(borderLineStyle);
	m_pBoxTplEntity->SetBorderColor(m_ctrlBorderColor.GetColor());

	CDialogEx::OnOK();
}

void BoxEntityPropDlg::OnCancel() 
{
	CDialogEx::OnCancel();
}

END_CUTLEADER_NAMESPACE()
