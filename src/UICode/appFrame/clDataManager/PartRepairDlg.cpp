#include "StdAfx.h"
#include "PartRepairDlg.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clUtilityResource.h"
#include "DateHelper.h"
#include "DBConnect.h"

#include "IntegerList.h"
#include "LogMessage.h"
#include "LogMessageList.h"
#include "baseConst.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"
#include "ProductItemWriter.h"

#include "Part.h"
#include "PartCadData.h"
#include "PartCamData.h"
#include "PartItem.h"
#include "PartLoader.h"
#include "PartWriter.h"
#include "clPartResource.h"
#include "PartManager.h"

#include "clDataManagerResource.h"

#include "ParamConfig.h"
#include "MachineItemLoader.h"
#include "clExpertLibResource.h"
#include "ExpertLibItemLoader.h"
#include "ParamConfigLoader.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartRepairDlg, CDialogEx)

PartRepairDlg::PartRepairDlg()
				: CLDialog(PartRepairDlg::IDD)
{
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_REPAIR);
}

PartRepairDlg::~PartRepairDlg(void)
{
}

void PartRepairDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
	DDX_Control(pDX, IDC_LIST_CAMDATA, m_partCamDataGrid);
}

BEGIN_MESSAGE_MAP(PartRepairDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)

	ON_MESSAGE(WM_GET_CELL_EDITOR_DATA, GetCellEditorData)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_CAMDATA, OnEndLabelEdit)

	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

END_MESSAGE_MAP()

BOOL PartRepairDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

void PartRepairDlg::OnDestroy()
{
	// release the item data.
	ReleaseGridItemData();

	CDialogEx::OnDestroy();
}

HBRUSH PartRepairDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL PartRepairDlg::OnInitDialog()
{
	__super::OnInitDialog();

	InitGridStyle();
	InitCAMGridData();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

LRESULT PartRepairDlg::GetCellEditorData(WPARAM wParam, LPARAM lParam)
{
	CellEditorData* pCellEditorData = reinterpret_cast<CellEditorData*>(wParam);
	int iRow = pCellEditorData->iRow;
	int iCol = pCellEditorData->iCol;

	// only can use combo box for "Invalid Data" cell.
	CString strInvalidData;
	strInvalidData = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
	CString strCellText = m_partCamDataGrid.GetItemText(iRow, iCol);
	if (strInvalidData != strCellText)
	{
		pCellEditorData->m_cellEditorType = READEDITBOX;
		return true;
	}

	StringListPtr pOptionList(new vector<CString>);
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	DBConnectPtr pMacLibDBCon = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	if (iCol == 2)
	{
		vector<DataProperty> expLibItemList = ExpertLibItemLoader::GetExpLibItemInfoList(pExpLibDBCon);
		IntegerListPtr pVisibleMacList = MachineItemLoader::GetVisibleMacList(pMacLibDBCon);
		for (unsigned int i = 0; i < expLibItemList.size(); i++)
		{
			DataProperty prop = expLibItemList[i];
			if (pVisibleMacList->Include(prop.GetItem(3).GetValue().llVal))
				pOptionList->push_back((LPCTSTR)prop.GetItem(1).GetValue().bstrVal);
		}
	}
	else if (iCol == 3)
	{
		// when you want to edit scheme, ExpertLibItem column should have been set.
		CString strExpLibItemName = m_partCamDataGrid.GetItemText(iRow, 2);
		if (strInvalidData == strExpLibItemName)
		{
			pCellEditorData->m_cellEditorType = READEDITBOX;
			return true;
		}

		// �õ���ר�ҿ�������С��������á���Ϣ��
		LONGLONG iExpLibItemID = ExpertLibItemLoader::GetExpLibItemIDByName(pExpLibDBCon, strExpLibItemName);
		vector<DataProperty> paramConfigList = ParamConfigLoader::GetParamConfigList(pExpLibDBCon, iExpLibItemID);
		for (unsigned int i = 0; i < paramConfigList.size(); i++)
		{
			DataProperty prop = paramConfigList[i];
			DataPropItem propItem = prop.GetItem(2);
			pOptionList->push_back((LPCTSTR)propItem.GetValue().bstrVal);
		}
	}

	if (pOptionList->size() > 0)
	{
		pCellEditorData->m_cellEditorType = COMBOBOX;
		pCellEditorData->m_optionValList.push_back(strInvalidData);
		for (unsigned int i = 0; i < pOptionList->size(); i++)
		{
			CString strName = pOptionList->at(i);
			pCellEditorData->m_optionValList.push_back(strName);
		}
	}

	return true;
}

void PartRepairDlg::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);

	int item = pDispInfo->item.iItem;
	int subitem = pDispInfo->item.iSubItem;
	CString str = pDispInfo->item.pszText;

	// �õ�ѡ�е�ר�ҿ⡣
	CString strExpLibItemName = m_partCamDataGrid.GetItemText(item, 2);
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iExpLibItemID = ExpertLibItemLoader::GetExpLibItemIDByName(pExpLibDBCon, strExpLibItemName);

	// ���������á��С�
	if (subitem == 3)
	{
		// ѡ�еġ��������á���
		LONGLONG iParamConfigID = ParamConfigLoader::GetConfigIDByName(pExpLibDBCon, iExpLibItemID, str);

		// ��������ĳ�ס��������ݡ��Ƿ��Ѿ�����������������á���
		LONGLONG iTmpCamDataID, iTmpExpLibItemID;
		if (PartManager::HasCamDatasetA(m_iPartID, iParamConfigID, iTmpCamDataID, iTmpExpLibItemID))
		{
			CString strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CONFIG_PARAM_USED);
			MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
			
			CString strExpLibItemName;
			strExpLibItemName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
			m_partCamDataGrid.SetItemText(item, 3, strExpLibItemName);
		}
		else
		{
			// ���µ�ǰ���������ݡ��ġ��������á�
			DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(m_iPartID);
			LONGLONG* pCamDataID = (LONGLONG*)m_partCamDataGrid.GetItemData(item);
			PartWriter::UpdateCamDataConfig(pPartDBCon, *pCamDataID, iParamConfigID);

			// ��������޸����ڡ�
			DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
			ProductItemWriter::UpdateModifyDate(pMainDBCon, m_iPartID, DateHelper::GetCurrentTime());
		}
	}
}

