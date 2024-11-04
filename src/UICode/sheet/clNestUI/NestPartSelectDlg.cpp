#include "stdafx.h"
#include "NestPartSelectDlg.h"

#include "baseConst.h"
#include "IntegerList.h"
#include "DateHelper.h"
#include "clUtilityResource.h"
#include "CGridColumnTraitText.h"

#include "clBaseDataResource.h"
#include "ImgData.h"
#include "DataBaseManager.h"
#include "baseConst.h"
#include "DataItem.h"
#include "FolderItem.h"
#include "ProductItemList.h"
#include "ProductFolderDlg.h"
#include "ProductStructProcessor.h"
#include "ProductItemLoader.h"

#include "PatternLoopList.h"
#include "PartCadData.h"
#include "PartLoader.h"
#include "clPartResource.h"

#include "clNestResource.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(NestPartSelectDlg, CDialogEx)

BEGIN_MESSAGE_MAP(NestPartSelectDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_WM_PAINT()

	ON_BN_CLICKED(IDC_CHECK_FOLDER, OnCheckFolderFilter)
	ON_BN_CLICKED(IDC_BTN_FOLDER, OnSelectFilterFolder)
	ON_BN_CLICKED(IDC_CHECK_NAME, OnCheckNameFilter)
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnSearch)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PART, OnPartListchanged)
	ON_BN_CLICKED(IDC_CHECK_SEL_ALL, OnSelectAll)

	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

NestPartSelectDlg::NestPartSelectDlg(CWnd* pParent /*=NULL*/)
	: CLDialog(NestPartSelectDlg::IDD, pParent)
{
	m_bFolderFilter = FALSE;
	m_bNameFilter = FALSE;
	m_bFinishAddPart = TRUE;
	m_bSelectAll = FALSE;
}

NestPartSelectDlg::~NestPartSelectDlg()
{
}

void NestPartSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_FOLDER, m_bFolderFilter);
	DDX_Check(pDX, IDC_CHECK_NAME, m_bNameFilter);
	DDX_Text(pDX, IDC_EDIT_PART_NAME, m_strNameFilter);

	DDX_Control(pDX, IDC_LIST_PART, m_partList);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_PART, m_partPreviewWnd);
	DDX_Check(pDX, IDC_CHECK_SEL_ALL, m_bSelectAll);
}

