#include "StdAfx.h"
#include "PartLayoutEditor.h"

#include "GlViewPort.h"
#include "ZoomInAction.h"
#include "ZoomOutAction.h"
#include "DummyAction.h"
#include "clBaseDataResource.h"
#include "DataBaseManager.h"
#include "LogMgr.h"
#include "ValueInputManager.h"

#include "MachineItemLoader.h"
#include "ToolAssignConfigItem.h"
#include "LeadConfigItem.h"
#include "PierceConfigItem.h"
#include "MicroJointConfigItem.h"
#include "CornerConfigItem.h"
#include "ParamConfig.h"
#include "ExpertLibItemLoader.h"
#include "ClOptionInfo.h"
#include "SheetEditParam.h"

#include "MicroJointList.h"
#include "clPartLayoutResource.h"
#include "PartPlacementPropertyBar.h"
#include "PartPlacementAddAction.h"
#include "PartPlacementAddAction2.h"
#include "PartPlacementList.h"
#include "PartPlacementSelectAction.h"
#include "PartInstanceSelectAction.h"
#include "PartPlacementCopyAction.h"
#include "PartPlacementRotateAction.h"
#include "PartPlacementMoveAction.h"
#include "PartPlacementGridAction.h"
#include "PartPlacementExplodeAction.h"
#include "PartPlacementDeleteAction.h"
#include "LoopToolEditAction2.h"
#include "CutSideAdjustAction.h"
#include "CutDirectionAdjustAction.h"
#include "MicroJointAddAction.h"
#include "LoopLeadEditAction2.h"
#include "StartCutPtAdjustAction.h"
#include "MicroJointEditAction2.h"
#include "MicroJointDeleteAction2.h"
#include "CornerAddAction2.h"
#include "CornerEditAction2.h"
#include "CornerDeleteAction2.h"
#include "DistanceMeasureAction.h"
#include "PartLayoutHelper.h"
#include "PartGroupList.h"
#include "PartPlacementManager.h"
#include "LoopInstanceSelectAction.h"
#include "LoopInstanceList.h"
#include "LoopInstance.h"
#include "PartPlacement.h"
#include "PartInstanceList.h"
#include "PartInstance.h"
#include "LoopInstanceManager.h"
#include "clCutFeatureResource.h"


BEGIN_CUTLEADER_NAMESPACE()

PartLayoutEditor::PartLayoutEditor(GlViewPortPtr pViewPort)
{
	m_pViewPort = pViewPort;
	m_pAction.reset(new DummyAction(m_pViewPort));
}

PartLayoutEditor::~PartLayoutEditor(void)
{
    // 这里先销毁action，因为有的action可能会监听m_pSpreadSamePartDlg。
    m_pAction.reset();
    if (m_pSpreadSamePartDlg)
    {
        delete m_pSpreadSamePartDlg;
        m_pSpreadSamePartDlg = NULL;
    }
}

BOOL PartLayoutEditor::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pAction)
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

BOOL PartLayoutEditor::LButtonUp(double& dPtX, double& dPtY)
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

BOOL PartLayoutEditor::LButtonDBClk(double& dPtX, double& dPtY)
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

BOOL PartLayoutEditor::RButtonUp(double& dPtX, double& dPtY)
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

