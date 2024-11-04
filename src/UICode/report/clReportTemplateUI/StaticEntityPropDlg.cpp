#include "stdafx.h"
#include "StaticEntityPropDlg.h"

#include "baseConst.h"
#include "UnitHelper.h"

#include "FontParam.h"
#include "StaticTplEntity.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(StaticEntityPropDlg, CDialogEx)

StaticEntityPropDlg::StaticEntityPropDlg()
					: CLDialog(StaticEntityPropDlg::IDD)
{
}

StaticEntityPropDlg::~StaticEntityPropDlg(void)
{
}

void StaticEntityPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_LABEL_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_LABEL_TEXT, m_strContent);
	DDX_Radio(pDX, IDC_RADIO_LEFT, m_iJustification);
}

BOOL StaticEntityPropDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	/************************************************************************/
	// init the dialog content.

	m_strName = m_pStaticTplEntity->GetName();
	m_strContent = m_pStaticTplEntity->GetTitle();

	// text justification type.
	if (m_pStaticTplEntity->GetTextJustType() == TEXT_JUST_LEFT)
		m_iJustification = 0;
	else if (m_pStaticTplEntity->GetTextJustType() == TEXT_JUST_CENTER)
		m_iJustification = 1;
	else if (m_pStaticTplEntity->GetTextJustType() == TEXT_JUST_RIGHT)
		m_iJustification = 2;

	// the font info.
	FontParamPtr pFontParam = m_pStaticTplEntity->GetFontParam();
	m_logicFont = pFontParam->GetLogicFont();
	m_fontColor = pFontParam->GetFontColor();
	/************************************************************************/

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

BEGIN_MESSAGE_MAP(StaticEntityPropDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_FONT, OnFont)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

BOOL StaticEntityPropDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH StaticEntityPropDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void StaticEntityPropDlg::OnFont() 
{
	LOGFONT	logicFont = m_logicFont;
	logicFont.lfHeight = UnitHelper::PointsToPixels(logicFont.lfHeight);

	CFontDialog	dlg(&logicFont);
	dlg.m_cf.rgbColors = m_fontColor;
	dlg.m_cf.Flags |= CF_EFFECTS;
	if( dlg.DoModal() == IDOK )
	{
		dlg.GetCurrentFont(&m_logicFont);
		m_logicFont.lfHeight = dlg.GetSize();
		m_fontColor = dlg.GetColor();
	}
}

void StaticEntityPropDlg::OnOK() 
{
	UpdateData(TRUE);

	// check name.
	if (m_strName != m_pStaticTplEntity->GetName())
	{
		m_pStaticTplEntity->SetName(m_strName);
		m_pStaticTplEntity->SetModified(TRUE);
	}

	// check title.
	if (m_strContent != m_pStaticTplEntity->GetTitle())
	{
		m_pStaticTplEntity->SetTitle(m_strContent);
		m_pStaticTplEntity->SetModified(TRUE);
	}

	// check text justification style.
	if (m_iJustification != m_pStaticTplEntity->GetTextJustType())
	{
		m_pStaticTplEntity->SetTextJustType((TEXT_JUST_TYPE)m_iJustification);
		m_pStaticTplEntity->SetModified(TRUE);
	}

	// check font param.
	FontParamPtr pFontParam = m_pStaticTplEntity->GetFontParam();
	if (!pFontParam->FitLogicFont(m_logicFont, m_fontColor))
	{
		pFontParam->UpdateFromLogicFont(m_logicFont, m_fontColor);
		m_pStaticTplEntity->SetModified(TRUE);
	}

	CDialogEx::OnOK();
}

void StaticEntityPropDlg::OnCancel() 
{
	CDialogEx::OnCancel();
}

END_CUTLEADER_NAMESPACE()
