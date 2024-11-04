#include "stdafx.h"
#include "NCViewBar.h"

#include "baseConst.h"
#include "clUtilityResource.h"
#include "GlViewPort.h"
#include "clUILibResource.h"
#include "clBaseDataResource.h"
#include "versionInfo.h"
#include "MultiLanguageMgr.h"
#include "CGridColumnTraitText.h"
#include "CGridColumnTraitEdit.h"

#include "CutNode.h"
#include "CutNodeList.h"
#include "ISequenceSimulateShape.h"
#include "SequenceSimulateShapeList.h"
#include "CutSequenceDrawer.h"

#include "NCCodeData.h"
#include "NCContainer.h"
#include "NCSimulateItem.h"
#include "NCSimulateItemList.h"
#include "NCRow.h"
#include "NCRowList.h"
#include "clPostProcessorResource.h"
#include "NCSimulateAction.h"
#include "MacRouterEditor.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

NCViewBar* NCViewBar::m_pNCViewBar = NULL;

BEGIN_MESSAGE_MAP(NCViewBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL_NC, OnItemchangedList)
    ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LISTCTRL_NC, OnGrid_beginLabelEdit)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LISTCTRL_NC, OnGrid_endLabelEdit)

	ON_BN_CLICKED(IDC_MACROUTER_SAVE_NC_CODE, OnSaveNCCode)
	ON_UPDATE_COMMAND_UI(IDC_MACROUTER_SAVE_NC_CODE, OnUpdateClickNCBtn)

END_MESSAGE_MAP()

NCViewBar::NCViewBar()
{
	m_bFillingGrid = FALSE;
}

NCViewBar::~NCViewBar()
{
}

// get the singleton object
NCViewBar* NCViewBar::GetInstance(CWnd* pParent)
{
	if (m_pNCViewBar == NULL)
	{
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MACROUTER_NC_CODE);

		m_pNCViewBar = new NCViewBar();
		m_pNCViewBar->m_pMainWnd = pParent;
		if (!m_pNCViewBar->Create(str, (CWnd*)pParent, CRect(0, 0, RIGHT_DOCK_PANE_WIDTH, 200), TRUE, IDC_MACROUTER_VIEW_NC_BAR, 
								  WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
			return NULL;
	}

	return m_pNCViewBar;
}

/////////////////////////////////////////////////////////////////////////////

int NCViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create controls.
	CreateNCGrid();
	CreateNCBtn();

	// layout the windows in this pane.
	AdjustLayout();

	return 0;
}

void NCViewBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// layout the windows in this pane.
	AdjustLayout();
}

void NCViewBar::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_saveNCBtn.EnableWindow(TRUE);
}

void NCViewBar::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
		return;

	CRect rectClient;
	GetClientRect(rectClient);

	// the height of the two windows.
	int iHeight1 = rectClient.Height() * 19 / 20;
	int iHeight2 = rectClient.Height() * 1 / 20;
	int iDeduction = 0; // the space between two windows.

	m_ncListGrid.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), iHeight1-iDeduction, SWP_NOACTIVATE | SWP_NOZORDER);
	m_saveNCBtn.SetWindowPos(NULL, rectClient.left, iHeight1, rectClient.Width(), iHeight2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void NCViewBar::InitBar(CString strSheetName, SequenceSimulateShapeListPtr pSeqSimShapeList, NCCodeDataPtr pNCCodeData, MacRouterEditorPtr pPostEditor, CString strNCPostfix,
                        LoopInstanceListPtr pAllLoopInstances, const std::set<LONGLONG>& patternLoops_failedToOffset)
{
	m_strSheetName = strSheetName;
	m_pSequenceSimulateShapeList = pSeqSimShapeList;
	m_pNCCodeData = pNCCodeData;
	m_pMacRouterEditor = pPostEditor;
	m_strNCPostfix = strNCPostfix;
    m_pAllLoopInstances = pAllLoopInstances;
    m_patternLoops_failedToOffset = patternLoops_failedToOffset;

	// init the grid.
	m_bFillingGrid = TRUE;
	InitNCGrid();
	m_bFillingGrid = FALSE;

	// select the first row.
	m_ncListGrid.SelectRow(0, TRUE);
}

void NCViewBar::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_bFillingGrid)
		return;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// get the selected NC row, and simulate it.
	if (pNMListView->iItem >= 0)
	{
		NCSimulateItemListPtr pNCSimItemList = m_pNCCodeData->GetNCSimItemList();

		// get the selected NC row objects.
		IntListPtr pNCRowList = GetSelectedNCRow();

		// get all cut nodes which relative ti these NC rows.
		CutNodeListPtr pCutNodeList(new CutNodeList);
		for (unsigned int i = 0; i < pNCRowList->size(); i++)
		{
			int iNCRowID = pNCRowList->at(i);

			// get the simulate shapes of the NC row.
			NCSimulateItemPtr pNCSimItem = pNCSimItemList->GetNCSimItem(iNCRowID);
			if (pNCSimItem)
			{
				// because different NC row can use same cut node, we should filter.
				ICutNodePtr pCutNode = pNCSimItem->GetCutNode();
				if (!pCutNodeList->GetCutNodeByID(pCutNode->GetID()))
					pCutNodeList->push_back(pCutNode);			
			}
		}

		/************************************************************************/
		// create NCSimulateAction action to simulate the selected NC rows.

		m_pMacRouterEditor->SetAction(MACROUTER_NC_SIMULATE);
		NCSimulateActionPtr pNCSimulateAction = boost::dynamic_pointer_cast<NCSimulateAction>(m_pMacRouterEditor->GetActionObject());
        pNCSimulateAction->GetWrongToolSizeLoopInstances()->Init(m_pAllLoopInstances.get(), m_patternLoops_failedToOffset);
		pNCSimulateAction->ViewCutNode(m_pSequenceSimulateShapeList, pCutNodeList);

		// redraw the view.
		CWnd* pView = GetRelativeView();
		pView->Invalidate();
		/************************************************************************/
	}

	*pResult = 0;
}

