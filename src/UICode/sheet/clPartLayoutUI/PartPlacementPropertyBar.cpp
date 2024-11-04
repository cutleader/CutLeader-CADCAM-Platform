#include "StdAfx.h"
#include "PartPlacementPropertyBar.h"

#include "DBConnect.h"
#include "clUtilityResource.h"
#include "NumberProp.h"
#include "baseConst.h"

#include "Rect2D.h"
#include "CommandManager.h"
#include "clUILibResource.h"
#include "DataBaseManager.h"
#include "DataItem.h"
#include "ProductItemLoader.h"
#include "clBaseDataResource.h"

#include "PatternList.h"
#include "clGeometryFeatureResource.h"
#include "LoopToolData.h"
#include "LoopCutFeature.h"
#include "PartCamData.h"
#include "PartItem.h"
#include "Part.h"

#include "PartPlacementList.h"
#include "clPartLayoutResource.h"
#include "PartLayoutEditor.h"
#include "PartPlacement.h"
#include "GridSpaceModifyCommand.h"
#include "PartPlacementPropertyEditCommand.h"
#include "PartGridGapDlg.h"
#include "PartPlacementManager.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementPropertyBar* PartPlacementPropertyBar::m_pPartPmtPropBar = NULL;

BEGIN_MESSAGE_MAP(PartPlacementPropertyBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChange)

	ON_BN_CLICKED(IDC_SET_GRID_GAP, OnSetGridGap)
	ON_UPDATE_COMMAND_UI(IDC_SET_GRID_GAP, OnUpdateClickBtn)
END_MESSAGE_MAP()

PartPlacementPropertyBar::PartPlacementPropertyBar(void)
{
	m_pPartPlacementList.reset();
}

PartPlacementPropertyBar::~PartPlacementPropertyBar(void)
{
}

PartPlacementPropertyBar* PartPlacementPropertyBar::GetInstance(CWnd* pParent)
{
	if (m_pPartPmtPropBar == NULL)
	{
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PROPBAR_EDITPRTPMT);

		m_pPartPmtPropBar = new PartPlacementPropertyBar();
		m_pPartPmtPropBar->m_pMainWnd = pParent;
		if (!m_pPartPmtPropBar->Create(str, (CWnd*)pParent, CRect(0, 0, RIGHT_DOCK_PANE_WIDTH, 200), TRUE, IDC_PROPBAR_EDITPRTPMT, 
									   WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_RIGHT|CBRS_FLOAT_MULTI))
			return NULL;
	}

	return m_pPartPmtPropBar;
}

int PartPlacementPropertyBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create controls.
	CreatePropWnd();
	CreateSetGapBtn();

	// layout the windows in this pane.
	AdjustLayout();

	return 0;
}

void PartPlacementPropertyBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// layout the windows in this pane.
	AdjustLayout();
}

LRESULT PartPlacementPropertyBar::OnPropertyChange(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* prop = (CMFCPropertyGridProperty *)lParam;
	COleVariant var = prop->GetValue();

	DataPropItem item = m_propInfo[(CObject*)prop];

	// check whether the input is valid.
	if (item.GetItemName() == IDS_PROP_REPEAT_NUM_X || item.GetItemName() == IDS_PROP_REPEAT_NUM_Y)
	{
		if (var.intVal < 1)
		{
			CString str, strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_REPEAT_NUM_SMALL_EX);
			::MessageBox(m_pMainWnd->m_hWnd, str, strProductName, MB_OK | MB_ICONWARNING);

			// refresh part pmt data.
			DisplayPartPmt(m_pPartPlacementList, m_pPartInstanceList, m_pViewPort);

			return S_OK;
		}
	}

	// create command
	DataPropItem* pNewPropItem = new DataPropItem(item.GetItemName(), var);
	DataPropItem* pOldPropItem = new DataPropItem(item.GetItemName(), item.GetValue());
	ICommandPtr pCommand(new PartPlacementPropertyEditCommand(GetEditorData(), m_pPartPlacementList->at(0), m_pPartInstanceList, pNewPropItem, pOldPropItem));
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	// 刷新界面。
	GetRelativeView()->Invalidate();

	return S_OK;
}