BOOL PartLayoutEditor::MovePoint(double& dPtX, double& dPtY)
{
	if (m_pAction)
	{
        // 设置该对话框的显示和隐藏时机。
        if (m_pAction->GetActionType() == EDIT_SELECTPRTPMT)
        {
            PartPlacementSelectActionPtr pPartPmtSelectAction = boost::dynamic_pointer_cast<PartPlacementSelectAction>(m_pAction);
            if (!pPartPmtSelectAction->IsCopying())
            {
                ShowSelectPartOrLoopActionBar();
            }
            else
            {
                HideSelectPartOrLoopActionBar();
            }
        }
        else if (m_pAction->GetActionType() == EDIT_SelectLoopInstance)
        {
            ShowSelectPartOrLoopActionBar();
        }
        else
        {
            HideSelectPartOrLoopActionBar();
        }

		// 设置“智能靠碰参数”对话框的显示和隐藏时机。
		SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
		if (pSheetEditParam->EnableDisDetect())
		{
			if (m_pAction->GetActionType() == EDIT_MOVEPRTPMT)
			{
				PartPlacementMoveActionPtr pPartPmtMoveAction = boost::dynamic_pointer_cast<PartPlacementMoveAction>(m_pAction);
				if (pPartPmtMoveAction->GetSelectedPrtPmts()->size() > 0)
				{
					ShowDisDetectDlg(pPartPmtMoveAction.get());
				}
				else
				{
					HideDisDetectDlg();
				}
			}
			else if (m_pAction->GetActionType() == EDIT_COPYPRTPMT)
			{
				PartPlacementCopyActionPtr pPartPmtCopyAction = boost::dynamic_pointer_cast<PartPlacementCopyAction>(m_pAction);
				if (pPartPmtCopyAction->GetSelectedPrtPmts()->size() > 0)
				{
					ShowDisDetectDlg(pPartPmtCopyAction.get());
				}
				else
				{
					HideDisDetectDlg();
				}
			}
			else if (m_pAction->GetActionType() == EDIT_SELECTPRTPMT)
			{
				PartPlacementSelectActionPtr pPartPmtSelectAction = boost::dynamic_pointer_cast<PartPlacementSelectAction>(m_pAction);
				if (pPartPmtSelectAction->IsCopying())
				{
					ShowDisDetectDlg(pPartPmtSelectAction.get());
				}
				else
				{
					HideDisDetectDlg();
				}
			}
            else if (m_pAction->GetActionType() == EDIT_SelectLoopInstance)
            {
                ShowSelectPartOrLoopActionBar();
            }
			else if (m_pAction->GetActionType() == EDIT_ADDPARTPMT)
			{
				PartPlacementAddActionPtr pPartPmtAddAction = boost::dynamic_pointer_cast<PartPlacementAddAction>(m_pAction);
				ShowDisDetectDlg(pPartPmtAddAction.get());
			}
			else if (m_pAction->GetActionType() == EDIT_ADDPARTPMT_EX)
			{
				PartPlacementAddAction2Ptr pPartPmtAddActionEx = boost::dynamic_pointer_cast<PartPlacementAddAction2>(m_pAction);
				ShowDisDetectDlg(pPartPmtAddActionEx.get());
			}
			else
			{
				HideDisDetectDlg();
			}
		}

		// 设置“传播起切点”对话框的显示和隐藏时机。
		if (m_pAction->GetActionType() == START_PT_ADJUST || m_pAction->GetActionType() == LOOPTOOL_ADJUST ||
			m_pAction->GetActionType() == PMT_MJ_ADD || m_pAction->GetActionType() == PMT_MJ_EDIT || m_pAction->GetActionType() == PMT_MJ_DELETE ||
			m_pAction->GetActionType() == PMT_CORNER_ADD || m_pAction->GetActionType() == PMT_CORNER_EDIT || m_pAction->GetActionType() == PMT_CORNER_DELETE ||
			m_pAction->GetActionType() == DIRECTION_ADJUST || m_pAction->GetActionType() == CUTSIDE_ADJUST || m_pAction->GetActionType() == LEAD_ADJUST)
		{
			ShowSpreadSamePartDlg();
		}
		else
		{
			HideSpreadSamePartDlg();
		}

		// 设置“定义零件阵列”对话框的显示和隐藏时机。
		if (m_pAction->GetActionType() != EDIT_GRIDPRTPMT)
		{
			HidePartGridDlg();
		}

		m_pAction->MovePoint(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
        {
			m_pAction = pNextAction;
        }

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL PartLayoutEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pAction)
	{
        m_pAction->OnKeyDown(nChar, nRepCnt, nFlags);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
        {
			m_pAction = pNextAction;
        }
	}

	UpdateOutputBar();

	return TRUE;
}

BOOL PartLayoutEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pAction)
	{
		m_pAction->OnKeyUp(nChar, nRepCnt, nFlags);

		UpdateOutputBar();
	}

	return TRUE;
}