BOOL NestPartSelectDlg::OnInitDialog()
{
	__super::OnInitDialog();

	((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->SetWindowText(m_pFilterFolder->GetFullPath());
	((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->EnableWindow(FALSE);
	((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(FALSE);

	// 初始化表格样式。
	InitListStyle();

	// 初始化表格数据。
	InitListData();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

HBRUSH NestPartSelectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_PART)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void NestPartSelectDlg::OnDestroy()
{
	// release the resource.
	ReleaseGridData();

	CDialogEx::OnDestroy();
}

void NestPartSelectDlg::OnPaint()
{
	CDialogEx::OnPaint();
	PreviewPart();
}

void NestPartSelectDlg::OnCheckFolderFilter()
{
	UpdateData(TRUE);

	// init the folder filter.
	if (m_bFolderFilter)
		((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->EnableWindow(TRUE);
	else
		((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->EnableWindow(FALSE);
}

void NestPartSelectDlg::OnCheckNameFilter()
{
	UpdateData(TRUE);

	// init the part name filter.
	if (m_bNameFilter)
		((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(TRUE);
	else
		((CWnd*)GetDlgItem(IDC_EDIT_PART_NAME))->EnableWindow(FALSE);
}

void NestPartSelectDlg::OnSelectFilterFolder()
{
	ProductFolderDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_pFilterFolder = dlg.GetSelectFolder();

		// get the path of the folder.
		CString strPath = m_pFilterFolder->GetItemPath();
		strPath += _T("\\");
		strPath += m_pFilterFolder->GetName();

		// put the folder path to the button.
		((CWnd*)GetDlgItem(IDC_BTN_FOLDER))->SetWindowText(strPath);
	}
}

void NestPartSelectDlg::OnSearch()
{
	UpdateData(TRUE);

	/************************************************************************/
	// 根据条件搜索零件。

	// name filter
	m_strNameFilter.Trim();
	if (m_bNameFilter && m_strNameFilter.GetLength() < 1)
		m_bNameFilter = FALSE;

	// search parts using the conditions.
	ProductItemListPtr pProductItemList = ProductStructProcessor::SearchDataItem(m_pFilterFolder, TRUE, DATANODE_PART, 
																				 m_bNameFilter, m_strNameFilter);
	/************************************************************************/

	// 禁止消息。
	m_bFinishAddPart = FALSE;

	// 释放表格中的数据。
	ReleaseGridData();

	// 初始化表格。
	int iItemCount = 0;
	for (unsigned int i = 0; i < pProductItemList->size(); i++)
	{
		ProductItemPtr pProductItem = pProductItemList->at(i);
		DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

		// insert a new row.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = iItemCount;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		lvItem.lParam = (LPARAM)(new LONGLONG(pDataItem->GetDataID()));
		m_partList.InsertItem(&lvItem);

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, iItemCount+1);
		m_partList.SetItemText(iItemCount, 1, strNum);

		// the part name column
		m_partList.SetItemText(iItemCount, 2, pDataItem->GetName());

		// 创建时间。
		m_partList.SetItemText(iItemCount, 3, DateHelper::GetStrFromTimeStamp(pDataItem->GetCreateTime()));

		// 修改时间。
		m_partList.SetItemText(iItemCount, 4, DateHelper::GetStrFromTimeStamp(pDataItem->GetModifyTime()));

		iItemCount++;
	}

	// 打开消息。
	m_bFinishAddPart = TRUE;

	// 选择第一行。
	if (iItemCount > 0)
	{
		m_partList.EnsureVisible(0, FALSE);
		m_partList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_partList.SetFocus();
	}
}

void NestPartSelectDlg::OnPartListchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_bFinishAddPart)
		return;

	// 看看是不是点了checkbox。
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(1)) && (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(2))) ||
		((pNMLV->uOldState & INDEXTOSTATEIMAGEMASK(2)) && (pNMLV->uNewState & INDEXTOSTATEIMAGEMASK(1)))) 
	{
		m_partList.SetItemState(pNMLV->iItem, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}

	// preview
	PreviewPart();

	*pResult = 0;
}

void NestPartSelectDlg::OnSelectAll()
{
	UpdateData(TRUE);

	// 禁止消息。
	m_bFinishAddPart = FALSE;

	for (int i = 0; i < m_partList.GetItemCount(); i++)
	{
		if (m_bSelectAll)
			m_partList.SetCheck(i, TRUE);
		else
			m_partList.SetCheck(i, FALSE);
	}

	// 打开消息。
	m_bFinishAddPart = TRUE;
}

void NestPartSelectDlg::OnBnClickedOk()
{
	CString strProductName, str;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	m_pSelPartIDList.reset(new IntegerList);

	// 遍历零件列表。
	for (int i = 0; i < m_partList.GetItemCount(); i++)
	{
		// 是否选中了这一项。
		if (m_partList.GetCheck(i))
		{
			LONGLONG* pPartID = (LONGLONG*)m_partList.GetItemData(i);

			// 检查选中的零件是否有闭合的外边框。
			DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(*pPartID);
			PartCadDataPtr pPartCadData = PartLoader::LoadPartCadData(pPartDBCon);
			if (pPartCadData->GetPatternLoopList()->GetBoundaryLoop() == INVALID_IDX)
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_NEST_OPEN_BOUNDARY);
				str.Replace(_T("WILLBEREPLACED1"), m_partList.GetItemText(i, 2));
				MessageBox(str, strProductName, MB_ICONWARNING);
				continue;
			}

			m_pSelPartIDList->AddInt(*pPartID);
		}
	}

	// 选中零件时才返回。
	if (m_pSelPartIDList->size() > 0)
		OnOK();
}

void NestPartSelectDlg::InitListStyle()
{
	CString str;

	// place check box at first column.
	CGridColumnTrait* pStatusCol = new CGridColumnTraitText;
	m_partList.InsertColumnTrait(0, _T(""), LVCFMT_LEFT, 30, 0, pStatusCol);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_partList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol1);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTNAME);
	CGridColumnTrait* pNameCo2 = new CGridColumnTraitText;
	m_partList.InsertColumnTrait(2, str, LVCFMT_LEFT, 120, 0, pNameCo2);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CREATE_DATE);
	CGridColumnTrait* pCreateDateCol = new CGridColumnTraitText;
	m_partList.InsertColumnTrait(3, str, LVCFMT_LEFT, 130, 0, pCreateDateCol);

	//
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MODIFY_DATE);
	CGridColumnTrait* pModifyDateCol = new CGridColumnTraitText;
	m_partList.InsertColumnTrait(4, str, LVCFMT_LEFT, 130, 0, pModifyDateCol);

	// set other style.
	m_partList.SetExtendedStyle(LVS_EX_CHECKBOXES | m_partList.GetExtendedStyle());
	m_partList.EnableVisualStyles(TRUE);
}

