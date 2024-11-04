#include "stdafx.h"
#include "PartInfoPage.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clUtilityResource.h"
#include "DateHelper.h"
#include "clUILibResource.h"
#include "ImgData.h"
#include "LogMessage.h"
#include "LogMessageList.h"
#include "LogMgr.h"
#include "baseConst.h"
#include "LineArc2DList.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"
#include "ProductItemWriter.h"
#include "ExtPropType.h"
#include "ExtPropTypeList.h"
#include "ExtPropVal.h"
#include "ExtPropValList.h"

#include "PatternList.h"
#include "Part.h"
#include "PartCadData.h"
#include "PartCamData.h"
#include "PartItem.h"
#include "PartBuilder.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "PartManager.h"
#include "clPartResource.h"

#include "SheetLoader.h"

#include "clDataManagerResource.h"
#include "PartRepairDlg.h"
#include "DataItemTracker.h"

#include "clExpertLibResource.h"
#include "ParamConfig.h"
#include "CamDataAddDlg.h"
#include "ParamConfigLoader.h"
#include "CLOptionInfo.h"
#include "PartEditParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const size_t MAX_TIP_TEXT_LENGTH = 1024;

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartInfoPage, CResizableDialog)

PartInfoPage::PartInfoPage() : CLResizableDialog(PartInfoPage::IDD)
{
	m_pPart.reset();
}

PartInfoPage::~PartInfoPage()
{
}

void PartInfoPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	// basic info group.
	DDX_Text(pDX, IDC_EDIT_PART_NAME, m_strPartName);
	DDX_Text(pDX, IDC_EDIT_PART_COMMENT, m_strComment);
	DDX_Text(pDX, IDC_EDIT_CREATE_DATE, m_strCreateDate);
	DDX_Text(pDX, IDC_EDIT_MODIFY_DATE, m_strModifyDate);

	DDX_Control(pDX, IDC_LIST_CAM_DATASET, m_partCamDataGrid);

	DDX_Control(pDX, IDC_STATIC_PREVIEWPART, m_previewWnd);
	DDX_Control(pDX, IDC_LIST_SHEET, m_sheetList);
	DDX_Control(pDX, IDC_LIST_EXT_PROP, m_extPropList);
}

BEGIN_MESSAGE_MAP(PartInfoPage, CResizableDialog)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()

	ON_EN_KILLFOCUS(IDC_EDIT_PART_NAME, OnKillPartName)
	ON_EN_KILLFOCUS(IDC_EDIT_PART_COMMENT, OnKillPartComment)

	ON_BN_CLICKED(IDC_BTN_ADD, OnAddCamData)
	ON_BN_CLICKED(IDC_BTN_REPAIR, OnRepairCamData)
	ON_BN_CLICKED(IDC_BTN_DEL, OnDeleteCamData)

	ON_MESSAGE(WM_GET_CELL_EDITOR_DATA, GetCellEditorData)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_EXT_PROP, OnEndLabelEdit)
END_MESSAGE_MAP()

