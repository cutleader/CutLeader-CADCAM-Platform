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
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	// ���ÿ����������á��еļӹ����յ������ԡ�
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

	// ��ʾ��Ϣ��
	CString str, strProductName;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_ASSIGN_CUTFEATURE);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	MessageBox(str, strProductName, MB_OK | MB_ICONINFORMATION);
}

void SheetView::OnResetCutFeature()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	CString str, strProductName;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_RESET_CUTFEATURE);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	if (MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		// ���ز������á�
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		SheetPtr pSheet = GetCurrentSheet();
		ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pSheet->GetParamConfigID());

		// ����ÿ����������á��еļӹ����ա�
		PartPlacementListPtr pPartPlacements = pSheet->GetPartPlacements();
		for (unsigned int i = 0; i < pPartPlacements->size(); i++)
		{
			PartPlacementPtr pPartPlacement = pPartPlacements->at(i);
			PartItemPtr pPartItem = pPartPlacement->GetPartItem();
			PartCadDataPtr pPartCadData = pPartItem->GetCadData();
			PartCamDataPtr pPartCamData = pPartItem->GetCamData();
			LoopCutFeatureManager::ResetLoopFeature(pPartCadData->GetPatternLoopList(), pPartCamData->GetLoopFeatureList(), pParamConfig.get(), pPartPlacement->GetMatrix());
		}

		// ���¹��򻺴档
		SheetSequenceDataPtr pSheetSequenceData = pSheet->GetSheetSequenceData();
		CutSequenceManager::UpdateCache(pSheetSequenceData->GetCutSequences());

		// ���µ�ǰ�����еĻ��档
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
	// ִ��һ�������������档

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

	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	Invalidate();
}

void SheetView::OnEditLoopTool()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(LOOPTOOL_ADJUST);
}

void SheetView::OnEditLoopLead()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(LEAD_ADJUST);
}

void SheetView::OnEditLoopStartPt()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(START_PT_ADJUST);
}

void SheetView::OnSwitchCutSide()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(CUTSIDE_ADJUST);
}

void SheetView::OnChangeDirection()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(DIRECTION_ADJUST);
}

void SheetView::OnAddMicrojoint()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_MJ_ADD);
}

void SheetView::OnEditMicrojoint()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_MJ_EDIT);
}

void SheetView::OnDelMicrojoint()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_MJ_DELETE);
}

void SheetView::OnAddCorner()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_CORNER_ADD);

	// ��ʾ�������������档
	DBConnectPtr pExpLibConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	CornerConfigItemPtr pCornerConfigItem = ConfigItemLoader::LoadCornerConfigItem(pExpLibConnect, GetCurrentSheet()->GetParamConfigID());
	CornerPropertyBar::GetInstance()->DisplayCornerData_4_create(pCornerConfigItem->GetDflCornerType(), pCornerConfigItem->GetDflCornerVal());
}

void SheetView::OnEditCorner()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_CORNER_EDIT);
}

void SheetView::OnDelCorner()
{
	// �л��༭����
	SwitchEditor(m_pPartLayoutEditor);

	// ���ء�������ֱ༭�����õ�����塣
	HideLayoutEditorPane();

	m_pPartLayoutEditor->SetEditAction(PMT_CORNER_DELETE);
}

END_CUTLEADER_NAMESPACE()
