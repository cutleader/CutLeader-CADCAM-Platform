#include "stdafx.h"
#include "PartEditParamPage.h"

#include "baseConst.h"
#include "clUtilityResource.h"
#include "baseConst.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "ExtPropType.h"
#include "ExtPropTypeList.h"

#include "PartEditParam.h"
#include "NewExtPropDlg.h"
#include "clOptionResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const size_t MAX_TIP_TEXT_LENGTH = 1024;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartEditParamPage, CResizableDialog)

PartEditParamPage::PartEditParamPage() : CLResizableDialog(PartEditParamPage::IDD)
{
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_EDIT);
}

PartEditParamPage::~PartEditParamPage()
{
}

void PartEditParamPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
	DDX_Control(pDX, IDC_LIST_EXT_PROP, m_extPropList);
}

BEGIN_MESSAGE_MAP(PartEditParamPage, CResizableDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_BN_CLICKED(IDC_BTN_ADD, AddExtProp)
	ON_BN_CLICKED(IDC_BTN_DEL, DelExtProp)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_EXT_PROP, OnEndLabelEdit)
END_MESSAGE_MAP()

BOOL PartEditParamPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	/************************************************************************/
	// layout controls.

	AddAnchor(IDC_STATIC_INFO, TOP_LEFT);
	AddAnchor(IDC_EDIT_MSG, TOP_LEFT, TOP_RIGHT);

	// 扩展属性。
	AddAnchor(IDC_STATIC_EXT_PROP, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_EXT_PROP, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_BTN_ADD, BOTTOM_RIGHT);
	AddAnchor(IDC_BTN_DEL, BOTTOM_RIGHT);
	/************************************************************************/

	/************************************************************************/
	// 初始化列表风格。

	CString str;

	// Requires one never uses column 0
	m_extPropList.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNumCol = new CGridColumnTraitText;
	m_extPropList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNumCol);

	// "name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_extPropList.InsertColumnTrait(2, str, LVCFMT_LEFT, 90, 0, pNameCol);

	// "data type" column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_DATATYPE);
	CGridColumnTraitCombo* pTypeCol = new CGridColumnTraitCombo;
	pTypeCol->SetStyle(pTypeCol->GetStyle() | CBS_DROPDOWNLIST);
	StringListPtr pList2 = ExtPropType::GetAllDataType();
	for (unsigned int j = 0; j < pList2->size(); j++)
		pTypeCol->AddItem(j, pList2->at(j));
	m_extPropList.InsertColumnTrait(3, str, LVCFMT_LEFT, 130, 0, pTypeCol);

	m_extPropList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BOOL PartEditParamPage::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH PartEditParamPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void PartEditParamPage::AddExtProp()
{
	NewExtPropDlg dlg;
	dlg.Init(m_pPartEditParam->GetExtPropTypeList());
	if (dlg.DoModal() == IDOK)
	{
		ExtPropTypePtr pExtPropType = dlg.GetExtPropType();
		m_pPartEditParam->GetExtPropTypeList()->push_back(pExtPropType);
		m_pPartEditParam->SetModified(TRUE);

		// 添加一行。
		int iRowCount = m_extPropList.GetItemCount();
		CString strNum;
		if ((iRowCount+1) < 10)
			strNum.Format(_T("0%d"), iRowCount+1);
		else
			strNum.Format(_T("%d"), iRowCount+1);
		iRowCount = m_extPropList.InsertItem(iRowCount, _T(""));
		m_extPropList.SetItemText(iRowCount, 1, strNum);
		m_extPropList.SetItemText(iRowCount, 2, pExtPropType->GetName());
		m_extPropList.SetItemText(iRowCount, 3, pExtPropType->GetDataTypeName());

		// 选中最后一行。
		iRowCount = m_extPropList.GetItemCount();
		for (int i = 0; i < iRowCount; i++)
		{
			if (m_extPropList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
				m_extPropList.SetItemState(i, 0, LVIS_SELECTED);
		}
		m_extPropList.EnsureVisible(iRowCount-1, FALSE);
		m_extPropList.SetItemState(iRowCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_extPropList.SetFocus();
	}
}

void PartEditParamPage::DelExtProp()
{
	for (int i = 0; i < m_extPropList.GetItemCount(); i++)
	{
		if (m_extPropList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			// delete the item.
			CString strName = m_extPropList.GetItemText(i, 2);
			m_pPartEditParam->GetExtPropTypeList()->DeleteItemByName(strName);
			m_pPartEditParam->SetModified(TRUE);
			m_extPropList.DeleteItem(i);
			i--;
		}
	}

	// re-index the first column.
	for (int i = 0; i < m_extPropList.GetItemCount(); i++)
	{
		CString strNum;
		if ((i+1) < 10)
			strNum.Format(_T("0%d"), i+1);
		else
			strNum.Format(_T("%d"), i+1);
		m_extPropList.SetItemText(i, 1, strNum);
	}

	// 选中最后一行。
	int iCount = m_extPropList.GetItemCount();
	if (iCount > 0)
	{
		m_extPropList.EnsureVisible(iCount-1, FALSE);
		m_extPropList.SetItemState(iCount-1, LVIS_SELECTED, LVIS_SELECTED);
		m_extPropList.SetFocus();
	}
}

void PartEditParamPage::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	// 得到扩展属性。
	int iItem = pDispInfo->item.iItem;
	CString strName = m_extPropList.GetItemText(iItem, 2);
	ExtPropTypePtr pExtPropType = m_pPartEditParam->GetExtPropTypeList()->GetItemByName(strName);

	// 修改。
	BOOL bModified = FALSE;
	int iSubitem = pDispInfo->item.iSubItem;
	CString str = pDispInfo->item.pszText;
	if (iSubitem == 3) // data type
	{
		if (pExtPropType->GetDataTypeName() != str)
		{
			pExtPropType->SetDataType(str);
			bModified = TRUE;
		}
	}
	if (bModified)
		m_pPartEditParam->SetModified(TRUE);
}

void PartEditParamPage::DisplayPartEditParam(PartEditParamPtr pPartEditParam)
{
	m_pPartEditParam = pPartEditParam;

	// 显示扩展属性。
	int nItem = 0;
	ExtPropTypeListPtr pExtPropTypes = m_pPartEditParam->GetExtPropTypeList();
	m_extPropList.DeleteAllItems();
	for (unsigned int i = 0; i < pExtPropTypes->size(); i++)
	{
		ExtPropTypePtr pExtPropType = pExtPropTypes->at(i);

		// init a row.
		CString strNum;
		if ((i+1) < 10)
			strNum.Format(_T("0%d"), i+1);
		else
			strNum.Format(_T("%d"), i+1);
		nItem = m_extPropList.InsertItem(++nItem, _T(""));
		m_extPropList.SetItemText(nItem, 1, strNum);
		m_extPropList.SetItemText(nItem, 2, pExtPropType->GetName());
		m_extPropList.SetItemText(nItem, 3, pExtPropType->GetDataTypeName());
	}
}

PartEditParamPtr PartEditParamPage::GetPartEditParam()
{
	return m_pPartEditParam;
}

END_CUTLEADER_NAMESPACE()