BOOL PartInfoPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	/************************************************************************/
	// layout controls.

	// "basic info" group
	AddAnchor(IDC_STATIC_BASE, TOP_LEFT, TOP_RIGHT);
	// name
	AddAnchor(IDC_EDIT_PART_NAME, TOP_LEFT);
	AddAnchor(IDC_STATIC_NAME, TOP_LEFT);
	// comment
	AddAnchor(IDC_EDIT_PART_COMMENT, TOP_LEFT);
	AddAnchor(IDC_STATIC_COMMENT, TOP_LEFT);
	// date
	AddAnchor(IDC_STATIC_CREATE_DATE, TOP_LEFT);
	AddAnchor(IDC_EDIT_CREATE_DATE, TOP_LEFT);
	AddAnchor(IDC_STATIC_MODIFY_DATE, TOP_LEFT);
	AddAnchor(IDC_EDIT_MODIFY_DATE, TOP_LEFT);

	// "cam data set" group
	AddAnchor(IDC_STATIC_CAMDATA, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_LIST_CAM_DATASET, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_BTN_ADD, TOP_RIGHT);
	AddAnchor(IDC_BTN_REPAIR, TOP_RIGHT);
	AddAnchor(IDC_BTN_DEL, TOP_RIGHT);

	// "preview" group.
	AddAnchor(IDC_STATIC_PREVIEWER, TOP_RIGHT);
	AddAnchor(IDC_STATIC_PREVIEWPART, TOP_RIGHT);

	// "relative sheet" group.
	AddAnchor(IDC_STATIC_SHEET, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_SHEET, TOP_LEFT, BOTTOM_RIGHT);

	// ��չ���ԡ�
	AddAnchor(IDC_STATIC_EXT_PROP, TOP_RIGHT, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_EXT_PROP, TOP_RIGHT, BOTTOM_RIGHT);
	/************************************************************************/

	InitGridStyle();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void PartInfoPage::OnDestroy()
{
	ReleaseGridItemData();
	CResizableDialog::OnDestroy();
}

void PartInfoPage::OnPaint()
{
	CResizableDialog::OnPaint();
	PreviewPart();
	UpdateCtrlStatus();
}

HBRUSH PartInfoPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEWPART)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void PartInfoPage::OnKillPartName()
{
	UpdateData(TRUE);

	if (m_pPart->GetName() != m_strPartName)
	{
		m_pPart->SetName(m_strPartName);
		m_pPart->SetModified(TRUE);
	}
}

void PartInfoPage::OnKillPartComment()
{
	UpdateData(TRUE);

	if (m_pPart->GetComment() != m_strComment)
	{
		m_pPart->SetComment(m_strComment);
		m_pPart->SetModified(TRUE);
	}
}

void PartInfoPage::OnAddCamData()
{
	CString strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	CamDataAddDlg dlg;
	dlg.SetPartID(m_pPart->GetID());
	if (dlg.DoModal() == IDOK)
	{
		LONGLONG iParamConfigID = dlg.iParamConfigID();
		LONGLONG iExpLibItemID = dlg.GetExpLibItemID();

		// �½����������ݡ���
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(m_pPart->GetID());
		PartCadDataPtr pPartCadData = PartLoader::LoadPartCadData(pPartDBCon);
		PartCamDataPtr pNewPartCamData = PartBuilder::BuildPartCamData(pPartCadData, iParamConfigID);

		// save the new cam data.
		m_pPart->SetModifyTime(DateHelper::GetCurrentTime());
		m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pPart->GetModifyTime());
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		ProductItemWriter::UpdateModifyDate(pMainDBCon, m_pPart->GetID(), m_pPart->GetModifyTime());
		PartWriter::SaveCamData(pPartDBCon, pNewPartCamData);

		/************************************************************************/		
		// update the grid.

		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		CString strParamConfig = ParamConfigLoader::GetConfigName(pExpLibDBCon, iParamConfigID);
		CString strExpLibItem = ParamConfigLoader::GetExpLibItemName(pExpLibDBCon, iParamConfigID);

		// NO.
		int iCount = m_partCamDataGrid.GetItemCount();
		CString str;
		str.Format(_T("%d"), iCount+1);
		m_partCamDataGrid.InsertItem(iCount, str);
		m_partCamDataGrid.SetItemText(iCount, 1, str);

		m_partCamDataGrid.SetItemText(iCount, 2, strExpLibItem);
		m_partCamDataGrid.SetItemText(iCount, 3, strParamConfig);

		// set the item ID.
		m_partCamDataGrid.SetItemData(iCount, (DWORD_PTR)(new LONGLONG(pNewPartCamData->GetID())));
		/************************************************************************/

		UpdateData(FALSE);
	}
}