CString PartLayoutEditor::GetEditorName()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT);
	return str;
}

PartPlacementListPtr PartLayoutEditor::GetSelectedPartPlacements()
{
    PartPlacementListPtr pSelectedPartPlacements(new PartPlacementList);

    if (const PartPlacementSelectAction* pPartPlacementSelectAction = dynamic_cast<const PartPlacementSelectAction*>(m_pAction.get()))
    {
        pSelectedPartPlacements = pPartPlacementSelectAction->GetSelectedPrtPmts();
    }
    else if (const LoopInstanceSelectAction* pLoopInstanceSelectAction = dynamic_cast<const LoopInstanceSelectAction*>(m_pAction.get()))
    {
        LoopInstanceListPtr pSelectedLoopInstances = pLoopInstanceSelectAction->GetSelectedLoopInstances();
        for (unsigned int i = 0; i < pSelectedLoopInstances->size(); i++)
        {
            LoopInstancePtr pLoopInstance = pSelectedLoopInstances->operator [](i);
            PartPlacementPtr pPartPlacement = pLoopInstance->GetPartInstance()->GetPartPlacement();
            if (!pSelectedPartPlacements->GetPartPlacementByID(pPartPlacement->GetID()))
            {
                pSelectedPartPlacements->push_back(pPartPlacement);
            }
        }
    }

    return pSelectedPartPlacements;
}

LoopInstanceListPtr PartLayoutEditor::GetSelectedLoopInstances()
{
    LoopInstanceListPtr pSelectedLoopInstances(new LoopInstanceList);

    if (const PartPlacementSelectAction* pPartPlacementSelectAction = dynamic_cast<const PartPlacementSelectAction*>(m_pAction.get()))
    {
        PartPlacementListPtr pSelectedPartPlacements = pPartPlacementSelectAction->GetSelectedPrtPmts();
        for (unsigned int i = 0; i < pSelectedPartPlacements->size(); i++)
        {
            PartPlacementPtr pPartPlacement = pSelectedPartPlacements->operator [](i);
            PartInstanceListPtr pPartInstances = m_pPartInstanceList->GetPartInstanceByPartPlacement(pPartPlacement.get());
            LoopInstanceListPtr pLoopInstances = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
            pSelectedLoopInstances->insert(pSelectedLoopInstances->end(), pLoopInstances->begin(), pLoopInstances->end());
        }
    }
    else if (const LoopInstanceSelectAction* pLoopInstanceSelectAction = dynamic_cast<const LoopInstanceSelectAction*>(m_pAction.get()))
    {
        pSelectedLoopInstances = pLoopInstanceSelectAction->GetSelectedLoopInstances();
    }

    return pSelectedLoopInstances;
}

void PartLayoutEditor::StartPartPlacementSelectAction_andPreSelect(PartPlacementListPtr pPartPlacements_preSelect)
{
    PartPlacementSelectAction* pPartPlacementSelectAction = new PartPlacementSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups,
        m_pViewPort, m_pDisDetectDlg, m_pSelectPartOrLoopActionBar);
    pPartPlacementSelectAction->SelectPartPlacements(pPartPlacements_preSelect, TRUE);
    m_pAction.reset(pPartPlacementSelectAction);
}

