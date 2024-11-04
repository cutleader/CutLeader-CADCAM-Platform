#include "stdafx.h"
#include "ItemListPage.h"

#include "DateHelper.h"
#include "clUtilityResource.h"
#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"

#include "baseConst.h"
#include "clUILibResource.h"
#include "ProdNodeData.h"
#include "ProductItemList.h"
#include "DataItem.h"
#include "FolderItem.h"
#include "ProductItemBase.h"
#include "clBaseDataResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const size_t MAX_TIP_TEXT_LENGTH = 1024;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(ItemListPage, CResizableDialog)

ItemListPage::ItemListPage() : CResizableDialog(ItemListPage::IDD)
{
	m_pProductItemList.reset();
}

ItemListPage::~ItemListPage()
{
}

void ItemListPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_ITEM, m_productItemGrid);
}

BEGIN_MESSAGE_MAP(ItemListPage, CResizableDialog)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL ItemListPage::OnInitDialog()
{
	CResizableDialog::OnInitDialog();

	/************************************************************************/
	// layout controls.

	AddAnchor(IDC_STATIC_SELECTED_ITEMS, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDC_LIST_ITEM, TOP_LEFT, BOTTOM_RIGHT);
	/************************************************************************/

	InitGridStyle();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

void ItemListPage::InitGridStyle()
{
	// Requires one never uses column 0
	m_productItemGrid.InsertHiddenLabelColumn();

	CString str;

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol = new CGridColumnTraitText;
	m_productItemGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	// "name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_productItemGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 100, 0, pNameCol);

	// "node datatype"
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NODE_DATATYPE);
	CGridColumnTrait* pNodeTypeCol = new CGridColumnTraitText;
	m_productItemGrid.InsertColumnTrait(3, str, LVCFMT_LEFT, 120, 0, pNodeTypeCol);

	// "create date" column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CREATE_DATE);
	CGridColumnTrait* pCreateDateCol = new CGridColumnTraitText;
	m_productItemGrid.InsertColumnTrait(4, str, LVCFMT_LEFT, 120, 0, pCreateDateCol);

	// "modify date" column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MODIFY_DATE);
	CGridColumnTrait* pModifyDateCol = new CGridColumnTraitText;
	m_productItemGrid.InsertColumnTrait(5, str, LVCFMT_LEFT, 120, 0, pModifyDateCol);

	m_productItemGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

void ItemListPage::DisplayItemList(ProductItemListPtr pProductItemList)
{
	m_pProductItemList = pProductItemList;

	//
	InitGridData();
}

void ItemListPage::InitGridData()
{
	m_productItemGrid.DeleteAllItems();

	int iItem = 0;
	for (unsigned int i = 0; i < m_pProductItemList->size(); i++)
	{
		ProductItemPtr pProductItem = m_pProductItemList->at(i);

		/************************************************************************/		
		// get the product item type.

		CString strNodeType;
		if (pProductItem->IsFolder())
		{
			strNodeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_FOLDER_NODE);
		}
		else
		{
			DataItemPtr pDataItem = boost::dynamic_pointer_cast<DataItem>(pProductItem);

			if (pDataItem->GetDataType() == DATANODE_PART)
				strNodeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_NODE);
			else if (pDataItem->GetDataType() == DATANODE_SHEET)
				strNodeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_NODE);
			else if (pDataItem->GetDataType() == DATANODE_NESTJOB)
				strNodeType = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NESTJOB_NODE);
		}
		/************************************************************************/

		/************************************************************************/
		// insert a row.

		CString strNum;
		strNum.Format(_T("%d"), iItem+1);
		m_productItemGrid.InsertItem(iItem, strNum);
		m_productItemGrid.SetItemText(iItem, 1, strNum);
		m_productItemGrid.SetItemText(iItem, 2, pProductItem->GetName());

		// "node data type" column.
		m_productItemGrid.SetItemText(iItem, 3, strNodeType);

		// "create date" column.
		CString strTime = DateHelper::GetStrFromTimeStamp(pProductItem->GetCreateTime());
		m_productItemGrid.SetItemText(iItem, 4, strTime);

		// "modify date" column.
		strTime = DateHelper::GetStrFromTimeStamp(pProductItem->GetModifyTime());
		m_productItemGrid.SetItemText(iItem, 5, strTime);

		iItem++;
		/************************************************************************/
	}
}

END_CUTLEADER_NAMESPACE()
