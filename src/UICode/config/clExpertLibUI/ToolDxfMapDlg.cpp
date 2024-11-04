#include "StdAfx.h"
#include "ToolDxfMapDlg.h"

#include "math.h"
#include <set>
#include "baseConst.h"
#include "clBaseDataResource.h"
#include "clUtilityResource.h"
#include "CGridColumnManagerProfile.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridRowTraitXP.h"
#include "clExpertLibResource.h"
#include "DataHelper.h"

#include "ToolInstance.h"
#include "ToolInstanceList.h"
#include "DxfLayerToolMap.h"
#include "ToolAssignConfigItem.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(ToolDxfMapDlg, CDialogEx)

ToolDxfMapDlg::ToolDxfMapDlg()
	: CLDialog(ToolDxfMapDlg::IDD)
{
}

ToolDxfMapDlg::~ToolDxfMapDlg(void)
{
}

void ToolDxfMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_DXF_MAP_BY_LAYER, m_nDxfMapType);
	DDX_Control(pDX, IDC_LIST_DXFLAYER_MAP_RULES, m_gridList_layerMap);
	DDX_Control(pDX, IDC_LIST_DXFCOLOR_MAP_RULES, m_gridList_colorMap);
}

BEGIN_MESSAGE_MAP(ToolDxfMapDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()

	ON_MESSAGE(WM_GET_CELL_EDITOR_DATA, GetCellEditorData)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DXFLAYER_MAP_RULES, OnLayerGrid_endLabelEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DXFCOLOR_MAP_RULES, OnColorGrid_endLabelEdit)

	ON_BN_CLICKED(IDC_RADIO_DXF_MAP_BY_LAYER, OnMapByLayerClicked)
	ON_BN_CLICKED(IDC_RADIO_DXF_MAP_BY_COLOR, OnMapByColorClicked)
	ON_BN_CLICKED(IDC_RADIO_DXF_MAP_NONE, OnMapNoneClicked)

	ON_BN_CLICKED(IDC_BTN_ADD_RULE, OnAddRule)
	ON_BN_CLICKED(IDC_BTN_DELETE_RULE, OnDeleteRule)
	ON_BN_CLICKED(IDC_BTN_DXFMAP_OK, OnOkClicked)
	ON_BN_CLICKED(IDC_BTN_DXFMAP_CANCEL, OnCancelClicked)
END_MESSAGE_MAP()