BOOL PartLayoutEditor::SetEditAction(EDIT_SHEET_ACTION_TYPE actionType)
{
    CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// load the condition type under the expert library item.
	LONGLONG iExpLibItemID = m_pParamConfig->GetParentID();
	DBConnectPtr pMacLibConnect = DataBaseManager::GetInstance()->GetMacLibDBConnect();
	DBConnectPtr pExpLibConnect = DataBaseManager::GetInstance()->GetExpLibDBConnect();
	LONGLONG iMacItemID = ExpertLibItemLoader::GetMacIDByExpLibItem(pExpLibConnect, iExpLibItemID);
	ConditionTypeListPtr pCondTypeList = MachineItemLoader::LoadMacConds(pMacLibConnect, iMacItemID);

	//
	ToolAssignConfigItemPtr pToolAssignConfigItem = boost::dynamic_pointer_cast<ToolAssignConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_TOOL_ASSIGN));
	LeadConfigItemPtr pLeadConfigItem = boost::dynamic_pointer_cast<LeadConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_LEAD_ASSIGN));
	PierceConfigItemPtr pPierceConfigItem = boost::dynamic_pointer_cast<PierceConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_PIERCE_ASSIGN));
	CornerConfigItemPtr pCornerConfigItem = boost::dynamic_pointer_cast<CornerConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_CORNER_ASSIGN));

	if (actionType == EDIT_GRIDPRTPMT)
	{
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();
        if (pSelectedPartPlacements->size() == 1 && !pSelectedPartPlacements->operator [](0)->IsGrid()) // 选中了一个非阵列零件
        {
            PartPlacementGridAction* pPartPlacementGridAction = new PartPlacementGridAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups,
                m_pViewPort, m_pCreatePartGridDlg);
            pPartPlacementGridAction->CreateGridFromPartPlacement(pSelectedPartPlacements->operator [](0));
            m_pAction.reset(pPartPlacementGridAction);
        }
        else
        {
		    m_pAction.reset(new PartPlacementGridAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pCreatePartGridDlg));
        }
	}
	else if (actionType == EDIT_EXPLODE_GRID)
	{
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();
        if (pSelectedPartPlacements->size() == 1 && pSelectedPartPlacements->operator [](0)->IsGrid()) // 选中了一个阵列零件
        {
            PartPlacementExplodeAction* pPartPlacementExplodeAction = new PartPlacementExplodeAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort);
            pPartPlacementExplodeAction->ExplodeGridPartPlacement(pSelectedPartPlacements->operator [](0));
            m_pAction.reset(pPartPlacementExplodeAction);
        }
        else
        {
		    m_pAction.reset(new PartPlacementExplodeAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort));
        }
	}
    else if (actionType == EDIT_SelectPartOrLoopInstance)
    {
        if (m_pSelectPartOrLoopActionBar->GetIsSelectingLoopInstance())
        {
            m_pAction.reset(new LoopInstanceSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSelectPartOrLoopActionBar));
        }
        else
        {
            m_pAction.reset(new PartPlacementSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pDisDetectDlg, m_pSelectPartOrLoopActionBar));
        }
    }
	else if (actionType == EDIT_SELECTPRTPMT)
    {
		m_pAction.reset(new PartPlacementSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pDisDetectDlg, m_pSelectPartOrLoopActionBar));
    }
    else if (actionType == EDIT_SelectLoopInstance)
    {
        m_pAction.reset(new LoopInstanceSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSelectPartOrLoopActionBar));
    }
	else if (actionType == EDIT_SELECTPRTINST)
    {
		m_pAction.reset(new PartInstanceSelectAction(m_pViewPort));
    }
	else if (actionType == EDIT_DELETEPARTPMT)
	{
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();
		PartPlacementDeleteActionPtr pPartPmtDelAction(new PartPlacementDeleteAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
            m_pPartGroups, pSelectedPartPlacements, m_pViewPort, m_pDisDetectDlg));
		pPartPmtDelAction->DeleteSelectedPartPlacements();
		m_pAction = pPartPmtDelAction;
	}
	else if(actionType == EDIT_ROTATEPARTPMT)
	{
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();
		m_pAction.reset(new PartPlacementRotateAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, pSelectedPartPlacements, m_pViewPort, m_pDisDetectDlg));
	}
	else if(actionType == EDIT_MOVEPRTPMT)
	{
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();
		m_pAction.reset(new PartPlacementMoveAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, pSelectedPartPlacements, m_pViewPort, m_pDisDetectDlg));
	}
	else if(actionType == EDIT_COPYPRTPMT)
	{
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();
		m_pAction.reset(new PartPlacementCopyAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, pSelectedPartPlacements, m_pViewPort, m_pDisDetectDlg));
	}
	else if (actionType == LOOPTOOL_ADJUST)
	{
		LoopToolEditAction2* pLoopToolEditAction = new LoopToolEditAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
            m_pPartGroups, m_pViewPort, pToolAssignConfigItem, pLeadConfigItem, pCondTypeList, m_pSpreadSamePartDlg);
        LoopInstanceListPtr pSelectedLoopInstances = GetSelectedLoopInstances();
        pLoopToolEditAction->EditToolsOfSelectedLoopInstances(pSelectedLoopInstances);
		m_pAction.reset(pLoopToolEditAction);
	}
	else if (actionType == LEAD_ADJUST)
	{
		LoopLeadEditAction2* pLoopLeadEditAction2 = new LoopLeadEditAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort,
            pLeadConfigItem, pCondTypeList, m_pSpreadSamePartDlg);
        LoopInstanceListPtr pSelectedLoopInstances = GetSelectedLoopInstances();
        pLoopLeadEditAction2->EditLeadsOfSelectedLoopInstances(pSelectedLoopInstances);
        m_pAction.reset(pLoopLeadEditAction2);
	}
	else if (actionType == START_PT_ADJUST)
	{
        LoopInstanceListPtr pSelectedLoopInstances = GetSelectedLoopInstances();
        if (pSelectedLoopInstances->size() > 1) // 选了超过一个轮廓实例
        {
            pSelectedLoopInstances->clear();
        }
        if (pSelectedLoopInstances->size() == 1)
        {
		    StartCutPtAdjustAction* pStartCutPtAdjustAction = new StartCutPtAdjustAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, pLeadConfigItem, m_pSpreadSamePartDlg);
            pStartCutPtAdjustAction->EditStartCutPoint_OfSelectedLoopInstance(pSelectedLoopInstances->front());
            m_pAction.reset(pStartCutPtAdjustAction);
        }
        else
        {
            m_pAction.reset(new StartCutPtAdjustAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, pLeadConfigItem, m_pSpreadSamePartDlg));
        }
	}
	else if (actionType == CUTSIDE_ADJUST)
	{
        LoopInstanceListPtr pSelectedLoopInstances = GetSelectedLoopInstances();
        if (pSelectedLoopInstances->size() > 1) // 选了超过一个轮廓实例
        {
            pSelectedLoopInstances->clear();
        }
        if (pSelectedLoopInstances->size() == 1)
        {
            CutSideAdjustAction* pCutSideAdjustAction = new CutSideAdjustAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, pLeadConfigItem, m_pSpreadSamePartDlg);
            pCutSideAdjustAction->ChangeCutSide_OfSelectedLoopInstance(pSelectedLoopInstances->front());
            m_pAction.reset(pCutSideAdjustAction);
        }
        else
        {
		    m_pAction.reset(new CutSideAdjustAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, pLeadConfigItem, m_pSpreadSamePartDlg));
        }
	}
	else if (actionType == DIRECTION_ADJUST)
	{
        LoopInstanceListPtr pSelectedLoopInstances = GetSelectedLoopInstances();
        if (pSelectedLoopInstances->size() > 1) // 选了超过一个轮廓实例
        {
            pSelectedLoopInstances->clear();
        }
        if (pSelectedLoopInstances->size() == 1)
        {
            CutDirectionAdjustAction* pCutDirectionAdjustAction = new CutDirectionAdjustAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, m_pSpreadSamePartDlg);
            pCutDirectionAdjustAction->ChangeCutDirection_OfSelectedLoopInstance(pSelectedLoopInstances->front());
            m_pAction.reset(pCutDirectionAdjustAction);
        }
        else
        {
            m_pAction.reset(new CutDirectionAdjustAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSpreadSamePartDlg));
        }
	}
	else if (actionType == PMT_MJ_ADD)
	{
        LoopInstanceListPtr pSelectedLoopInstances = GetSelectedLoopInstances();
        if (pSelectedLoopInstances->size() > 1) // 选了超过一个轮廓实例
        {
            pSelectedLoopInstances->clear();
        }
        if (pSelectedLoopInstances->size() == 1)
        {
            MicroJointAddAction* pMicroJointAddAction = new MicroJointAddAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, m_pParamConfig, m_pSpreadSamePartDlg);
            pMicroJointAddAction->AddMicroJointOnSelectedLoopInstance(pSelectedLoopInstances->front());
            m_pAction.reset(pMicroJointAddAction);
        }
        else
        {
            m_pAction.reset(new MicroJointAddAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pParamConfig, m_pSpreadSamePartDlg));
        }
	}
	else if (actionType == PMT_MJ_EDIT)
	{
		m_pAction.reset(new MicroJointEditAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSpreadSamePartDlg));
	}
	else if (actionType == PMT_MJ_DELETE)
	{
		// 看看当前有没有选中的微连接特征。
		MicroJointListPtr pMicroJointList;
		if (m_pAction->GetActionType() == MicroJoint_EDIT)
		{
			MicroJointEditAction2* pMJEditAction = dynamic_cast<MicroJointEditAction2*>(m_pAction.get());
			pMicroJointList = pMJEditAction->GetSelectMJList();
		}

		// 切换到微连接删除动作。
		MicroJointDeleteAction2* pMJDelAction = new MicroJointDeleteAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSpreadSamePartDlg);
		m_pAction.reset(pMJDelAction);

		// 要是有选中的微连接特征，删除。
		if (pMicroJointList && pMicroJointList->size() > 0)
		{
			MicroJointListPtr pDelMJList(new MicroJointList);
			pDelMJList->insert(pDelMJList->end(), pMicroJointList->begin(), pMicroJointList->end());
			pMJDelAction->SelectMJList(pDelMJList);
		}
	}
	else if (actionType == PMT_CORNER_ADD)
	{
		m_pAction.reset(new CornerAddAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSpreadSamePartDlg));
	}
	else if (actionType == PMT_CORNER_EDIT)
	{
		m_pAction.reset(new CornerEditAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSpreadSamePartDlg,
            pCornerConfigItem));
	}
	else if (actionType == PMT_CORNER_DELETE)
	{
		m_pAction.reset(new CornerDeleteAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, m_pSpreadSamePartDlg));
	}
	else if (actionType == MEASURE_PT_DISTANCE)
	{
		m_pAction.reset(new DistanceMeasureAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort));
	}
    else if (actionType == EDIT_CreatePartGroup)
    {
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();
        if (pSelectedPartPlacements->size() > 0)
        {
            // 从零件放置集合中过滤出零件组
            PartGroupListPtr pPartGroups_return(new PartGroupList);
            PartPlacementListPtr pPartPlacements_return(new PartPlacementList);
            PartPlacementManager::FilterOutPartGroups_fromSelectedPartPlacements(pSelectedPartPlacements.get(), m_pPartGroups.get(), pPartGroups_return, pPartPlacements_return);

            // 执行命令，创建零件组。
            CWnd* pView = CWnd::FromHandle(m_pViewPort->GetWnd());
            PartLayoutHelper::ExecuteCommand2CreatePartGroup(pView, m_pPartGroups, pPartGroups_return, pPartPlacements_return);

            // 进入零件选择action。
            PartPlacementSelectActionPtr pPartPlacementSelectAction(new PartPlacementSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, m_pDisDetectDlg, m_pSelectPartOrLoopActionBar));
            pPartPlacementSelectAction->SelectPartPlacements(pSelectedPartPlacements, TRUE);
            m_pAction = pPartPlacementSelectAction;
        }
        else
        {
            CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PartGroup_1);
            MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);

            // 进入零件选择action。
            PartPlacementSelectActionPtr pPartPlacementSelectAction(new PartPlacementSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, m_pDisDetectDlg, m_pSelectPartOrLoopActionBar));
            m_pAction = pPartPlacementSelectAction;
        }
    }
    else if (actionType == EDIT_ExplodePartGroup)
    {
        PartPlacementListPtr pSelectedPartPlacements = GetSelectedPartPlacements();

        // 从零件放置集合中过滤出零件组
        PartGroupListPtr pPartGroups_return(new PartGroupList);
        PartPlacementListPtr pPartPlacements_return(new PartPlacementList);
        PartPlacementManager::FilterOutPartGroups_fromSelectedPartPlacements(pSelectedPartPlacements.get(), m_pPartGroups.get(), pPartGroups_return, pPartPlacements_return);

        if (pPartPlacements_return->size() == 0 && pPartGroups_return->size() == 1)
        {
            // 执行命令，创建零件组。
            CWnd* pView = CWnd::FromHandle(m_pViewPort->GetWnd());
            PartLayoutHelper::ExecuteCommand2ExplodePartGroup(pView, m_pPartGroups, pPartGroups_return->operator [](0));

            // 进入零件选择action。
            PartPlacementSelectActionPtr pPartPlacementSelectAction(new PartPlacementSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, m_pDisDetectDlg, m_pSelectPartOrLoopActionBar));
            pPartPlacementSelectAction->SelectPartPlacements(pSelectedPartPlacements, TRUE);
            m_pAction = pPartPlacementSelectAction;
        }
        else
        {
            CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PartGroup_2);
            MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);

            // 进入零件选择action。
            PartPlacementSelectActionPtr pPartPlacementSelectAction(new PartPlacementSelectAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList,
                m_pPartGroups, m_pViewPort, m_pDisDetectDlg, m_pSelectPartOrLoopActionBar));
            m_pAction = pPartPlacementSelectAction;
        }
    }

	return TRUE;
}

