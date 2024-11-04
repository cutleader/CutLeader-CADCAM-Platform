#include "StdAfx.h"
#include "CutFeatureEditor.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "IAction.h"
#include "DataBaseManager.h"

#include "PatternList.h"
#include "PatternLoopList.h"

#include "MicroJointList.h"
#include "CornerList.h"
#include "LoopCutFeatureList.h"
#include "LoopCutFeatureManager.h"
#include "MicroJointAddAction.h"
#include "MicroJointEditAction.h"
#include "MicroJointDelAction.h"
#include "MicroJointPropertyBar.h"
#include "CornerAddAction.h"
#include "CornerEditAction.h"
#include "CornerDeleteAction.h"
#include "CornerPropertyBar.h"
#include "LoopLeadEditAction.h"
#include "CutSideChangeAction.h"
#include "CutDirectionChangeAction.h"
#include "StartCutPtChangeAction.h"
#include "clCutFeatureResource.h"
#include "LoopLeadPropertyBar.h"
#include "LoopToolEditPanel.h"
#include "LoopToolEditAction.h"

#include "MachineItemLoader.h"
#include "ToolAssignConfigItem.h"
#include "LeadConfigItem.h"
#include "PierceConfigItem.h"
#include "MicroJointConfigItem.h"
#include "CornerConfigItem.h"
#include "ParamConfig.h"
#include "ExpertLibItemLoader.h"

BEGIN_CUTLEADER_NAMESPACE()

CutFeatureEditor::CutFeatureEditor(GlViewPortPtr pViewPort)
{
	m_pAction.reset();

	m_pViewPort = pViewPort;
}

CutFeatureEditor::~CutFeatureEditor(void)
{
}

