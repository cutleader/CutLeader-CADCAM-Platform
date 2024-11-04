#include "stdafx.h"
#include "PartAddDlg.h"

#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "DateHelper.h"
#include "clUtilityResource.h"

#include "baseConst.h"
#include "clBaseDataResource.h"
#include "clUILibResource.h"
#include "Rect2D.h"
#include "LineArc2DList.h"
#include "ProductFolderManager.h"
#include "DataBaseManager.h"
#include "ImgData.h"

#include "ProductFolderDlg.h"
#include "FolderItem.h"
#include "ProductStructProcessor.h"
#include "ProductItemList.h"
#include "ProductItemLoader.h"
#include "SearchPartDlg.h"

#include "clPartResource.h"
#include "PartManager.h"
#include "PartLoader.h"

#include "clPartLayoutResource.h"
#include "PartLayoutEditor.h"

#include "ParamConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartAddDlg, CResizableDialog)

PartAddDlg::PartAddDlg() : CLResizableDialog(PartAddDlg::IDD)
{
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PART_DB_CLICK_ADD);
	m_bFinishAddPart = TRUE;
}

PartAddDlg::~PartAddDlg()
{
}

void PartAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
	DDX_Control(pDX, IDC_PART_LIST, m_partList);
	DDX_Control(pDX, IDC_STATIC_PART_PREVIEW, m_previewWnd);
}

BEGIN_MESSAGE_MAP(PartAddDlg, CResizableDialog)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_SIZE()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PART_LIST, OnPartListchanged)
	ON_NOTIFY(NM_DBLCLK, IDC_PART_LIST, OnDblclkList)

	ON_BN_CLICKED(IDC_BTN_SEARCH, OnSearchPart)
	ON_BN_CLICKED(IDC_BTN_ADD, OnAddToSheet)
END_MESSAGE_MAP()

BOOL PartAddDlg::OnInitDialog()
{
	__super::OnInitDialog();

	/************************************************************************/
	// about scroll bar.

	// get the original size
	GetWindowRect(m_rcOriginalRect);

	// initial scroll position
	m_nScrollPos = 0; 
	/************************************************************************/

	// init the style of the list ctrl
	InitListStyle();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void PartAddDlg::OnDestroy()
{
	// release the item data.
	ReleaseGridData();

	CResizableDialog::OnDestroy();
}

HBRUSH PartAddDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PART_PREVIEW)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void PartAddDlg::OnPaint()
{
	CDialogEx::OnPaint();

	PreviewPart();
}

void PartAddDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nDelta;
	int nMaxPos = m_rcOriginalRect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;

		nDelta = min(max(nMaxPos/20,5), nMaxPos-m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/20,5), m_nScrollPos);
		break;
	case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(max(nMaxPos/10,5), nMaxPos-m_nScrollPos);
		break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/10,5), m_nScrollPos);
		break;

	default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT, m_nScrollPos, TRUE);
	ScrollWindow(0, -nDelta);

	PreviewPart();

	CResizableDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void PartAddDlg::OnSize(UINT nType, int cx, int cy) 
{
	m_nCurHeight = cy;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; 
	si.nMin = 0;
	si.nMax = m_rcOriginalRect.Height();
	si.nPage = cy;
	si.nPos = m_nScrollPos;
	SetScrollInfo(SB_VERT, &si, TRUE);

	CResizableDialog::OnSize(nType, cx, cy);
}

void PartAddDlg::OnSearchPart()
{
	SearchPartDlg dlg;
	dlg.Init(m_pWorkspace);
	if (dlg.DoModal() == IDOK)
	{
		ProductItemListPtr pProductItemList = dlg.GetResult();
		UpdatePartGrid(pProductItemList);
	}
}

void PartAddDlg::OnAddToSheet()
{
	// �õ�ѡ���
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_partList.GetItemCount(); i++)
	{
		if (m_partList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}
	if (iIndex == INVALID_IDX)
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_SELECTPRTPMT);
		MessageBox(str, strProductName, MB_ICONWARNING);
		return;
	}

	// ����������ġ�
	LONGLONG* pPartID = (LONGLONG*)m_partList.GetItemData(iIndex);
	m_pPartLayoutEditor->AddPartPmt(*pPartID, m_pParamConfig->GetID());
}

void PartAddDlg::OnPartListchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// preview
	PreviewPart();

	*pResult = 0;
}

void PartAddDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem == -1)
		return;

	// ����������ġ�
	LONGLONG* pPartID = (LONGLONG*)m_partList.GetItemData(pNMListView->iItem);
	m_pPartLayoutEditor->AddPartPmt(*pPartID, m_pParamConfig->GetID());
}

void PartAddDlg::DisplayPartList(PartLayoutEditorPtr pPartLayoutEditor, ParamConfigPtr pParamConfig, FolderNodePtr pWorkspace)
{
	m_pParamConfig = pParamConfig;
	m_pPartLayoutEditor = pPartLayoutEditor;
	m_pWorkspace = pWorkspace;

	// ��ʾ���������
	ProductItemListPtr pProductItemList = ProductStructProcessor::SearchDataItem(m_pWorkspace, TRUE, DATANODE_PART, FALSE, _T(""));
	UpdatePartGrid(pProductItemList);
}

void PartAddDlg::InitListStyle()
{
	CString str;

	// Requires one never uses column 0
	m_partList.InsertHiddenLabelColumn();

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol1 = new CGridColumnTraitText;
	m_partList.InsertColumnTrait(1, str, LVCFMT_LEFT, 35, 0, pNUMCol1);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTNAME);
	CGridColumnTrait* pNameCo2 = new CGridColumnTraitText;
	m_partList.InsertColumnTrait(2, str, LVCFMT_LEFT, 130, 0, pNameCo2);

	m_partList.SetExtendedStyle(m_partList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
}

void PartAddDlg::UpdatePartGrid(ProductItemListPtr pProductItemList)
{
	// ��ֹ��Ϣ��
	m_bFinishAddPart = FALSE;

	// ������
	ReleaseGridData();

	// ��ʼ�����
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

		iItemCount++;
	}

	// ����Ϣ��
	m_bFinishAddPart = TRUE;

	// ѡ���һ�С�
	if (iItemCount > 0)
	{
		m_partList.EnsureVisible(0, FALSE);
		m_partList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_partList.SetFocus();
	}
}

void PartAddDlg::ReleaseGridData()
{
	for (int i = 0; i < m_partList.GetItemCount(); i++)
	{
		DWORD_PTR pItemData = m_partList.GetItemData(i);
		delete (LONGLONG*)pItemData;
	}

	m_partList.DeleteAllItems();
}

void PartAddDlg::PreviewPart()
{
	// �õ�ѡ���
	int iIndex = INVALID_IDX;
	for (int i = 0; i < m_partList.GetItemCount(); i++)
	{
		if (m_partList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
			iIndex = i;
	}

	// �õ�Ԥ�����ݡ�
	ImgDataPtr pImgData;
	pImgData.reset();
	if (iIndex != INVALID_IDX)
	{
		LONGLONG* pPartID = (LONGLONG*)m_partList.GetItemData(iIndex);
		DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(*pPartID);
		pImgData = PartLoader::LoadPreviewData(pPartDBCon);
	}

	// Ԥ����
	ImgData::DrawImgData(pImgData, &m_previewWnd);
}

END_CUTLEADER_NAMESPACE()