BOOL PartLayoutEditor::SetViewAction(VIEW_ACTION_TYPE viewType)
{
	if (viewType == ACTION_ZOOM_IN)
		m_pAction.reset(new ZoomInAction(m_pViewPort));
	else if(viewType == ACTION_ZOOM_OUT)
		m_pAction.reset(new ZoomOutAction(m_pViewPort));

	return TRUE;
}

void PartLayoutEditor::AddPartPmt(LONGLONG iPartID, LONGLONG iShtParamConfigID)
{
	// 新建action。
	PartPlacementAddAction* pPartPmtAddAction = new PartPlacementAddAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, iPartID, iShtParamConfigID, m_pDisDetectDlg);
 	m_pAction.reset(pPartPmtAddAction);

	// 设置焦点到当前视图。
	CWnd::FromHandle(m_pViewPort->GetWnd())->SetFocus();
}

void PartLayoutEditor::AddPartPmt(PartItemListPtr pPartItemList, const std::map<LONGLONG, std::vector<Matrix2D>>& matrixData_of_sameShapePart, FolderNodePtr pSaveFolder)
{
	// 新建action。
	PartPlacementAddAction2* pPartPmtAddAction = new PartPlacementAddAction2(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroups, m_pViewPort, pPartItemList, matrixData_of_sameShapePart, pSaveFolder, m_pDisDetectDlg);
	m_pAction.reset(pPartPmtAddAction);

	// 设置焦点到当前视图。
	CWnd::FromHandle(m_pViewPort->GetWnd())->SetFocus();
}

