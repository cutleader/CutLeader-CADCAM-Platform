#include "StdAfx.h"
#include "SheetView.h"

#include "resource.h"
#include "baseConst.h"
#include "PathHelper.h"
#include "DataBaseManager.h"
#include "LineArc2DList.h"
#include "clBaseDataResource.h"
#include "versionInfo.h"
#include "clSecureResource.h"
#include "LogMgr.h"

#include "PatternLoopList.h"
#include "GeometryFeatureEditor.h"
#include "PatternPropertyBar.h"
#include "LoopTopologyItem.h"
#include "LoopTopologyItemList.h"
#include "LoopProcessor.h"

#include "PartTopologyItem.h"
#include "PartTopologyItemList.h"
#include "PartCadData.h"
#include "PartCamDataList.h"
#include "PartItem.h"
#include "PartItemList.h"
#include "Part.h"
#include "PartBuilder.h"
#include "PartManager.h"

#include "PartPlacementList.h"
#include "PartInstanceList.h"
#include "PartPlacementAddPanel.h"
#include "PartLayoutEditor.h"
#include "PartPlacementPropertyBar.h"
#include "PartPlacementViewBar.h"
#include "PartTopologyBar.h"
#include "PartInstanceTopologyItemList.h"
#include "PartPlacementManager.h"
#include "Sheet.h"
#include "clSheetResource.h"
#include "RemnantMatInfoDlg.h"

#include "ExtImpDataSelDlg.h"
#include "ImportManager.h"
#include "clDataImportExportResource.h"
#include "Loop2PartDlg.h"

#include "SheetDocument.h"
#include "UIManager.h"
#include "CommandManager.h"
#include "InteractBar.h"
#include "ProductItemLoader.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "ParamConfig.h"
#include "ParamConfigLoader.h"
#include "SequenceConfigItem.h"
#include "MaterialLibLoader.h"
#include "IMaterialSize.h"
#include "PartInstanceQuantityDlg.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

void SheetView::OnPartPmtAddPart()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

    // 此时切换到零件选择命令
    m_pPartLayoutEditor->SetEditAction(EDIT_SELECTPRTPMT);

	// 加载当前板材使用的“参数配置”。
	SheetPtr pSheet = GetCurrentSheet();
	LONGLONG iParamConfigID = pSheet->GetParamConfigID();
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, iParamConfigID);

	// 显示零件选择界面。
	PartPlacementAddPanel* pAddPrtInstPane = PartPlacementAddPanel::GetInstance();
	pAddPrtInstPane->ShowPane(!pAddPrtInstPane->IsVisible(), FALSE, TRUE);
	if (pAddPrtInstPane->IsVisible())
	{
		DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
		FolderNodePtr pWorkspace = ProductItemLoader::LoadProductTree(pMainDBCon);
		pAddPrtInstPane->DisplayPartList(m_pPartLayoutEditor, pParamConfig, pWorkspace);
	}
}