void PartInfoPage::OnRepairCamData()
{
	PartRepairDlg dlg;
	dlg.SetPart(m_pPart->GetID());
	dlg.DoModal();

	InitCAMGridData();
}

void PartInfoPage::OnDeleteCamData()
{
	CString str, strProductName, strTaskName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	strTaskName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TASK_DELETE_CAMDATA);

	// whether select a row.
	int iIndex = m_partCamDataGrid.GetSelectionMark();
	if (iIndex == -1)
		return;

	// check whether user want to delete the last cam data set.
	if (m_partCamDataGrid.GetItemCount() == 1)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_LAST);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DEL_CAMDATA);
	if (MessageBox(str, strProductName, MB_YESNO | MB_ICONWARNING) == IDYES)
	{
		m_pPart->SetModifyTime(DateHelper::GetCurrentTime());
		m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pPart->GetModifyTime());

		// ɾ���������ݡ�
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(m_pPart->GetID());
		LONGLONG* pPartCamDataID = (LONGLONG*)m_partCamDataGrid.GetItemData(iIndex);
		PartWriter::DeleteCamData(pPartDBCon, *pPartCamDataID);
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		ProductItemWriter::UpdateModifyDate(pMainDBCon, m_pPart->GetID(), m_pPart->GetModifyTime());

		// update the grid.
		m_partCamDataGrid.DeleteItem(iIndex);
	}

	UpdateData(FALSE);
}

LRESULT PartInfoPage::GetCellEditorData(WPARAM wParam, LPARAM lParam)
{
	CellEditorData* pCellEditorData = reinterpret_cast<CellEditorData*>(wParam);
	if (pCellEditorData->hwnd == m_extPropList.m_hWnd)
	{
		CString strName = m_extPropList.GetItemText(pCellEditorData->iRow, 1);

		// �õ���չ�������͡�
		ExtPropTypePtr pExtPropType;
		ExtPropValPtr pExtPropVal = m_pPart->GetExtPropValList()->GetItemByName(strName);
		if (pExtPropVal)
		{
			pExtPropType = pExtPropVal->GetExtPropType();
		}
		else
		{
			ExtPropTypeListPtr pExtPropTypes = CLOptionInfo::GetInstance()->GetPartEditParam()->GetExtPropTypeList();
			pExtPropType = pExtPropTypes->GetItemByName(strName);
		}

		// ������ӵı༭�����͡�
		if (pExtPropType->GetDataType() == BASIC_DATA_STRING)
		{
			pCellEditorData->m_cellEditorType = STREDITBOX;
		}
		else if (pExtPropType->GetDataType() == BASIC_DATA_INT)
		{
			pCellEditorData->m_cellEditorType = INTEDITBOX;
		}
		else if (pExtPropType->GetDataType() == BASIC_DATA_FLOAT)
		{
			pCellEditorData->m_cellEditorType = FLOATEDITBOX;
		}
		else if (pExtPropType->GetDataType() == BASIC_DATA_BOOL)
		{
			pCellEditorData->m_cellEditorType = COMBOBOX;

			// prepare the options for combo box.
			CString str1, str2, str3;
			str1 = EMPTY_STRING;
			str2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_YES);
			str3 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NO);

			pCellEditorData->m_optionValList.push_back(str1);
			pCellEditorData->m_optionValList.push_back(str2);
			pCellEditorData->m_optionValList.push_back(str3);
		}
	}

	return true;
}