void PartLayoutEditor::InitFloatDlg()
{
	// “智能靠碰参数”对话框。
	m_pDisDetectDlg = new DistanceDetectDlg();
	m_pDisDetectDlg->Create(IDD_clPartLayoutUI_DistanceDetect);
	m_pDisDetectDlg->ShowWindow(SW_HIDE);

	// “传播相同零件”对话框。
	m_pSpreadSamePartDlg = new SpreadSamePartDlg();
	m_pSpreadSamePartDlg->Create(IDD_clPartLayoutUI_SpreadSameParts);
	m_pSpreadSamePartDlg->ShowWindow(SW_HIDE);

	// “定义零件阵列”对话框。
	m_pCreatePartGridDlg = new CreatePartGridDlg();
	m_pCreatePartGridDlg->Create(IDD_clPartLayoutUI_CreatePartGrid);
	m_pCreatePartGridDlg->ShowWindow(SW_HIDE);

    //
    m_pSelectPartOrLoopActionBar = new SelectPartOrLoopActionBar();
    m_pSelectPartOrLoopActionBar->Create(IDD_clPartLayoutUI_SelectPartOrLoopActionBar);
    m_pSelectPartOrLoopActionBar->ShowWindow(SW_HIDE);
}

void PartLayoutEditor::DestroyChildDialogOfSheetView()
{
    if (m_pDisDetectDlg != NULL)
    {
        m_pDisDetectDlg->DestroyWindow();
    }

    if (m_pSpreadSamePartDlg != NULL)
    {
        m_pSpreadSamePartDlg->DestroyWindow();
    }

    if (m_pCreatePartGridDlg != NULL)
    {
        m_pCreatePartGridDlg->DestroyWindow();
    }

    if (m_pSelectPartOrLoopActionBar != NULL)
    {
        m_pSelectPartOrLoopActionBar->DestroyWindow();
    }
}