void PartPlacementPropertyBar::OnSetGridGap()
{
	if (m_pPartPlacementList->size() == 1)
	{
		PartPlacementPtr pPartPlacement = m_pPartPlacementList->at(0);
		if (pPartPlacement->IsGrid())
		{
			PartGridGapDlg dlg(m_pMainWnd);
			if (dlg.DoModal() == IDOK)
			{
				// 通过阵列新的零件间距，计算出行/列间距。
                BOOL bEnableComcut = FALSE;
				double dPartDis = dlg.GetPartDis(bEnableComcut);
                if (bEnableComcut)
                {
                    LoopCutFeaturePtr pBoundaryLoopFeature = pPartPlacement->GetPartItem()->GetCamData()->GetBoundaryLoopFeature();
                    dPartDis = pBoundaryLoopFeature->GetLoopTool()->GetToolWidth();
                }
				pair<double, double> spaceData = PartPlacementManager::CalcGridPartSpacing(pPartPlacement, dPartDis);

				// 执行命令。
				ICommandPtr pCommand(new GridSpaceModifyCommand(GetEditorData(), pPartPlacement, pPartPlacement->GetSpacingX(), pPartPlacement->GetSpacingY(), spaceData.first, spaceData.second));
				pCommand->Do();
				CommandManager::GetInstance()->AddCommand(pCommand);

				// update the props of grid pmt.
				DisplayPartPmt(m_pPartPlacementList, m_pPartInstanceList, m_pViewPort);
			}
		}
	}
}

void PartPlacementPropertyBar::OnUpdateClickBtn(CCmdUI* pCmdUI)
{
	if (m_pPartPlacementList && m_pPartPlacementList->size() == 1)
	{
		PartPlacementPtr pPartPlacement = m_pPartPlacementList->at(0);
		if (pPartPlacement->IsGrid())
		{
			pCmdUI->Enable();
			return;
		}
	}

	pCmdUI->Enable(FALSE);
}

void PartPlacementPropertyBar::DisplayPartPmt(PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, GlViewPortPtr pViewPort)
{
	m_pPartPlacementList = pPartPlacements;
	m_pPartInstanceList = pPartInstances;
	m_pViewPort = pViewPort;

	m_wndPropList.RemoveAll();
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	if (m_pPartPlacementList->size() == 1)
	{
		PartPlacementPtr pPartPlacement = m_pPartPlacementList->at(0);

		// get the property of the part placement
		DataProperty partPmtProp = pPartPlacement->GetProperty();

		// init "Part Placement Data" category
		vector<DataPropItem> partPmtPropItems;
		partPmtProp.GetItemsByType(IDS_CATEGORY_PARTPMT_DATA, partPmtPropItems);
		InitPropGroup(IDS_CATEGORY_PARTPMT_DATA, partPmtPropItems);

		// init "grid data" category
		vector<DataPropItem> gridPropItems;
		partPmtProp.GetItemsByType(IDS_CATEGORY_PARTPMT_GRID, gridPropItems);
		if (gridPropItems.size() > 0)
			InitPropGroup(IDS_CATEGORY_PARTPMT_GRID, gridPropItems);
	}
	else
	{
		// init the header for CMFCPropertyGridCtrl.
		CString strCol1, strCol2;
		strCol1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_TYPE);
		strCol2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COUNT);
		m_wndPropList.EnableHeaderCtrl(TRUE, strCol1, strCol2);

		//
		int iSinglePmtCount = 0, iGridPmtCount = 0;
		for (unsigned int i = 0; i < m_pPartPlacementList->size(); i++)
		{
			PartPlacementPtr pPartPlacement = m_pPartPlacementList->at(i);
			if (pPartPlacement->IsGrid())
				iGridPmtCount++;
			else
				iSinglePmtCount++;
		}

		/************************************************************************/
		// add two prop items.

		CString strSinglePmt, strGridPmt;
		strSinglePmt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_SINGLE);
		strGridPmt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PARTPMT_GRID);

		// for "single" part pmt.
		CString strSinglePmtCount;
		strSinglePmtCount.Format(_T("%d"), iSinglePmtCount);
		CMFCPropertyGridProperty* pPropItem1 = new CMFCPropertyGridProperty(strSinglePmt, strSinglePmtCount);
		pPropItem1->AllowEdit(FALSE);
		m_wndPropList.AddProperty(pPropItem1);

		// for "grid" part pmt.
		CString strGridPmtCount;
		strGridPmtCount.Format(_T("%d"), iGridPmtCount);
		CMFCPropertyGridProperty* pPropItem2 = new CMFCPropertyGridProperty(strGridPmt, strGridPmtCount);
		pPropItem2->AllowEdit(FALSE);
		m_wndPropList.AddProperty(pPropItem2);
		/************************************************************************/
	}

	if (!IsVisible())
		ShowPane(TRUE, FALSE, TRUE);
}