void NestPartSelectDlg::InitListData()
{
	// 从数据库查到所有零件。
	vector<DataProperty> partInfoList;
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	ProductItemLoader::GetDataPropList(pMainDBCon, partInfoList, DATANODE_PART);

	// 禁止消息。
	m_bFinishAddPart = FALSE;

	// add each part info into "all part" grid.
	for (unsigned int i = 0; i < partInfoList.size(); i++)
	{
		DataProperty prop = partInfoList[i];

		// insert a new row.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		lvItem.lParam = (LPARAM)(new LONGLONG(prop.GetItem(0).GetValue().llVal));
		m_partList.InsertItem(&lvItem);

		// "num" column
		CString strNum;
		strNum.Format(INT_FORMAT, i+1);
		m_partList.SetItemText(i, 1, strNum);

		// the part name column
		DataPropItem propItem = prop.GetItem(1);
		VARIANT v = propItem.GetValue();
		m_partList.SetItemText(i, 2, (LPCTSTR)v.bstrVal);

		// "create date" column.
		propItem = prop.GetItem(3);
		v = propItem.GetValue();
		m_partList.SetItemText(i, 3, DateHelper::GetStrFromTimeStamp(v.llVal));

		// "modify date" column.
		propItem = prop.GetItem(4);
		v = propItem.GetValue();
		m_partList.SetItemText(i, 4, DateHelper::GetStrFromTimeStamp(v.llVal));
	}

	// 打开消息。
	m_bFinishAddPart = TRUE;

	// 选择第一行。
	int iItemCount = m_partList.GetItemCount();
	if (iItemCount > 0)
	{
		m_partList.EnsureVisible(0, FALSE);
		m_partList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_partList.SetFocus();
	}
}

void NestPartSelectDlg::ReleaseGridData()
{
	for (int i = 0; i < m_partList.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_partList.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}

	m_partList.DeleteAllItems();
}

void NestPartSelectDlg::PreviewPart()
{
	// 得到选中项。
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_partList.GetItemCount(); i++)
	{
		if (m_partList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}

	// 得到预览数据。
	ImgDataPtr pImgData;
	pImgData.reset();
	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pPartID = (LONGLONG*)m_partList.GetItemData(iIndex);
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(*pPartID);
		pImgData = PartLoader::LoadPreviewData(pPartDBCon);
	}

	// 预览。
	ImgData::DrawImgData(pImgData, &m_partPreviewWnd);
}

END_CUTLEADER_NAMESPACE()