void PartRepairDlg::OnBnClickedOk()
{
	// check whether at least one cam data is valid.
	// notes:
	//   1) if no cam data is valid, cannot close dlg from this button.
	DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(m_iPartID);
	vector<DataProperty> camDataList = PartLoader::GetPartCamDataProp(pPartDBCon);
	if (camDataList.size() == 0)
	{
		CString str, strProductName;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_NOT_REPAIR);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	OnOK();
}

void PartRepairDlg::InitGridStyle()
{
	CString str;

	// Requires one never uses column 0
	m_partCamDataGrid.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol = new CGridColumnTraitText;
	m_partCamDataGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_EXPLIB_ITEM);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_partCamDataGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 150, 0, pNameCol);
	m_partCamDataGrid.AddDynamicCol(2);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARAM_CONFIG);
	CGridColumnTrait* pSchemeCol = new CGridColumnTraitText;
	m_partCamDataGrid.InsertColumnTrait(3, str, LVCFMT_LEFT, 150, 0, pSchemeCol);
	m_partCamDataGrid.AddDynamicCol(3); // decide the cell editor for this column at run-time.

	m_partCamDataGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void PartRepairDlg::InitCAMGridData()
{
	m_partCamDataGrid.DeleteAllItems();

	DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(m_iPartID);

	// ��ʾû�ж�ʧ���������á��Ĺ������ݡ�
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
		lvItem.lParam = (LPARAM)(new LONGLONG(iCamDataID));
		m_partCamDataGrid.InsertItem(&lvItem);

		// num column.
		CString str;
		str.Format(_T("%d"), i+1);
		m_partCamDataGrid.SetItemText(i, 1, str);

		// ר�ҿ⡣
		propItem = prop.GetItem(1);
		v = propItem.GetValue();
		CString strExpLibItemName = v.bstrVal;
		m_partCamDataGrid.SetItemText(i, 2, strExpLibItemName);

		// �������á�
		propItem = prop.GetItem(2);
		v = propItem.GetValue();
		CString strConfigName = v.bstrVal;
		m_partCamDataGrid.SetItemText(i, 3, strConfigName);
	}

	// ��ʾ��ʧ�ˡ��������á��Ĺ������ݡ�
	int iCurCount = m_partCamDataGrid.GetItemCount();
	camDataList = PartLoader::GetInvalidCamData(pPartDBCon);
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
		lvItem.lParam = (LPARAM)(new LONGLONG(iCamDataID));
		m_partCamDataGrid.InsertItem(&lvItem);

		// num column.
		CString str;
		str.Format(_T("%d"), iCurCount+1);
		m_partCamDataGrid.SetItemText(iCurCount+i, 1, str);

		// expLibItem name column
		CString strExpLibItemName;
		strExpLibItemName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_partCamDataGrid.SetItemText(iCurCount+i, 2, strExpLibItemName);

		// ������������ column
		CString strConfigName;
		strConfigName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_INVALID_DATA);
		m_partCamDataGrid.SetItemText(iCurCount+i, 3, strConfigName);
	}
}

void PartRepairDlg::ReleaseGridItemData()
{
	for (int i = 0; i < m_partCamDataGrid.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_partCamDataGrid.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}
}

END_CUTLEADER_NAMESPACE()