void SheetView::OnPartPmtAddSglDxf()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	// 准备数据。
	LONGLONG iParamConfigID = GetCurrentSheet()->GetParamConfigID();
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pExpLibDBCon, iParamConfigID);
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	FolderNodePtr pWorksapce = ProductItemLoader::LoadProductTree(pMainDBCon);

	// 选择要导入的dxf/dwg文件。
	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_DXFDWG_FILEEXT);
	ExtImpDataSelDlg impFileDlg(strDflExt, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter, this);
	impFileDlg.SetSavePath(pWorksapce, TRUE);
	impFileDlg.SetExpLibItem(iExpLibItemID, FALSE);
	impFileDlg.SetParamConfig(iParamConfigID, FALSE);
	if (impFileDlg.DoModal() == IDOK)
	{
		CString strFilePath = impFileDlg.GetPathName();
		CString strName = PathHelper::GetFileName(strFilePath);
		IMPEXP_DATA_TYPE impFileType = impFileDlg.GetImpType(strFilePath);
		FolderNodePtr pSaveFolder = impFileDlg.GetSavePath();
		LONGLONG iParamConfigID = impFileDlg.GetPartScheme();

        // 得到dxf data
        DxfDataPtr pDxfData = impFileDlg.GetDxfData();
        if (!pDxfData)
        {
            LogMgr::DebugWarn(_T("Debug warning 1 in OnPartPmtAddSglDxf function!"), this);
			return;
        }

		// 导入dxf/dwg文件为PartCadData对象，此时不要生成阵列回路，否则会影响拓扑关系识别。
		PartCadDataPtr pPartCadData = ImportManager::DxfDwg2CadData(strFilePath, pDxfData.get(), FALSE);

		// 利用“零件cad数据”和零件方案来构建零件对象。
		PartPtr pPart = PartBuilder::BuildPart(strName, pPartCadData, iParamConfigID, TRUE);
		PartItemPtr pPartItem(new PartItem(pPart->GetID(), strName, pPartCadData, pPart->GetCamDataList()->at(0)));

		// 启动action，在板材上面添加“零件项”。
		PartItemListPtr pPartItemList(new PartItemList);
		pPartItemList->push_back(pPartItem);
		map<LONGLONG, vector<Matrix2D>> matrixData_of_sameShapePart;
		m_pPartLayoutEditor->AddPartPmt(pPartItemList, matrixData_of_sameShapePart, pSaveFolder);
	}
}

void SheetView::OnPartPmtAddAsmDxf()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	// 准备数据。
	LONGLONG iParamConfigID = GetCurrentSheet()->GetParamConfigID();
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iExpLibItemID = ParamConfigLoader::GetExpLibItemID(pExpLibDBCon, iParamConfigID);
	DBConnectPtr pMainDBCon = DataBaseManager::GetInstance()->GetMainDBConnect();
	FolderNodePtr pWorksapce = ProductItemLoader::LoadProductTree(pMainDBCon);

	// 选择要导入的dxf/dwg文件。
	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMPORT_DXFDWG_FILEEXT);
	ExtImpDataSelDlg impFileDlg(strDflExt, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter, this);
	impFileDlg.SetSavePath(pWorksapce, TRUE);
	impFileDlg.SetExpLibItem(iExpLibItemID, FALSE);
	impFileDlg.SetParamConfig(iParamConfigID, FALSE);
	if (impFileDlg.DoModal() == IDOK)
	{
		CString strFilePath = impFileDlg.GetPathName();
		CString strName = PathHelper::GetFileName(strFilePath);
		IMPEXP_DATA_TYPE impFileType = impFileDlg.GetImpType(strFilePath);
		FolderNodePtr pSaveFolder = impFileDlg.GetSavePath();
		LONGLONG iParamConfigID = impFileDlg.GetPartScheme();

        // 得到dxf data
        DxfDataPtr pDxfData = impFileDlg.GetDxfData();
        if (!pDxfData)
        {
            LogMgr::DebugWarn(_T("Debug warning 1 in OnPartPmtAddAsmDxf function!"), this);
			return;
        }

		// 导入dxf/dwg文件为PartCadData对象，此时不要生成阵列回路，否则会影响拓扑关系识别。
		PartCadDataPtr pPartCadData = ImportManager::DxfDwg2CadData(strFilePath, pDxfData.get(), FALSE);

		LoopTopologyItemListPtr pLoopTopItemList = LoopProcessor::CalcLoopTopStruct(pPartCadData->GetPatternLoopList().get());
        if (!LoopProcessor::NeedToSeperate(pLoopTopItemList.get()))
		{
			// 利用“零件cad数据”和零件方案来构建零件对象。
			PartPtr pPart = PartBuilder::BuildPart(strName, pPartCadData, iParamConfigID, TRUE);
			PartItemPtr pPartItem(new PartItem(pPart->GetID(), strName, pPartCadData, pPart->GetCamDataList()->at(0)));

			// 启动action，在板材上面添加“零件项”。
			PartItemListPtr pPartItemList(new PartItemList);
			pPartItemList->push_back(pPartItem);
			map<LONGLONG, vector<Matrix2D>> matrixData_of_sameShapePart;
			m_pPartLayoutEditor->AddPartPmt(pPartItemList, matrixData_of_sameShapePart, pSaveFolder);
		}
		else
		{
			// 这个对话框用于从回路识别零件。
			Loop2PartDlg dlg;
			dlg.Init(pLoopTopItemList, strName, TRUE);
			int iRet = dlg.DoModal();
			if (iRet == IDC_BTN_ACCEPT)
			{
				// 得到识别出的零件集合。
				BOOL bRecognizeSamePart = false;
				vector<pair<CString, PatternLoopListPtr> > pPartList = dlg.GetRecognizedParts(bRecognizeSamePart);

				// 创建所有“零件项”。
				PartItemListPtr pPartItemList(new PartItemList);
				for (unsigned int i = 0; i < pPartList.size(); i++)
				{
					PatternListPtr pPatList = pPartList.at(i).second->GetPatList();
					PartCadDataPtr pPartCadData(new PartCadData(pPatList, pPartList.at(i).second));
					PartPtr pPart = PartBuilder::BuildPart(pPartList.at(i).first, pPartCadData, iParamConfigID, TRUE);
					PartItemPtr pPartItem(new PartItem(pPart->GetID(), pPart->GetName(), pPartCadData, pPart->GetCamDataList()->at(0)));
					pPartItemList->push_back(pPartItem);
				}

				// 检查这些零件中是不是有相同的零件，他们可以用相同的CAD数据和part id。
				map<LONGLONG, vector<Matrix2D>> matrixData_of_sameShapePart;
				PartItemListPtr pPartItemList_result;
				if (bRecognizeSamePart)
				{
					pPartItemList_result = PartManager::MergeCadData_of_sameShapePart(pPartItemList.get(), matrixData_of_sameShapePart);
				}
				else
				{
					pPartItemList_result = pPartItemList;
				}

				// 启动action，在板材上面添加“零件项”集合。
				m_pPartLayoutEditor->AddPartPmt(pPartItemList_result, matrixData_of_sameShapePart, pSaveFolder);
			}
		}
	}
}