BOOL ToolDxfMapDlg::OnInitDialog()
{
	__super::OnInitDialog();

	InitGridStyle();

	// ȷ��ӳ�䷽ʽ
	if (m_pToolAssignConfigItem->GetDxfLayerToolMapType() == DxfLayerToolMap_layer)
	{
		m_nDxfMapType = 0;
	}
	else if (m_pToolAssignConfigItem->GetDxfLayerToolMapType() == DxfLayerToolMap_color)
	{
		m_nDxfMapType = 1;
	}
	else if (m_pToolAssignConfigItem->GetDxfLayerToolMapType() == DxfLayerToolMap_notUse)
	{
		m_nDxfMapType = 2;
	}

	// ��ʼ��ÿ���б�
	DxfLayerToolMapPtr pDxfLayerToolMap = m_pToolAssignConfigItem->GetDxfLayerToolMap();
	vector<LayerMapInfo> layerMapInfoList = pDxfLayerToolMap->GetLayerMapInfoList();
	vector<ColorMapInfo> colorMapInfoList = pDxfLayerToolMap->GetColorMapInfoList();
	for (unsigned int i = 0; i < layerMapInfoList.size(); i++)
	{
		LayerMapInfo layerMapInfo = layerMapInfoList.at(i);

		// insert an item.
		int iRowCount = m_gridList_layerMap.GetItemCount();
		m_gridList_layerMap.InsertItem(iRowCount, _T(""));
		m_gridList_layerMap.SetItemData(iRowCount, (DWORD_PTR)(new CString(layerMapInfo.m_strLayerName)));

		// ���
		CString str;
		str.Format(_T("%d"), iRowCount+1);
		m_gridList_layerMap.SetItemText(iRowCount, 1, str);
		m_gridList_layerMap.SetItemText(iRowCount, 2, layerMapInfo.m_strLayerName);
		m_gridList_layerMap.SetItemText(iRowCount, 3, layerMapInfo.m_pToolInstance->GetToolName());
	}
	for (unsigned int i = 0; i < colorMapInfoList.size(); i++)
	{
		ColorMapInfo colorMapInfo = colorMapInfoList.at(i);

		// insert an item.
		int iRowCount = m_gridList_colorMap.GetItemCount();
		m_gridList_colorMap.InsertItem(iRowCount, _T(""));
		m_gridList_colorMap.SetItemData(iRowCount, (DWORD_PTR)(new int(colorMapInfo.m_iColorIndex)));

		// ���
		CString str;
		str.Format(_T("%d"), iRowCount+1);
		m_gridList_colorMap.SetItemText(iRowCount, 1, str);
		str.Format(_T("%d"), colorMapInfo.m_iColorIndex);
		m_gridList_colorMap.SetItemText(iRowCount, 2, str);
		m_gridList_colorMap.SetItemText(iRowCount, 3, colorMapInfo.m_pToolInstance->GetToolName());
	}
	UpdateStatus_of_twoGrids();

	UpdateData(FALSE);
	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BOOL ToolDxfMapDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH ToolDxfMapDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void ToolDxfMapDlg::OnDestroy()
{
	// �ͷ�grid����Դ
	for (unsigned int i = 0; i < m_gridList_layerMap.GetItemCount(); i++)
	{
		CString* pLayerName = (CString*)m_gridList_layerMap.GetItemData(i);
		delete pLayerName;
	}
	for (unsigned int i = 0; i < m_gridList_colorMap.GetItemCount(); i++)
	{
		COLORREF* pColor = (COLORREF*)m_gridList_colorMap.GetItemData(i);
		delete pColor;
	}

	CDialogEx::OnDestroy();
}

void ToolDxfMapDlg::OnLayerGrid_endLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	int iRowIndex = pDispInfo->item.iItem;
	CString str = pDispInfo->item.pszText;

	// ��ǰ�޸ĵĹ���
	CString* pItemData = (CString*)m_gridList_layerMap.GetItemData(iRowIndex);
	LayerMapInfo* pLayerMapInfo = m_pToolAssignConfigItem->GetDxfLayerToolMap()->GetLayerMapInfo(*pItemData);

	int iColumnIndex = pDispInfo->item.iSubItem;
	if (iColumnIndex == 2)
	{
		if (LayerName_exist(str)) // ��ĳһ���ظ���
		{
			m_gridList_layerMap.SetItemText(iRowIndex, 2, pLayerMapInfo->m_strLayerName);
		}
		else
		{
			if (pLayerMapInfo->m_strLayerName != str)
			{
				pLayerMapInfo->m_strLayerName = str;
				m_pToolAssignConfigItem->SetModified(TRUE);
				m_gridList_layerMap.SetItemData(iRowIndex, (DWORD_PTR)(new CString(str)));
			}
		}
	}
	else if (iColumnIndex == 3)
	{
		if (pLayerMapInfo->m_pToolInstance->GetToolName() != str)
		{
			ToolInstancePtr pToolInstance_new = m_pToolAssignConfigItem->GetToolInstanceList()->GetToolByName(str);
			pLayerMapInfo->m_pToolInstance = pToolInstance_new;
			m_pToolAssignConfigItem->SetModified(TRUE);
		}
	}
}