void PartLayoutEditor::ShowDisDetectDlg(PartPlacementMoveBase* pPartPmtMoveBase)
{
	// 把“动作”设置到“智能靠碰参数”对话框。
	m_pDisDetectDlg->InitAction(pPartPmtMoveBase);

	if (!m_pDisDetectDlg->IsWindowVisible())
	{
		// 显示对话框。
		m_pDisDetectDlg->SetParent(CWnd::FromHandle(m_pViewPort->GetWnd()));
		m_pDisDetectDlg->ShowWindow(SW_SHOW);
	}

	// 调整位置。
	CRect parentRect, winRect;
	CWnd::FromHandle(m_pViewPort->GetWnd())->GetWindowRect(parentRect);
	m_pDisDetectDlg->GetWindowRect(winRect);
	m_pDisDetectDlg->MoveWindow(parentRect.Width()-winRect.Width()-7, 2, winRect.Width(), winRect.Height());
}

void PartLayoutEditor::HideDisDetectDlg()
{
	if (m_pDisDetectDlg->IsWindowVisible())
		m_pDisDetectDlg->ShowWindow(SW_HIDE);
}

void PartLayoutEditor::ShowSpreadSamePartDlg()
{
	if (!m_pSpreadSamePartDlg->IsWindowVisible())
	{
		// 显示对话框。
		m_pSpreadSamePartDlg->SetParent(CWnd::FromHandle(m_pViewPort->GetWnd()));
		m_pSpreadSamePartDlg->ShowWindow(SW_SHOW);
	}

	// 调整位置。
	CRect parentRect, winRect;
	CWnd::FromHandle(m_pViewPort->GetWnd())->GetWindowRect(parentRect);
	m_pSpreadSamePartDlg->GetWindowRect(winRect);
	m_pSpreadSamePartDlg->MoveWindow(parentRect.Width()-winRect.Width()-7, 2, winRect.Width(), winRect.Height());
}

