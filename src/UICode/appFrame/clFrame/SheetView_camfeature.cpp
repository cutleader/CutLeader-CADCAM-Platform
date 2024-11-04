#include "StdAfx.h"
#include "SheetView.h"

#include "DBConnect.h"
#include "DataBaseManager.h"
#include "ProgressData.h"
#include "ProgressDlg.h"
#include "IAction.h"
#include "clBaseDataResource.h"

#include "CornerPropertyBar.h"
#include "LoopCutFeatureManager.h"
#include "clCutFeatureResource.h"

#include "PartCadData.h"
#include "PartCamData.h"
#include "PartItem.h"

#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartPlacementManager.h"
#include "clPartLayoutResource.h"
#include "PartLayoutEditor.h"
#include "InterfereCheckTaskEx.h"
#include "SheetSequenceData.h"
#include "CutSequenceManager.h"
#include "Sheet.h"

#include "CornerConfigItem.h"
#include "ConfigItemLoader.h"
#include "ParamConfigLoader.h"

BEGIN_CUTLEADER_NAMESPACE()

void SheetView::OnAssignCutFeature()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	// 检查每个“零件放置”中的加工工艺的完整性。
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	SheetPtr pSheet = GetCurrentSheet();
	ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pSheet->GetParamConfigID());
	PartPlacementListPtr pPartPlacements = pSheet->GetPartPlacements();
	for (unsigned int i = 0; i < pPartPlacements->size(); i++)
	{
		PartItemPtr pPartItem = pPartPlacements->at(i)->GetPartItem();
		PartCadDataPtr pPartCadData = pPartItem->GetCadData();
		PartCamDataPtr pPartCamData = pPartItem->GetCamData();
		LoopCutFeatureManager::CheckIntegrity(pPartCadData->GetPatternLoopList().get(), pPartCamData->GetLoopFeatureList(), pParamConfig.get(), FALSE, TRUE, TRUE, pPartPlacements->at(i)->GetMatrix());
	}

	// 提示信息。
	CString str, strProductName;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_ASSIGN_CUTFEATURE);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	MessageBox(str, strProductName, MB_OK | MB_ICONINFORMATION);
}

void SheetView::OnResetCutFeature()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	CString str, strProductName;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_RESET_CUTFEATURE);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	if (MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		// 加载参数配置。
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		SheetPtr pSheet = GetCurrentSheet();
		ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pSheet->GetParamConfigID());

		// 重置每个“零件放置”中的加工工艺。
		PartPlacementListPtr pPartPlacements = pSheet->GetPartPlacements();
		for (unsigned int i = 0; i < pPartPlacements->size(); i++)
		{
			PartPlacementPtr pPartPlacement = pPartPlacements->at(i);
			PartItemPtr pPartItem = pPartPlacement->GetPartItem();
			PartCadDataPtr pPartCadData = pPartItem->GetCadData();
			PartCamDataPtr pPartCamData = pPartItem->GetCamData();
			LoopCutFeatureManager::ResetLoopFeature(pPartCadData->GetPatternLoopList(), pPartCamData->GetLoopFeatureList(), pParamConfig.get(), pPartPlacement->GetMatrix());
		}

		// 更新工序缓存。
		SheetSequenceDataPtr pSheetSequenceData = pSheet->GetSheetSequenceData();
		CutSequenceManager::UpdateCache(pSheetSequenceData->GetCutSequences());

		// 更新当前动作中的缓存。
		if (m_pCurEditor)
		{
			IActionPtr pCurAction = m_pCurEditor->GetActionObject();
			if (pCurAction)
				pCurAction->UpdateActionBuffer();
		}
	}
}

void SheetView::OnCheckInterfere()
{
	/************************************************************************/
	// 执行一个任务来检测干涉。

	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();

	// init the task.
	SheetPtr pSheet = GetCurrentSheet();
	PartInstanceListPtr pPartInstances = pSheet->GetPartInstances();
	PartPlacementListPtr pPartPlacements = pSheet->GetPartPlacements();
	ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pSheet->GetParamConfigID());
	InterfereCheckTaskExPtr pIntfCheckTaskEx(new InterfereCheckTaskEx(pPartPlacements, pPartInstances, pParamConfig));

	// display the progress of the task.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_INTF_CHECK_TASK_EX);
	ProgressDlg progressDlg;
	progressDlg.Init(str, pIntfCheckTaskEx);
	progressDlg.DoModal();
	/************************************************************************/

	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	Invalidate();
}

void SheetView::OnEditLoopTool()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(LOOPTOOL_ADJUST);
}

void SheetView::OnEditLoopLead()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(LEAD_ADJUST);
}

void SheetView::OnEditLoopStartPt()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(START_PT_ADJUST);
}

void SheetView::OnSwitchCutSide()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(CUTSIDE_ADJUST);
}

void SheetView::OnChangeDirection()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(DIRECTION_ADJUST);
}

void SheetView::OnAddMicrojoint()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_MJ_ADD);
}

void SheetView::OnEditMicrojoint()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_MJ_EDIT);
}

void SheetView::OnDelMicrojoint()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_MJ_DELETE);
}

void SheetView::OnAddCorner()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_CORNER_ADD);

	// 显示角特征创建界面。
	DBConnectPtr pExpLibConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	CornerConfigItemPtr pCornerConfigItem = ConfigItemLoader::LoadCornerConfigItem(pExpLibConnect, GetCurrentSheet()->GetParamConfigID());
	CornerPropertyBar::GetInstance()->DisplayCornerData_4_create(pCornerConfigItem->GetDflCornerType(), pCornerConfigItem->GetDflCornerVal());
}

void SheetView::OnEditCorner()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_CORNER_EDIT);
}

void SheetView::OnDelCorner()
{
	// 切换编辑器。
	SwitchEditor(m_pPartLayoutEditor);

	// 隐藏“零件布局编辑器”用到的面板。
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_CORNER_DELETE);
}

END_CUTLEADER_NAMESPACE()