void ToolDxfMapDlg::OnColorGrid_endLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	USES_CONVERSION;

	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	int iRowIndex = pDispInfo->item.iItem;
	CString str = pDispInfo->item.pszText;

	// ��ǰ�޸ĵĹ���
	COLORREF* pItemData = (COLORREF*)m_gridList_colorMap.GetItemData(iRowIndex);
	ColorMapInfo* pColorMapInfo = m_pToolAssignConfigItem->GetDxfLayerToolMap()->GetColorMapInfo(*pItemData);

	int iColumnIndex = pDispInfo->item.iSubItem;
	if (iColumnIndex == 2)
	{
		int iColorIndex = atoi(W2A(str));
		if (LayerColor_exist(iColorIndex)) // ��ĳһ���ظ���
		{
			str.Format(_T("%d"), pColorMapInfo->m_iColorIndex);
			m_gridList_colorMap.SetItemText(iRowIndex, 2, str);
		}
		else
		{
			if (pColorMapInfo->m_iColorIndex != iColorIndex)
			{
				pColorMapInfo->m_iColorIndex = iColorIndex;
				m_pToolAssignConfigItem->SetModified(TRUE);
				m_gridList_colorMap.SetItemData(iRowIndex, (DWORD_PTR)(new int(iColorIndex)));
			}
		}
	}
	else if (iColumnIndex == 3)
	{
		if (pColorMapInfo->m_pToolInstance->GetToolName() != str)
		{
			ToolInstancePtr pToolInstance_new = m_pToolAssignConfigItem->GetToolInstanceList()->GetToolByName(str);
			pColorMapInfo->m_pToolInstance = pToolInstance_new;
			m_pToolAssignConfigItem->SetModified(TRUE);
		}
	}
}

LRESULT ToolDxfMapDlg::GetCellEditorData(WPARAM wParam, LPARAM lParam)
{
	CellEditorData* pCellEditorData = reinterpret_cast<CellEditorData*>(wParam);
	pCellEditorData->m_cellEditorType = COMBOBOX;
	pCellEditorData->m_optionValList.clear();
	ToolInstanceListPtr pToolInstanceList = m_pToolAssignConfigItem->GetToolInstanceList();
	for (unsigned int i = 0; i < pToolInstanceList->size(); i++)
	{
		pCellEditorData->m_optionValList.push_back(pToolInstanceList->at(i)->GetToolName());
	}

	return true;
}

void ToolDxfMapDlg::OnMapByLayerClicked()
{
	UpdateData(TRUE);
	UpdateStatus_of_twoGrids();
	if (m_pToolAssignConfigItem->GetDxfLayerToolMapType() != DxfLayerToolMap_layer)
	{
		m_pToolAssignConfigItem->SetDxfLayerToolMapType(DxfLayerToolMap_layer);
		m_pToolAssignConfigItem->SetModified(TRUE);
	}
}

void ToolDxfMapDlg::OnMapByColorClicked()
{
	UpdateData(TRUE);
	UpdateStatus_of_twoGrids();
	if (m_pToolAssignConfigItem->GetDxfLayerToolMapType() != DxfLayerToolMap_color)
	{
		m_pToolAssignConfigItem->SetDxfLayerToolMapType(DxfLayerToolMap_color);
		m_pToolAssignConfigItem->SetModified(TRUE);
	}
}

void ToolDxfMapDlg::OnMapNoneClicked()
{
	UpdateData(TRUE);
	UpdateStatus_of_twoGrids();
	if (m_pToolAssignConfigItem->GetDxfLayerToolMapType() != DxfLayerToolMap_notUse)
	{
		m_pToolAssignConfigItem->SetDxfLayerToolMapType(DxfLayerToolMap_notUse);
		m_pToolAssignConfigItem->SetModified(TRUE);
	}
}

void ToolDxfMapDlg::OnAddRule()
{
	CString str;
	if (m_nDxfMapType == 0)
	{
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TOOL_MAP_LAYERNAME);
		str = CheckLayerName(str);
		LayerMapInfo layerMapInfo(str, m_pToolAssignConfigItem->GetDefaultTool_4_outerLoop());
		m_pToolAssignConfigItem->GetDxfLayerToolMap()->AddLayerMapInfo(layerMapInfo);
		m_pToolAssignConfigItem->SetModified(TRUE);

		// insert an item.
		int iRowCount = m_gridList_layerMap.GetItemCount();
		m_gridList_layerMap.InsertItem(iRowCount, _T(""));
		m_gridList_layerMap.SetItemData(iRowCount, (DWORD_PTR)(new CString(layerMapInfo.m_strLayerName)));

		// ���
		str.Format(_T("%d"), iRowCount+1);
		m_gridList_layerMap.SetItemText(iRowCount, 1, str);
		m_gridList_layerMap.SetItemText(iRowCount, 2, layerMapInfo.m_strLayerName);
		m_gridList_layerMap.SetItemText(iRowCount, 3, layerMapInfo.m_pToolInstance->GetToolName());
	}
	else if (m_nDxfMapType == 1)
	{
		int iColorIndex = CheckLayerColor(1);
		ColorMapInfo colorMapInfo(iColorIndex, m_pToolAssignConfigItem->GetDefaultTool_4_outerLoop());
		m_pToolAssignConfigItem->GetDxfLayerToolMap()->AddColorMapInfo(colorMapInfo);
		m_pToolAssignConfigItem->SetModified(TRUE);

		// insert an item.
		int iRowCount = m_gridList_colorMap.GetItemCount();
		m_gridList_colorMap.InsertItem(iRowCount, _T(""));
		m_gridList_colorMap.SetItemData(iRowCount, (DWORD_PTR)(new int(colorMapInfo.m_iColorIndex)));

		// ���
		str.Format(_T("%d"), iRowCount+1);
		m_gridList_colorMap.SetItemText(iRowCount, 1, str);
		str.Format(_T("%d"), colorMapInfo.m_iColorIndex);
		m_gridList_colorMap.SetItemText(iRowCount, 2, str);
		m_gridList_colorMap.SetItemText(iRowCount, 3, colorMapInfo.m_pToolInstance->GetToolName());
	}
}

