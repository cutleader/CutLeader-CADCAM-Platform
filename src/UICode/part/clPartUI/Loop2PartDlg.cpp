#include "StdAfx.h"
#include "Loop2PartDlg.h"

#include "baseConst.h"
#include "clUtilityResource.h"
#include "CGridColumnTraitText.h"

#include "LineArc2DList.h"
#include "clBaseDataResource.h"

#include "PatternLoopList.h"
#include "PartTopologyItemList.h"
#include "PartBuilder.h"
#include "clPartResource.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(Loop2PartDlg, CDialogEx)

Loop2PartDlg::Loop2PartDlg()
	: CLDialog(Loop2PartDlg::IDD)
{
	m_strMsg_1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LOOP2PART_MSG_1);
	m_strMsg_2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LOOP2PART_MSG_2);
	m_bSelectAll = TRUE;
	m_bRecognizeSamePart = FALSE;
	m_bEnableEvent = FALSE;
}

Loop2PartDlg::~Loop2PartDlg(void)
{
}

void Loop2PartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG_1, m_strMsg_1);
	DDX_Text(pDX, IDC_EDIT_MSG_2, m_strMsg_2);
	DDX_Control(pDX, IDC_LIST_PART, m_partGrid);
	DDX_Check(pDX, IDC_CHECK_SEL_ALL, m_bSelectAll);
	DDX_Check(pDX, IDC_CHECK_RECOGNIZE_SAME_PART, m_bRecognizeSamePart);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_8, m_previewWnd);
}

BEGIN_MESSAGE_MAP(Loop2PartDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PART, OnItemchangedList)
	ON_BN_CLICKED(IDC_CHECK_SEL_ALL, OnSelectAll)
	ON_BN_CLICKED(IDC_CHECK_RECOGNIZE_SAME_PART, OnRecognizeSamePart)

	ON_BN_CLICKED(IDC_BTN_ACCEPT, OnAccept)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnCancel)
END_MESSAGE_MAP()

HBRUSH Loop2PartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_8)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void Loop2PartDlg::OnDestroy()
{
	// 释放视口。
	m_previewWnd.ReleaseViewPort();

	CDialogEx::OnDestroy();
}

void Loop2PartDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_bEnableEvent)
		return;

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	int item = pNMLV->iItem;
	if (item == -1)
		return;

	// 零件ID。
	LONGLONG* pItemData = (LONGLONG*)m_partGrid.GetItemData(item);
	if (!pItemData)
		return;

	// 绘制选中零件。
	for (unsigned int i = 0; i < m_partList.size(); i++)
	{
		PatternLoopListPtr pPatternLoopList = m_partList.at(i).second;
		if (pPatternLoopList->GetID() == *pItemData)
		{
			LineArc2DListPtr pSelGeom2DList = pPatternLoopList->Calculate2DLineArcs();
			m_previewWnd.SetSelGeom2DList(pSelGeom2DList);
			m_previewWnd.Invalidate();
			break;
		}
	}

	*pResult = 0;
}

void Loop2PartDlg::OnSelectAll()
{
	UpdateData(TRUE);

	// 禁止消息。
	m_bEnableEvent = FALSE;

	for (int i = 0; i < m_partGrid.GetItemCount(); i++)
	{
		if (m_bSelectAll)
			m_partGrid.SetCheck(i, TRUE);
		else
			m_partGrid.SetCheck(i, FALSE);
	}

	// 打开消息。
	m_bEnableEvent = TRUE;
}

void Loop2PartDlg::OnRecognizeSamePart()
{
	UpdateData(TRUE);
}

void Loop2PartDlg::OnAccept()
{
	// 遍历零件列表。
	for (int i = 0; i < m_partGrid.GetItemCount(); i++)
	{
		if (m_partGrid.GetCheck(i))
		{
			LONGLONG* pPartID = (LONGLONG*)m_partGrid.GetItemData(i);

			for (unsigned int j = 0; j < m_partList.size(); j++)
			{
				if (m_partList.at(j).second->GetID() == *pPartID)
				{
					CString strPartName = m_partList.at(j).first;
					PatternLoopListPtr pPatternLoopList = m_partList.at(j).second;
					m_selPartList.push_back(pair<CString, PatternLoopListPtr>(strPartName, pPatternLoopList));
					break;
				}
			}
		}
	}

	// 选中零件时才返回。
	if (m_selPartList.size() > 0)
		EndDialog(IDC_BTN_ACCEPT);
	else
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LOOP2PART_SEL_PART);
		MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);
	}
}

void Loop2PartDlg::OnCancel()
{
	EndDialog(IDC_BTN_CANCEL);
}