void SheetView::OnPartPmtSelect()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(EDIT_SELECTPRTPMT);
}

void SheetView::OnZoomAll()
{
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, GetCurrentSheet()->GetMaterialSizeID());
	if (pMaterialSize)
	{
		// the drawing area.
		CRect wndRect;
		GetClientRect(&wndRect);

		// set the drawing area.
		double dLeftBottomPtX = .0, dLeftBottomPtY = .0;
		double dXDirRange = m_pViewPort->GetFitAllParam(wndRect.Width(), wndRect.Height(), 0, pMaterialSize->GetWidth(),
														0, pMaterialSize->GetHeight(), 1.1, dLeftBottomPtX, dLeftBottomPtY);
		m_pViewPort->SetDrawingArea(1.1 * dXDirRange, wndRect.Width(), wndRect.Height(), dLeftBottomPtX, dLeftBottomPtY);
	}

	Invalidate();
}

void SheetView::OnZoomIn()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetViewAction(ACTION_ZOOM_IN);

	Invalidate();
}

void SheetView::OnZoomOut()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetViewAction(ACTION_ZOOM_OUT);

	Invalidate();
}

void SheetView::OnPartPmtCopy()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(EDIT_COPYPRTPMT);
}

void SheetView::OnPartPmtDelete()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(EDIT_DELETEPARTPMT);
}

void SheetView::OnPartPmtRotate()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(EDIT_ROTATEPARTPMT);
}

void SheetView::OnPartPmtMove()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(EDIT_MOVEPRTPMT);
}

void SheetView::OnPartPmtGrid()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(EDIT_GRIDPRTPMT);
}

void SheetView::OnExplodeGrid()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(EDIT_EXPLODE_GRID);
}