void ToolDxfMapDlg::OnDeleteRule()
{
	CString strMsg, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_DELETE_TOOL_ASSIGN_RULE);

	if (m_nDxfMapType == 0)
	{
		int iIndex_selectedRule = INVALID_IDX;
		for (int i = 0; i < m_gridList_layerMap.GetItemCount(); i++)
		{
			if (m_gridList_layerMap.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
				iIndex_selectedRule = i;
		}

		if (iIndex_selectedRule != INVALID_IDX && MessageBox(strMsg, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			CString* pLayerName = (CString*)m_gridList_layerMap.GetItemData(iIndex_selectedRule);
			m_pToolAssignConfigItem->GetDxfLayerToolMap()->DeleteLayerMapInfo(*pLayerName);
			m_pToolAssignConfigItem->SetModified(TRUE);

			// ����grid
			delete pLayerName;
			m_gridList_layerMap.DeleteItem(iIndex_selectedRule);
			for (int i = 0; i < m_gridList_layerMap.GetItemCount(); i++)
			{
				CString strNum;
				strNum.Format(_T("%d"), i+1);
				m_gridList_layerMap.SetItemText(i, 1, strNum);
			}
		}
	}
	else if (m_nDxfMapType == 1)
	{
		int iIndex_selectedRule = INVALID_IDX;
		for (int i = 0; i < m_gridList_colorMap.GetItemCount(); i++)
		{
			if (m_gridList_colorMap.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
				iIndex_selectedRule = i;
		}

		if (iIndex_selectedRule != INVALID_IDX && MessageBox(strMsg, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			int* pColorIndex = (int*)m_gridList_colorMap.GetItemData(iIndex_selectedRule);
			m_pToolAssignConfigItem->GetDxfLayerToolMap()->DeleteColorMapInfo(*pColorIndex);
			m_pToolAssignConfigItem->SetModified(TRUE);

			// ����grid
			delete pColorIndex;
			m_gridList_colorMap.DeleteItem(iIndex_selectedRule);
			for (int i = 0; i < m_gridList_colorMap.GetItemCount(); i++)
			{
				CString strNum;
				strNum.Format(_T("%d"), i+1);
				m_gridList_colorMap.SetItemText(i, 1, strNum);
			}
		}
	}
}

void ToolDxfMapDlg::OnOkClicked()
{
	EndDialog(IDC_BTN_DXFMAP_OK);
}

void ToolDxfMapDlg::OnCancelClicked()
{
	EndDialog(IDC_BTN_DXFMAP_CANCEL);
}

void ToolDxfMapDlg::InitGridStyle()
{
	CString str;

	// ͼ��ӳ��
	{
		// Requires one never uses column 0
		m_gridList_layerMap.InsertHiddenLabelColumn();

		// ���
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
		CGridColumnTrait* pNumCol = new CGridColumnTraitText;
		m_gridList_layerMap.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNumCol);

		// ����
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TOOL_MAP_LAYERNAME);
		CGridColumnTrait* pNameCol = new CGridColumnTraitEdit(STREDITBOX);
		m_gridList_layerMap.InsertColumnTrait(2, str, LVCFMT_LEFT, 120, 0, pNameCol);

		// ����ĵ���
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TOOL);
		CGridColumnTrait* pToolCol = new CGridColumnTraitText;
		m_gridList_layerMap.InsertColumnTrait(3, str, LVCFMT_LEFT, 150, 0, pToolCol);
		m_gridList_layerMap.AddDynamicCol(3);

		// set other style.
		m_gridList_layerMap.EnableVisualStyles(TRUE);
		m_gridList_layerMap.IsSendItemClickEventToParent(true);
	}

	// ��ɫӳ��
	{
		// Requires one never uses column 0
		m_gridList_colorMap.InsertHiddenLabelColumn();

		// ���
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
		CGridColumnTrait* pNumCol = new CGridColumnTraitText;
		m_gridList_colorMap.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNumCol);

		// ����
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COLOR);
		CGridColumnTrait* pColorCol = new CGridColumnTraitEdit(INTEDITBOX);
		m_gridList_colorMap.InsertColumnTrait(2, str, LVCFMT_LEFT, 120, 0, pColorCol);

		// ����ĵ���
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_TOOL);
		CGridColumnTrait* pToolCol = new CGridColumnTraitText;
		m_gridList_colorMap.InsertColumnTrait(3, str, LVCFMT_LEFT, 150, 0, pToolCol);
		m_gridList_colorMap.AddDynamicCol(3);

		// set other style.
		m_gridList_colorMap.EnableVisualStyles(TRUE);
		m_gridList_colorMap.IsSendItemClickEventToParent(true);
	}
}

