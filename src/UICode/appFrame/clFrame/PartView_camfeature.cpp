#include "StdAfx.h"
#include "PartView.h"

#include "UIManager.h"
#include "InteractBar.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"
#include "ProgressData.h"
#include "ProgressDlg.h"

#include "CutFeatureEditor.h"
#include "CornerPropertyBar.h"
#include "clCutFeatureResource.h"
#include "LoopCutFeatureManager.h"
#include "LoopCutFeatureList.h"
#include "InterfereCheckTask.h"

#include "PartCadData.h"
#include "PartItem.h"
#include "PartCamData.h"

#include "ConditionType.h"
#include "ConditionTypeList.h"
#include "CornerConfigItem.h"
#include "MachineItemLoader.h"
#include "ExpertLibItemLoader.h"
#include "ParamConfig.h"
#include "ConfigItemLoader.h"
#include "ParamConfigLoader.h"

BEGIN_CUTLEADER_NAMESPACE()

void PartView::OnAssignCutFeature()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);
	m_pCutFeatureEditor->SetDummyAction();

	// ���������ӹ����ա�
	DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	PartItemPtr pPartItem = GetCurrentPart();
	PartCadDataPtr pPartCadData = pPartItem->GetCadData();
	PartCamDataPtr pPartCamData = pPartItem->GetCamData();
	ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pPartCamData->GetParamConfigID());
	LoopCutFeatureManager::CheckIntegrity(pPartCadData->GetPatternLoopList().get(), pPartCamData->GetLoopFeatureList(), pParamConfig.get(), FALSE, TRUE, TRUE, Matrix2D());

	// show message to user.
	CString str, strProductName;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_ASSIGN_CUTFEATURE);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	MessageBox(str, strProductName, MB_OK | MB_ICONINFORMATION);
}

void PartView::OnResetCutFeature()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);
	m_pCutFeatureEditor->SetDummyAction();

	CString str, strProductName;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_RESET_CUTFEATURE);
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	if (MessageBox(str, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		PartItemPtr pPartItem = GetCurrentPart();
		PartCadDataPtr pPartCadData = pPartItem->GetCadData();
		PartCamDataPtr pPartCamData = pPartItem->GetCamData();
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pPartCamData->GetParamConfigID());
		LoopCutFeatureManager::ResetLoopFeature(pPartCadData->GetPatternLoopList(), pPartCamData->GetLoopFeatureList(), pParamConfig.get(), Matrix2D());

		// notify all observers.
		pPartCamData->GetLoopFeatureList()->NotifyAllObserver(INVALID_ID, VARIANT());
	}
}

void PartView::OnCheckInterfere()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);
	m_pCutFeatureEditor->SetDummyAction();

	/************************************************************************/
	// create a task to auto sequence.

	// init the task.
	PartCamDataPtr pPartCamData = GetCurrentPart()->GetCamData();
	LoopCutFeatureListPtr pLoopFeatureList = pPartCamData->GetLoopFeatureList();
	InterfereCheckTaskPtr pIntfCheckTask(new InterfereCheckTask(pLoopFeatureList));

	// display the progress of the task.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_INTF_CHECK_TASK);
	ProgressDlg progressDlg;
	progressDlg.Init(str, pIntfCheckTask);
	progressDlg.DoModal();
	/************************************************************************/

	Invalidate();
}

void PartView::OnEditLoopTool()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(LOOPTOOL_EDIT);
}

void PartView::OnEditLoopLead()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(LEAD_EDIT);
}

void PartView::OnEditLoopStartPt()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(START_PT_CHANGE);
}

void PartView::OnSwitchCutSide()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(CUTSIDE_CHANGE);
}

void PartView::OnChangeDirection()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(DIRECTION_CHANGE);
}

void PartView::OnAddMicrojoint()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(MicroJoint_ADD);
}

void PartView::OnEditMicrojoint()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(MicroJoint_EDIT);
}

void PartView::OnDeleteMicrojoint()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(MicroJoint_DELETE);
}

void PartView::OnAddCornerFeature()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(CORNER_ADD);

	// ��ʾ�������������档
	PartCamDataPtr pPartCamData = GetCurrentPart()->GetCamData();
	DBConnectPtr pExpLibConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	CornerConfigItemPtr pCornerConfigItem = ConfigItemLoader::LoadCornerConfigItem(pExpLibConnect, pPartCamData->GetParamConfigID());
	CornerPropertyBar::GetInstance()->DisplayCornerData_4_create(pCornerConfigItem->GetDflCornerType(), pCornerConfigItem->GetDflCornerVal());
}

void PartView::OnEditCornerFeature()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(CORNER_EDIT);
}

void PartView::OnDeleteCornerFeature()
{
	// �л��༭����
	SwitchEditor(m_pCutFeatureEditor);

	m_pCutFeatureEditor->SetAction(CORNER_DELETE);
}

END_CUTLEADER_NAMESPACE()