void SheetView::OnCreatePartGroup()
{
    SwitchEditor(m_pPartLayoutEditor);
    HideLayoutEditorPane();
    m_pPartLayoutEditor->SetEditAction(EDIT_CreatePartGroup);
}

void SheetView::OnExplodePartGroup()
{
    SwitchEditor(m_pPartLayoutEditor);
    HideLayoutEditorPane();
    m_pPartLayoutEditor->SetEditAction(EDIT_ExplodePartGroup);
}

void SheetView::OnGenRemnantMat()
{
	RemnantMatInfoDlg dlg(this);
	SheetPtr pSheet = GetCurrentSheet();
	dlg.Init(pSheet);
	if (dlg.DoModal() == IDC_BTN_SAVE)
	{
		CString strProductName, str;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_REM_MAT_SAVED);
		MessageBox(str, strProductName, MB_OK | MB_ICONINFORMATION);
	}
}

void SheetView::OnMeasureDis()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(MEASURE_PT_DISTANCE);
}

void SheetView::OnShowPartPmtTreeView()
{
	if (!PartPlacementViewBar::GetInstance()->IsVisible())
	{
		SheetPtr pSheet = GetCurrentSheet();
		PartPlacementViewBar::GetInstance()->DisplayPartPmt(pSheet->GetPartPlacements(), pSheet->GetPartInstances(), m_pPartLayoutEditor);
	}
	else
		PartPlacementViewBar::GetInstance()->ShowBar(FALSE);
}

void SheetView::OnUpdatePartPmtTreeView(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(PartPlacementViewBar::GetInstance()->IsVisible());
}

void SheetView::OnShowPartTopTreeView()
{
	if (!PartTopologyBar::GetInstance()->IsVisible())
	{
		SheetPtr pSheet = GetCurrentSheet();

		// 计算零件实例中各个回路实例的外接矩形。
		PartInstanceListPtr pPartInstances = pSheet->GetPartInstances();
		map<LONGLONG, pair<Rect2D, map<LONGLONG,Rect2D>>> partRectMap = pPartInstances->GetLoopInstanceRect();

		// 计算出“零件实例”之间的拓扑关系。
		PartInstanceTopologyItemListPtr pPartInstanceTopologyItemList = PartPlacementManager::CalcPartTopStruct(pPartInstances, partRectMap);

		PartTopologyBar::GetInstance()->DisplayPartTop(pPartInstanceTopologyItemList, m_pPartLayoutEditor);
	}
	else
		PartTopologyBar::GetInstance()->ShowBar(FALSE);
}

void SheetView::OnUpdatePartTopTreeView(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(PartTopologyBar::GetInstance()->IsVisible());
}

void SheetView::OnViewOffsetLoop()
{
	if (m_pOffPartPlacementList)
	{
		m_pOffPartPlacementList.reset();
	}
	else
	{
		SheetPtr pSheet = GetCurrentSheet();

		// 得到激光宽度。
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pSheet->GetParamConfigID());
		ParamConfigItemPtr pParamConfigItem = pParamConfig->GetItemByType(PARAMCONFIG_ITEM_GENERATE_SEQ);
		SequenceConfigItemPtr pSeqConfigItem = boost::dynamic_pointer_cast<SequenceConfigItem>(pParamConfigItem);

		// 得到偏移后的“零件放置”集合。
		m_pOffPartPlacementList.reset(new PartPlacementList);
		PartInstanceListPtr pNewPartInsts(new PartInstanceList);
		PartPlacementManager::OffsetPartPmts(pSheet->GetPartPlacements().get(), pSheet->GetPartInstances().get(), m_pOffPartPlacementList, pNewPartInsts);
	}

	Invalidate(TRUE);
}

void SheetView::OnUpdateOffsetLoop(CCmdUI* pCmdUI)
{
	if (m_pOffPartPlacementList)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void SheetView::OnPartInstanceQuantity()
{
    SheetPtr pSheet = GetCurrentSheet();
    PartInstanceQuantityDlg dlg(pSheet, this);
    dlg.DoModal();
}

END_CUTLEADER_NAMESPACE()