void ToolDxfMapDlg::UpdateStatus_of_twoGrids()
{
	if (m_nDxfMapType == 0)
	{
		m_gridList_layerMap.ShowWindow(SW_SHOW);
		m_gridList_layerMap.EnableWindow(TRUE);
		m_gridList_colorMap.ShowWindow(SW_HIDE);

		((CWnd*)GetDlgItem(IDC_BTN_ADD_RULE))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_BTN_DELETE_RULE))->EnableWindow(TRUE);
	}
	else if (m_nDxfMapType == 1)
	{
		m_gridList_layerMap.ShowWindow(SW_HIDE);
		m_gridList_colorMap.ShowWindow(SW_SHOW);
		m_gridList_colorMap.EnableWindow(TRUE);

		((CWnd*)GetDlgItem(IDC_BTN_ADD_RULE))->EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_BTN_DELETE_RULE))->EnableWindow(TRUE);
	}
	else if (m_nDxfMapType == 2)
	{
		m_gridList_layerMap.EnableWindow(FALSE);
		m_gridList_colorMap.EnableWindow(FALSE);

		((CWnd*)GetDlgItem(IDC_BTN_ADD_RULE))->EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_DELETE_RULE))->EnableWindow(FALSE);
	}
}

bool ToolDxfMapDlg::LayerName_exist(CString strName)
{
	LayerMapInfo* pLayerMapInfo = m_pToolAssignConfigItem->GetDxfLayerToolMap()->GetLayerMapInfo(strName);
	if (pLayerMapInfo)
	{
		return true;
	}

	return false;
}

CString ToolDxfMapDlg::CheckLayerName(CString strName)
{
	int iPostFix = 1;
	CString strName_test = strName;
	do 
	{
		if (LayerName_exist(strName_test))
		{
			strName_test.Format(_T("%s%d"), strName, iPostFix);
			iPostFix++;
		}
		else
		{
			break;
		}
	} while (true);

	return strName_test;
}

bool ToolDxfMapDlg::LayerColor_exist(int iColorIndex)
{
	ColorMapInfo* pColorMapInfo = m_pToolAssignConfigItem->GetDxfLayerToolMap()->GetColorMapInfo(iColorIndex);
	if (pColorMapInfo)
	{
		return true;
	}

	return false;
}

int ToolDxfMapDlg::CheckLayerColor(int iColorIndex)
{
	int iColorIndex_test = iColorIndex;
	do 
	{
		if (LayerColor_exist(iColorIndex_test))
		{
			iColorIndex_test++;
		}
		else
		{
			break;
		}
	} while (true);

	return iColorIndex_test;
}

END_CUTLEADER_NAMESPACE()