void PartInfoPage::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	// �õ���չ���ԡ�
	BOOL bModified = FALSE;
	int iItem = pDispInfo->item.iItem;
	CString strName = m_extPropList.GetItemText(iItem, 1);
	CString strVal = pDispInfo->item.pszText;
	ExtPropValPtr pExtPropVal = m_pPart->GetExtPropValList()->GetItemByName(strName);
	if (pExtPropVal)
	{
		// �޸ġ�
		if (pExtPropVal->GetStrValue(TRUE) != strVal)
		{
			_bstr_t bstr = strVal;
			pExtPropVal->SetVal(bstr, TRUE);
			bModified = TRUE;
		}
	}
	else
	{
		// �½�һ������ֵ����
		ExtPropTypeListPtr pExtPropTypes = CLOptionInfo::GetInstance()->GetPartEditParam()->GetExtPropTypeList();
		ExtPropTypePtr pExtPropType = pExtPropTypes->GetItemByName(strName);
		ExtPropValPtr pExtPropVal(new ExtPropVal);
		pExtPropVal->SetExtPropType(pExtPropType);
		_bstr_t bstr = strVal;
		pExtPropVal->SetVal(bstr, TRUE);
		m_pPart->GetExtPropValList()->push_back(pExtPropVal);

		bModified = TRUE;
	}

	// ������չ���ԡ�
	if (bModified)
	{
		// ��������޸�ʱ�䡣
		m_pPart->SetModifyTime(DateHelper::GetCurrentTime());
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		ProductItemWriter::UpdateModifyDate(pMainDBCon, m_pPart->GetID(), m_pPart->GetModifyTime());

		// ������չ���ԡ�
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(m_pPart->GetID());
		pPartDBCon->BeginTransaction();
		PartWriter::DeleteExtProp(pPartDBCon, FALSE);
		PartWriter::SaveExtProp(pPartDBCon, m_pPart->GetExtPropValList(), FALSE);
		pPartDBCon->CommitTransaction();

		// ���½��档
		m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pPart->GetModifyTime());
		UpdateData(FALSE);
	}
}

void PartInfoPage::DisplayPartInfo(PartPtr pPart) 
{
	m_pPart = pPart;

	// init the basic and scheme data.
	m_strPartName = m_pPart->GetName();
	m_strComment = m_pPart->GetComment();
	m_strCreateDate = DateHelper::GetStrFromTimeStamp(m_pPart->GetCreateTime());
	m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pPart->GetModifyTime());

	// init the sheet grid.
	InitCAMGridData();

	// init the sheet grid.
	InitSheetGridData();

	// ��ʾ�������չ���ԡ�
	InitExtPropGridData();

	// preview part.
	PreviewPart();

	UpdateData(FALSE);
}

void PartInfoPage::UpdateCtrlStatus()
{
	// check whether the sheet is opened.
	if (DataItemTracker::GetInstance()->IsDataItemOpened(m_pPart->GetID()))
	{
		((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_EDIT_PART_COMMENT))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_LIST_CAM_DATASET))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_ADD))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_DEL))->EnableWindow(FALSE);
	}
	else
	{
		((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_EDIT_PART_COMMENT))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_LIST_CAM_DATASET))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_BTN_ADD))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_BTN_DEL))->EnableWindow(TRUE);
	}
}

void PartInfoPage::UpdateModifyDate()
{
	m_strModifyDate = DateHelper::GetStrFromTimeStamp(m_pPart->GetModifyTime());
	UpdateData(FALSE);
}