BOOL Loop2PartDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// 识别出的零件集合。
	vector<PatternLoopListPtr> partList;
	PartTopologyItemListPtr pPartTopItemList = PartBuilder::CalcPartTopStruct(m_pLoopTopItemList.get());
	pPartTopItemList->GetAllPart(partList);
	for (unsigned int i = 0; i < partList.size(); i++)
	{
		PatternLoopListPtr pPatternLoopList = partList.at(i);
		CString strPartName;
		strPartName.Format(_T("%s-%d"), m_strName, i+1);
		m_partList.push_back(pair<CString, PatternLoopListPtr>(strPartName, pPatternLoopList));
	}

	/************************************************************************/
	// 初始化预览窗口。

	// 从所有回路得到的基本图形集合。
	LineArc2DListPtr pGeom2DList(new LineArc2DList);
	for (unsigned int i = 0; i < partList.size(); i++)
	{
		PatternLoopListPtr pPatternLoopList = partList.at(i);
		LineArc2DListPtr pTmpGeom2DList = pPatternLoopList->Calculate2DLineArcs();
		pGeom2DList->insert(pGeom2DList->end(), pTmpGeom2DList->begin(), pTmpGeom2DList->end());
	}

	// 初始化视口。
	Rect2D geomRect = pGeom2DList->GetRectBox();
	m_previewWnd.InitViewPort(geomRect);
	m_previewWnd.SetSolidGeom2DList(pGeom2DList);
	/************************************************************************/

	// 初始化表格。
	InitGrid();

	m_bEnableEvent = TRUE;

	if (m_bDisplay_RecognizeSamePartBtn)
	{
		((CWnd*)GetDlgItem(IDC_CHECK_RECOGNIZE_SAME_PART))->ShowWindow(SW_SHOW);
	}
	else
	{
		((CWnd*)GetDlgItem(IDC_CHECK_RECOGNIZE_SAME_PART))->ShowWindow(SW_HIDE);
	}

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void Loop2PartDlg::Init(LoopTopologyItemListPtr pLoopTopItemList, CString strName, BOOL bDisplay_RecognizeSamePartBtn)
{ 
	m_pLoopTopItemList = pLoopTopItemList;
	m_strName = strName;
	m_bDisplay_RecognizeSamePartBtn = bDisplay_RecognizeSamePartBtn;
}

void Loop2PartDlg::InitGrid()
{
	CString str;

	/************************************************************************/
	// 初始化样式。

	// place check box at first column.
	CGridColumnTrait* pStatusCol = new CGridColumnTraitText;
	m_partGrid.InsertColumnTrait(0, _T(""), LVCFMT_LEFT, 30, 0, pStatusCol);

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol = new CGridColumnTraitText;
	m_partGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 30, 0, pNUMCol);

	// "part name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_partGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 110, 0, pNameCol);

	// “零件尺寸”列。
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_SIZE);
	CGridColumnTrait* pPartSizeCol = new CGridColumnTraitText;
	m_partGrid.InsertColumnTrait(3, str, LVCFMT_LEFT, 140, 0, pPartSizeCol);

	// set other style.
	m_partGrid.SetExtendedStyle(LVS_EX_CHECKBOXES | m_partGrid.GetExtendedStyle());
	m_partGrid.EnableVisualStyles(TRUE);
	/************************************************************************/

	// 初始化数据。
	for (unsigned int i = 0; i < m_partList.size(); i++)
	{
		// insert a new row.
		LVITEM lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_PARAM;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		lvItem.pszText = _T("");
		m_partGrid.InsertItem(&lvItem);

		// "num" column
		CString strNum;
		strNum.Format(_T("%d"), i+1);
		m_partGrid.SetItemText(i, 1, strNum);

		// 零件名称。
		CString strPartName;
		strPartName.Format(_T("%s-%d"), m_strName, i+1);
		m_partGrid.SetItemText(i, 2, m_partList.at(i).first);

		// 零件尺寸。
		PatternLoopListPtr pPatternLoopList = m_partList.at(i).second;
		Rect2D outerRect = pPatternLoopList->GetRectBox();
		CString strPartSize;
		strPartSize.Format(_T("%.3f * %.3f"), outerRect.GetWidth(), outerRect.GetHeight());
		m_partGrid.SetItemText(i, 3, strPartSize);

		// 设置数据。
		m_partGrid.SetItemData(i, (DWORD_PTR)(new LONGLONG(m_partList.at(i).second->GetID())));

		// 选中该行。
		m_partGrid.SetCheck(i, TRUE);
	}
}

END_CUTLEADER_NAMESPACE()