void PartLayoutEditor::HideSpreadSamePartDlg()
{
	if (m_pSpreadSamePartDlg->IsWindowVisible())
		m_pSpreadSamePartDlg->ShowWindow(SW_HIDE);
}

void PartLayoutEditor::ShowSelectPartOrLoopActionBar()
{
    if (!m_pSelectPartOrLoopActionBar->IsWindowVisible())
    {
        // 显示对话框。
        m_pSelectPartOrLoopActionBar->SetParent(CWnd::FromHandle(m_pViewPort->GetWnd()));
        m_pSelectPartOrLoopActionBar->ShowWindow(SW_SHOW);
    }

    // 调整位置。
    CRect parentRect, winRect;
    CWnd::FromHandle(m_pViewPort->GetWnd())->GetWindowRect(parentRect);
    m_pSelectPartOrLoopActionBar->GetWindowRect(winRect);
    m_pSelectPartOrLoopActionBar->MoveWindow(parentRect.Width()-winRect.Width()-7, 2, winRect.Width(), winRect.Height());
}

void PartLayoutEditor::HideSelectPartOrLoopActionBar()
{
    if (m_pSelectPartOrLoopActionBar->IsWindowVisible())
        m_pSelectPartOrLoopActionBar->ShowWindow(SW_HIDE);
}

void PartLayoutEditor::HidePartGridDlg()
{
	if (m_pCreatePartGridDlg->IsWindowVisible())
		m_pCreatePartGridDlg->ShowWindow(SW_HIDE);
}

END_CUTLEADER_NAMESPACE()
