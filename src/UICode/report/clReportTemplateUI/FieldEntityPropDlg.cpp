#include "stdafx.h"
#include "FieldEntityPropDlg.h"

#include "baseConst.h"
#include "UnitHelper.h"
#include "ExtPropType.h"
#include "ExtPropTypeList.h"

#include "FontParam.h"
#include "FieldTplEntity.h"
#include "BindItem.h"
#include "BindItemList.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(FieldEntityPropDlg, CDialogEx)

FieldEntityPropDlg::FieldEntityPropDlg()
					: CLDialog(FieldEntityPropDlg::IDD)
{
}

FieldEntityPropDlg::~FieldEntityPropDlg(void)
{
}

void FieldEntityPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_FIELD_NAME, m_strName);
	DDX_Radio(pDX, IDC_RADIO_LEFT, m_iJustification);

	DDX_Radio(pDX, IDC_RADIO_DATA_PROP, m_iDataBind);
	DDX_Control(pDX, IDC_COMBO_BIND_TYPE, m_ctrlBindType);
	DDX_Control(pDX, IDC_COMBO_EXT_PROP, m_ctrlExtProp);
}

BOOL FieldEntityPropDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	m_strName = m_pFieldTplEntity->GetName();

	// text justification type.
	if (m_pFieldTplEntity->GetTextJustType() == TEXT_JUST_LEFT)
		m_iJustification = 0;
	else if (m_pFieldTplEntity->GetTextJustType() == TEXT_JUST_CENTER)
		m_iJustification = 1;
	else if (m_pFieldTplEntity->GetTextJustType() == TEXT_JUST_RIGHT)
		m_iJustification = 2;

	/************************************************************************/
	// 数据绑定。

	// 绑定的哪一种数据。
	if (m_pFieldTplEntity->GetExtPropName() == _T(""))
		m_iDataBind = 0;
	else
		m_iDataBind = 1;

	// init the data bind info.
	int iIndex = 0;
	m_ctrlBindType.AddString(_T(""));
	m_ctrlBindType.SetItemData(0, (DWORD_PTR)BIND_ITEM_NOT_INIT);
	for (unsigned int i = 0; i < m_pAvailBindItemList->size(); i++)
	{
		BindItemPtr pBindItem = m_pAvailBindItemList->at(i);
		m_ctrlBindType.AddString(pBindItem->GetBindItemName());
		m_ctrlBindType.SetItemData(i+1, (DWORD_PTR)pBindItem->GetBindItemType());

		if (pBindItem->GetBindItemType() == m_pFieldTplEntity->GetBindTargetType())
			iIndex = i + 1;
	}
	if (m_iDataBind == 0)
		m_ctrlBindType.SetCurSel(iIndex);
	else
		m_ctrlBindType.SetCurSel(0);

	// 扩展属性。
	int iExtPropIndex = 0;
	m_ctrlExtProp.AddString(_T(""));
	for (unsigned int i = 0; i < m_pExtPropTypes->size(); i++)
	{
		ExtPropTypePtr pExtPropType = m_pExtPropTypes->at(i);
		m_ctrlExtProp.AddString(pExtPropType->GetName());
		if (pExtPropType->GetName() == m_pFieldTplEntity->GetExtPropName())
			iExtPropIndex = i + 1;
	}
	if (m_iDataBind == 1)
		m_ctrlExtProp.SetCurSel(iExtPropIndex);
	else
		m_ctrlExtProp.SetCurSel(0);
	/************************************************************************/

	// the font info.
	FontParamPtr pFontParam = m_pFieldTplEntity->GetFontParam();
	m_logicFont = pFontParam->GetLogicFont();
	m_fontColor = pFontParam->GetFontColor();

	UpdateData(FALSE);
	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

BEGIN_MESSAGE_MAP(FieldEntityPropDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_FONT, OnFont)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

BOOL FieldEntityPropDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH FieldEntityPropDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void FieldEntityPropDlg::OnFont() 
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

void FieldEntityPropDlg::OnOK() 
{
	UpdateData(TRUE);

	// check name.
	if (m_strName != m_pFieldTplEntity->GetName())
	{
		m_pFieldTplEntity->SetName(m_strName);
		m_pFieldTplEntity->SetModified(TRUE);
	}

	// check text justification style.
	if (m_iJustification != m_pFieldTplEntity->GetTextJustType())
	{
		m_pFieldTplEntity->SetTextJustType((TEXT_JUST_TYPE)m_iJustification);
		m_pFieldTplEntity->SetModified(TRUE);
	}

	// check data binding.
	if (m_iDataBind == 0)
	{
		DWORD_PTR iData = m_ctrlBindType.GetItemData(m_ctrlBindType.GetCurSel());
		if ((BIND_ITEM_TYPE)iData != m_pFieldTplEntity->GetBindTargetType())
		{
			m_pFieldTplEntity->SetBindTargetType((BIND_ITEM_TYPE)iData);
			m_pFieldTplEntity->SetModified(TRUE);
		}
	}
	else
	{
		CString strExtProp;
		m_ctrlExtProp.GetLBText(m_ctrlExtProp.GetCurSel(), strExtProp);
		if (strExtProp != m_pFieldTplEntity->GetExtPropName())
		{
			m_pFieldTplEntity->SetExtPropName(strExtProp);
			m_pFieldTplEntity->SetModified(TRUE);
		}
	}

	// check font param.
	FontParamPtr pFontParam = m_pFieldTplEntity->GetFontParam();
	if (!pFontParam->FitLogicFont(m_logicFont, m_fontColor))
	{
		pFontParam->UpdateFromLogicFont(m_logicFont, m_fontColor);
		m_pFieldTplEntity->SetModified(TRUE);
	}

	CDialogEx::OnOK();
}

void FieldEntityPropDlg::OnCancel() 
{
	CDialogEx::OnCancel();
}

void FieldEntityPropDlg::Init(FieldTplEntityPtr pFieldTplEntity, BindItemListPtr pAvailBindItemList, ExtPropTypeListPtr pExtPropTypes)
{ 
	m_pFieldTplEntity = pFieldTplEntity;
	m_pAvailBindItemList = pAvailBindItemList;
	m_pExtPropTypes = pExtPropTypes;
}

END_CUTLEADER_NAMESPACE()