void PartInfoPage::ReleaseGridItemData()
{
	for (int i = 0; i < m_partCamDataGrid.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_partCamDataGrid.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

void PartInfoPage::InitGridStyle()
{
	CString str;

	/************************************************************************/
	// for "m_partCamDataGrid" grid.

	// Requires one never uses column 0
	m_partCamDataGrid.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol = new CGridColumnTraitText;
	m_partCamDataGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	// ר�ҿ�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXPLIB_ITEM);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_partCamDataGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 150, 0, pNameCol);

	// �������á�
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARAM_CONFIG);
	CGridColumnTrait* pSchemeCol = new CGridColumnTraitText;
	m_partCamDataGrid.InsertColumnTrait(3, str, LVCFMT_LEFT, 150, 0, pSchemeCol);

	m_partCamDataGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/

	/************************************************************************/
	// for "m_sheetList" grid.

	// Requires one never uses column 0
	m_sheetList.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	pNUMCol = new CGridColumnTraitText;
	m_sheetList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	// "name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	pNameCol = new CGridColumnTraitText;
	m_sheetList.InsertColumnTrait(2, str, LVCFMT_LEFT, 100, 0, pNameCol);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CREATE_DATE);
	CGridColumnTrait* pCreateDateCol = new CGridColumnTraitText;
	m_sheetList.InsertColumnTrait(3, str, LVCFMT_LEFT, 120, 0, pCreateDateCol);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MODIFY_DATE);
	CGridColumnTrait* pModifyDateCol = new CGridColumnTraitText;
	m_sheetList.InsertColumnTrait(4, str, LVCFMT_LEFT, 120, 0, pModifyDateCol);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMENT);
	CGridColumnTrait* pCommentCol = new CGridColumnTraitText;
	m_sheetList.InsertColumnTrait(5, str, LVCFMT_LEFT, 200, 0, pCommentCol);
	/************************************************************************/

	/************************************************************************/
	// ��ʼ������չ���ԡ��б�

	// Requires one never uses column 0
	m_extPropList.InsertHiddenLabelColumn();

	// "Name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_extPropList.InsertColumnTrait(1, str, LVCFMT_LEFT, 80, 0, pNUMCol1);

	// "Value" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_VALUE);
	CGridColumnTrait* pNameCol1 = new CGridColumnTraitText;
	m_extPropList.InsertColumnTrait(2, str, LVCFMT_LEFT, 80, 0, pNameCol1);

	// we should decide the cell editor for this column at run-time.
	m_extPropList.AddDynamicCol(2);
	/************************************************************************/
}

void PartInfoPage::InitCAMGridData()
{
	// clear the grid.
	ReleaseGridItemData();
	m_partCamDataGrid.DeleteAllItems();

	// ��ʾû�ж�ʧ���������á��ġ������������ݡ���
	DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(m_pPart->GetID());
	vector<DataProperty> camDataList = PartLoader::GetPartCamDataProp(pPartDBCon);
	for (unsigned int i = 0; i < camDataList.size(); i++)
	{
		DataProperty prop = camDataList[i];

		DataPropItem propItem = prop.GetItem(0);
		VARIANT v = propItem.GetValue();
		LONGLONG iCamDataID = v.llVal;

		// insert a new row.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		lvItem.lParam = (DWORD_PTR)(new LONGLONG(iCamDataID));
		m_partCamDataGrid.InsertItem(&lvItem);

		// num column.
		CString str;
		str.Format(_T("%d"), i+1);
		m_partCamDataGrid.SetItemText(i, 1, str);

		// expLibItem name column
		propItem = prop.GetItem(1);
		v = propItem.GetValue();
		CString strExpLibItemName = v.bstrVal;
		m_partCamDataGrid.SetItemText(i, 2, strExpLibItemName);

		// param config name column
		propItem = prop.GetItem(2);
		v = propItem.GetValue();
		CString strConfigName = v.bstrVal;
		m_partCamDataGrid.SetItemText(i, 3, strConfigName);
	}

	// ��ʾ��ʧ���������á��ġ������������ݡ���
	camDataList = PartLoader::GetInvalidCamData(pPartDBCon);
	int iCurCount = m_partCamDataGrid.GetItemCount();
	for (unsigned int i = 0; i < camDataList.size(); i++)
	{
		DataProperty prop = camDataList[i];

		DataPropItem propItem = prop.GetItem(0);
		VARIANT v = propItem.GetValue();
		LONGLONG iCamDataID = v.llVal;

		// insert a new row.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = iCurCount+i;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		lvItem.lParam = (DWORD_PTR)(new LONGLONG(iCamDataID));
		m_partCamDataGrid.InsertItem(&lvItem);

		// num column.
		CString str;
		str.Format(_T("%d"), iCurCount+i+1);
		m_partCamDataGrid.SetItemText(iCurCount+i, 1, str);

		// expLibItem name column
		CString strExpLibItemName;
		strExpLibItemName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_partCamDataGrid.SetItemText(iCurCount+i, 2, strExpLibItemName);

		CString strConfigName;
		strConfigName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_partCamDataGrid.SetItemText(iCurCount+i, 3, strConfigName);
	}

	// remember to update the status of "repair" button
	if (camDataList.size() > 0)
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR))->EnableWindow(TRUE);
	else
		((CWnd*)GetDlgItem(IDC_BTN_REPAIR))->EnableWindow(FALSE);
}