void NCViewBar::OnGrid_beginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
    int iRowIndex = pDispInfo->item.iItem;
    CString str = pDispInfo->item.pszText;

    int iColumnIndex = pDispInfo->item.iSubItem;
    if (iColumnIndex == 2)
    {
        m_strCellText_beforeEdit = m_ncListGrid.GetItemText(iRowIndex, 2);
    }
}

void NCViewBar::OnGrid_endLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
    int iRowIndex = pDispInfo->item.iItem;
    CString str = pDispInfo->item.pszText;

    int iColumnIndex = pDispInfo->item.iSubItem;
    if (iColumnIndex == 2)
    {
        if (str != m_strCellText_beforeEdit)
        {
            int iRowID = m_ncListGrid.GetItemData(iRowIndex);
            NCRowPtr pNCRow = m_pNCCodeData->GetNCRowList()->GetNCRowByID(iRowID);
            pNCRow->SetNCRow(str);
        }
    }
}

void NCViewBar::OnSaveNCCode()
{
	// prepare some data.
	CString strFilter, strNCExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NC_FILEFILTER);
	strFilter.Replace(_T("WILLBEREPLACED1"), m_strNCPostfix);
	strNCExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NC_FILEEXT);
	strNCExt.Replace(_T("WILLBEREPLACED1"), m_strNCPostfix);

	CFileDialog saveDlg(FALSE, NULL, m_strSheetName, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter, NULL);
	if (saveDlg.DoModal() == IDOK)
	{
		// get the full path name.
		CString strFilePath = saveDlg.GetPathName();
		if (strFilePath.Find(strNCExt) == -1)
			strFilePath += strNCExt;

		// check whether this file exists.
		CFileFind finder;  
		if (finder.FindFile(strFilePath))  
		{
			CString tmpStr;
			tmpStr = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NC_FILE_EXIST);
			if (MessageBox(tmpStr, NULL, MB_YESNO | MB_ICONQUESTION) != IDYES)
				return;
		}
		finder.Close();

		// save the NC code
		CStdioFile file(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		for (int i = 0; i < m_ncListGrid.GetItemCount(); i++)
		{
			CString str = m_ncListGrid.GetItemText(i, 2);
			file.WriteString(str);
			file.WriteString(_T("\n"));
		}
	}
}

void NCViewBar::OnUpdateClickNCBtn(CCmdUI* pCmdUI)
{
	pCmdUI->Enable();
}

BOOL NCViewBar::CreateNCGrid()
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// create the listctrl.
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_VSCROLL | LVS_REPORT;
	if (!m_ncListGrid.Create(dwStyle, rectDummy, this, IDC_LISTCTRL_NC))
		return FALSE;

	CString str;

    // Requires one never uses column 0
    m_ncListGrid.InsertHiddenLabelColumn();

    // ÐòºÅÁÐ
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
    CGridColumnTrait* pNumCol = new CGridColumnTraitText;
    m_ncListGrid.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNumCol);

    // ÄÚÈÝÁÐ
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MACROUTER_NC_CODE);
    CGridColumnTrait* pContentCol = new CGridColumnTraitEdit;
    m_ncListGrid.InsertColumnTrait(2, str, LVCFMT_LEFT, 180, 0, pContentCol);

    m_ncListGrid.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;
}

BOOL NCViewBar::CreateNCBtn()
{
	CString str;

	CRect rectDummy (0, 0, 0, 0);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MACROUTER_SAVE_NC_CODE);
	m_saveNCBtn.Create(str, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, rectDummy, this, IDC_MACROUTER_SAVE_NC_CODE);

	return TRUE;
}

void NCViewBar::InitNCGrid()
{
	m_ncListGrid.DeleteAllItems();

	NCRowListPtr pNCRowList = m_pNCCodeData->GetNCRowList();
	for (unsigned int i = 0; i < pNCRowList->size(); i++)
	{
		NCRowPtr pNCRow = pNCRowList->at(i);

		// insert an item.
		m_ncListGrid.InsertItem(i, _T(""));
        m_ncListGrid.SetItemData(i, pNCRow->GetNCRowID());

        // "num" column
        CString strNum;
        strNum.Format(INT_FORMAT, i+1);
        m_ncListGrid.SetItemText(i, 1, strNum);

		// set the second column.
		m_ncListGrid.SetItemText(i, 2, pNCRow->GetNCRow());
	}
}

IntListPtr NCViewBar::GetSelectedNCRow()
{
	IntListPtr pIntList(new vector<int>);

	for (int i = 0; i < m_ncListGrid.GetItemCount(); i++)
	{
		if (m_ncListGrid.IsRowSelected(i))
		{
			int iItemData = m_ncListGrid.GetItemData(i);
			pIntList->push_back(iItemData);
		}
	}

	return pIntList;
}

END_CUTLEADER_NAMESPACE()