void PartPlacementPropertyBar::UpdatePmtProp()
{
	// 得到“零件放置”的属性。
	PartPlacementPtr pPartPlacement = m_pPartPlacementList->at(0);
	DataProperty partPmtProp = pPartPlacement->GetProperty();
	vector<DataPropItem> partPmtPropItems;
	partPmtProp.GetItemsByType(IDS_CATEGORY_PARTPMT_DATA, partPmtPropItems);

	// 更新每个属性值。
	CMFCPropertyGridProperty* pGeneralGroup = m_wndPropList.GetProperty(0);
	for (int i = 0; i < pGeneralGroup->GetSubItemsCount(); i++)
	{
		CMFCPropertyGridProperty* pProp = pGeneralGroup->GetSubItem(i);
		pProp->SetValue(partPmtPropItems.at(i).GetValue());
	}
}

void PartPlacementPropertyBar::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
		return;

	CRect rectClient;
	GetClientRect(rectClient);

	// the height of the two windows.
	int iHeight1 = rectClient.Height() * 19 / 20;
	int iHeight2 = rectClient.Height() * 1 / 20;
	int iDeduction = 0; // the space between two windows.

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), iHeight1-iDeduction, SWP_NOACTIVATE | SWP_NOZORDER);
	m_setGridGapBtn.SetWindowPos(NULL, rectClient.left, iHeight1, rectClient.Width(), iHeight2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL PartPlacementPropertyBar::CreatePropWnd()
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
		return -1;

	return TRUE;
}

BOOL PartPlacementPropertyBar::CreateSetGapBtn()
{
	CString str;

	CRect rectDummy (0, 0, 0, 0);
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SET_GRID_GAP);
	m_setGridGapBtn.Create(str, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, rectDummy, this, IDC_SET_GRID_GAP);

	return TRUE;
}

void PartPlacementPropertyBar::InitPropGroup(int iGroupName, vector<DataPropItem>& propItems)
{
	CString strGroupName;
	strGroupName = MultiLanguageMgr::GetInstance()->TranslateString(iGroupName);

	// go through each prop item.
	CMFCPropertyGridProperty* pGeneralGroup = new CMFCPropertyGridProperty(strGroupName);
	for (unsigned int i = 0; i < propItems.size(); i++)
	{
		DataPropItem item = propItems[i];

		// the name
		CString propName;
		propName = MultiLanguageMgr::GetInstance()->TranslateString(item.GetItemName());

		// 新建属性项。
		CMFCPropertyGridProperty* pProp = NULL;
		if (item.IsNumber())
			pProp = new NumberProp(propName, item.GetValue(), item.GetFormat(), NULL, 0, item.HasButton(), item.CanNegative());
		else
			pProp = new CMFCPropertyGridProperty(propName, item.GetValue(), _T(""));

		// 是否只读。
		if (item.IsReadOnly())
			pProp->Enable(FALSE);

		pGeneralGroup->AddSubItem(pProp);
		m_propInfo[(CObject*)pProp] = item;
	}

	m_wndPropList.AddProperty(pGeneralGroup);
}

END_CUTLEADER_NAMESPACE()