BOOL CutFeatureEditor::LButtonDown(double& dPtX, double& dPtY)
{
	if(m_pAction)
	{
		m_pAction->LButtonDown(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL CutFeatureEditor::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_pAction)
	{
		m_pAction->LButtonUp(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL CutFeatureEditor::LButtonDBClk(double& dPtX, double& dPtY)
{
	if (m_pAction)
	{
		m_pAction->LButtonDBClk(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL CutFeatureEditor::RButtonUp(double& dPtX, double& dPtY)
{
	if (m_pAction)
	{
		m_pAction->RButtonUp(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL CutFeatureEditor::MovePoint(double& dPtX, double& dPtY)
{
	if (m_pAction)
	{
		m_pAction->MovePoint(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL CutFeatureEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pAction)
	{
		m_pAction->OnKeyDown(nChar, nRepCnt, nFlags);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;
	}

	UpdateOutputBar();

	return TRUE;
}

BOOL CutFeatureEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pAction)
	{
		m_pAction->OnKeyUp(nChar, nRepCnt, nFlags);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;
	}

	UpdateOutputBar();

	return TRUE;
}

CString CutFeatureEditor::GetEditorName()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE);
	return str;
}

BOOL CutFeatureEditor::SetAction(CUTFEATURE_ACTION_TYPE actionType)
{
	// load the condition type under the expert library item.
	LONGLONG iExpLibItemID = m_pParamConfig->GetParentID();
	DBConnectPtr pMacLibConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	DBConnectPtr pExpLibConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iMacItemID = ExpertLibItemLoader::GetMacIDByExpLibItem(pExpLibConnect, iExpLibItemID);
	ConditionTypeListPtr pCondTypeList = MachineItemLoader::LoadMacConds(pMacLibConnect, iMacItemID);

    ToolAssignConfigItemPtr pToolAssignConfigItem = boost::dynamic_pointer_cast<ToolAssignConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_TOOL_ASSIGN));
	LeadConfigItemPtr pLeadConfigItem = boost::dynamic_pointer_cast<LeadConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_LEAD_ASSIGN));
	PierceConfigItemPtr pPierceConfigItem = boost::dynamic_pointer_cast<PierceConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_PIERCE_ASSIGN));
	CornerConfigItemPtr pCornerConfigItem = boost::dynamic_pointer_cast<CornerConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_CORNER_ASSIGN));

	// hide all prop bar in this category.
    LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
	LoopLeadPropertyBar::GetInstance()->ShowBar(FALSE);
	CornerPropertyBar::GetInstance()->ShowBar(FALSE);
	MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);

    if (actionType == LOOPTOOL_EDIT)
    {
        LoopToolEditAction* pLoopToolEditAction = new LoopToolEditAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort, pToolAssignConfigItem, pLeadConfigItem, pCondTypeList);
        m_pAction.reset(pLoopToolEditAction);
    }
	else if (actionType == LEAD_EDIT)
	{
		LoopLeadEditAction* pLoopLeadEditAction = new LoopLeadEditAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort, pLeadConfigItem, pCondTypeList);
		m_pAction.reset(pLoopLeadEditAction);
	}
	else if (actionType == START_PT_CHANGE)
	{
		StartCutPtChangeAction* pStartPtChangeAction = new StartCutPtChangeAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort, pLeadConfigItem);
		m_pAction.reset(pStartPtChangeAction);
	}
	else if (actionType == DIRECTION_CHANGE)
	{
		CutDirectionChangeAction* pDirChangeAction = new CutDirectionChangeAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort);
		m_pAction.reset(pDirChangeAction);
	}
	else if (actionType == CUTSIDE_CHANGE)
	{
		CutSideChangeAction* pSideChangeAction = new CutSideChangeAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort, pLeadConfigItem);
		m_pAction.reset(pSideChangeAction);
	}
	else if (actionType == MicroJoint_ADD)
	{
		MicroJointAddAction* pMicroJointAddAction = new MicroJointAddAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort, m_pParamConfig);
		m_pAction.reset(pMicroJointAddAction);
	}
	else if (actionType == MicroJoint_EDIT)
	{
		MicroJointEditAction* pMicroJointEditAction = new MicroJointEditAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort);
		m_pAction.reset(pMicroJointEditAction);
	}
	else if (actionType == MicroJoint_DELETE)
	{
		// ������ǰ��û��ѡ�е�΢����������
		MicroJointListPtr pMicroJointList;
		if (m_pAction->GetActionType() == MicroJoint_EDIT)
		{
			MicroJointEditAction* pMJEditAction = dynamic_cast<MicroJointEditAction*>(m_pAction.get());
			pMicroJointList = pMJEditAction->GetSelectMJList();
		}

		// �л���΢����ɾ��������
		MicroJointDelAction* pMJDelAction = new MicroJointDelAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort);
		m_pAction.reset(pMJDelAction);

		// Ҫ����ѡ�е�΢����������ɾ����
		if (pMicroJointList && pMicroJointList->size() > 0)
		{
			MicroJointListPtr pDelMJList(new MicroJointList);
			pDelMJList->insert(pDelMJList->end(), pMicroJointList->begin(), pMicroJointList->end());
			pMJDelAction->SelectMJList(pDelMJList);
		}
	}
	else if (actionType == CORNER_ADD)
	{
		m_pAction.reset(new CornerAddAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort));
	}
	else if (actionType == CORNER_EDIT)
	{
		CornerEditAction* pCornerEditAction = new CornerEditAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort, pCornerConfigItem);
		m_pAction.reset(pCornerEditAction);
	}
	else if (actionType == CORNER_DELETE)
	{
		// ������ǰ��û��ѡ�еĽ�������
		CornerListPtr pCornerList;
		if (m_pAction->GetActionType() == CORNER_EDIT)
		{
			CornerEditAction* pCornerEditAction = dynamic_cast<CornerEditAction*>(m_pAction.get());
			pCornerList = pCornerEditAction->GetSelectCornerList();
		}

		// �л���������ɾ��������
		CornerDeleteAction* pCornerDelAction = new CornerDeleteAction(m_pPatList, m_pPatternLoopList, m_pLoopFeatureList, m_pViewPort);
		m_pAction.reset(pCornerDelAction);

		// Ҫ����ѡ�еĽ�������ɾ����
		if (pCornerList && pCornerList->size() > 0)
		{
			CornerListPtr pDelCornerList(new CornerList);
			pDelCornerList->insert(pDelCornerList->end(), pCornerList->begin(), pCornerList->end());
			pCornerDelAction->SelectCornerList(pDelCornerList);
		}
	}

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
