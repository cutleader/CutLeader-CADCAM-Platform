#include "stdafx.h"
#include "LineEntityPropDlg.h"

#include "baseConst.h"
#include "baseConst.h"
#include "LineTplEntity.h"
#include "clReportResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(LineEntityPropDlg, CDialogEx)

LineEntityPropDlg::LineEntityPropDlg()
					: CLDialog(LineEntityPropDlg::IDD)
{
}

LineEntityPropDlg::~LineEntityPropDlg(void)
{
}

void LineEntityPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Control(pDX, IDC_EDIT_THICKNESS, m_iLineThickness);
	DDX_Control(pDX, IDC_COMBO_STYLE, m_ctrlLineStyle);
	DDX_Control(pDX, IDC_STATIC_COLOR, m_ctrlLineColor);
}

BOOL LineEntityPropDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	/************************************************************************/
	// init some style.

	CString text;
	text = MultiLanguageMgr::GetInstance()->TranslateString( IDS_LINE_STYLE_1 );
	m_ctrlLineStyle.SetItemData( m_ctrlLineStyle.AddString( text ), PS_SOLID );
	text = MultiLanguageMgr::GetInstance()->TranslateString( IDS_LINE_STYLE_2 );
	m_ctrlLineStyle.SetItemData( m_ctrlLineStyle.AddString( text ), PS_DASH );
	text = MultiLanguageMgr::GetInstance()->TranslateString( IDS_LINE_STYLE_3 );
	m_ctrlLineStyle.SetItemData( m_ctrlLineStyle.AddString( text ), PS_DOT );
	/************************************************************************/

	/************************************************************************/
	// init the dialog content.

	m_strName = m_pLineTplEntity->GetName();
	m_iLineThickness.Init(m_pLineTplEntity->GetLineThickness(), 0, false);
	m_ctrlLineColor.SetColor( m_pLineTplEntity->GetLineColor());
	for( int t = 0 ; t < m_ctrlLineStyle.GetCount() ; t++ )
		if( m_ctrlLineStyle.GetItemData( t ) == static_cast< DWORD >( m_pLineTplEntity->GetLineStyle() ) )
			m_ctrlLineStyle.SetCurSel( t );
	/************************************************************************/

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

BEGIN_MESSAGE_MAP(LineEntityPropDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_COLOR, OnLineColor)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)

END_MESSAGE_MAP()

void LineEntityPropDlg::OnLineColor()
{
	CColorDialog dlg( m_pLineTplEntity->GetLineColor() );
	if( dlg.DoModal() == IDOK )
	{
		m_ctrlLineColor.SetColor( dlg.GetColor() );
	}
}

void LineEntityPropDlg::OnOK() 
{
	UpdateData(TRUE);

	m_pLineTplEntity->SetName(m_strName);
	m_pLineTplEntity->SetLineThickness((int)m_iLineThickness.Get_Number());
	LINE_STYLE_TYPE lineStyle = (LINE_STYLE_TYPE)m_ctrlLineStyle.GetItemData( m_ctrlLineStyle.GetCurSel() );
	m_pLineTplEntity->SetLineStyle(lineStyle);
	m_pLineTplEntity->SetLineColor(m_ctrlLineColor.GetColor());

	CDialogEx::OnOK();
}

void LineEntityPropDlg::OnCancel() 
{
	CDialogEx::OnCancel();
}

END_CUTLEADER_NAMESPACE()