void PartInfoPage::InitExtPropGridData()
{
	m_extPropList.DeleteAllItems();

	// �Ȱ�����е���չ������ʾ������
	ExtPropValListPtr pExtPropVals = m_pPart->GetExtPropValList();
	for (unsigned int i = 0; i < pExtPropVals->size(); i++)
	{
		ExtPropValPtr pExtPropVal = pExtPropVals->at(i);

		m_extPropList.InsertItem(i, _T(""));
		m_extPropList.SetItemText(i, 1, pExtPropVal->GetExtPropType()->GetName());
		m_extPropList.SetItemText(i, 2, pExtPropVal->GetStrValue(TRUE));
	}

	// ��ʾ������û�������չ���ԡ�
	ExtPropTypeListPtr pExtPropTypes = CLOptionInfo::GetInstance()->GetPartEditParam()->GetExtPropTypeList();
	for (unsigned int i = 0; i < pExtPropTypes->size(); i++)
	{
		ExtPropTypePtr pExtPropType = pExtPropTypes->at(i);
		if (!pExtPropVals->GetItemByName(pExtPropType->GetName()))
		{
			m_extPropList.InsertItem(i, _T(""));
			m_extPropList.SetItemText(i, 1, pExtPropType->GetName());
			m_extPropList.SetItemText(i, 2, EMPTY_STRING);
		}
	}
}

void PartInfoPage::PreviewPart()
{
	ImgData::DrawImgData(m_pPart->GetPreviewData(), &m_previewWnd);
}

void PartInfoPage::InitSheetGridData()
{
	m_sheetList.DeleteAllItems();

	// get all sheets which referenced this part.
	vector<DataProperty> sheetInfoList;
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	if (SheetLoader::GetSheetInfoList(pMainDBCon, sheetInfoList, m_pPart->GetID()))
	{
		for (unsigned int i = 0; i < sheetInfoList.size(); i++)
		{
			DataProperty prop = sheetInfoList.at(i);

			/************************************************************************/			
			// insert a row.

			// NO.
			CString str;
			str.Format(_T("%d"), i+1);
			m_sheetList.InsertItem(i, str);
			m_sheetList.SetItemText(i, 1, str);

			// sheet name.
			DataPropItem propItem1 = prop.GetItem(1);
			VARIANT v1 = propItem1.GetValue();
			m_sheetList.SetItemText(i, 2, (LPCTSTR)v1.bstrVal);

			// create date.
			DataPropItem propItem2 = prop.GetItem(2);
			VARIANT v2 = propItem2.GetValue();
			m_sheetList.SetItemText(i, 3, DateHelper::GetStrFromTimeStamp(v2.llVal));

			// modify date.
			DataPropItem propItem3 = prop.GetItem(3);
			VARIANT v3 = propItem3.GetValue();
			m_sheetList.SetItemText(i, 4, DateHelper::GetStrFromTimeStamp(v2.llVal));

			// sheet comment.
			DataPropItem propItem4 = prop.GetItem(4);
			VARIANT v4 = propItem4.GetValue();
			m_sheetList.SetItemText(i, 5, (LPCTSTR)v4.bstrVal);
			/************************************************************************/
		}
	}
}

END_CUTLEADER_NAMESPACE()
