#include "StdAfx.h"
#include "SequenceEditor.h"

#include "GlViewPort.h"
#include "IAction.h"
#include "DummyAction.h"
#include "clBaseDataResource.h"

#include "PartPlacementList.h"
#include "PartInstanceList.h"
#include "LoopInstanceList.h"

#include "LoopSequenceAddAction.h"
#include "BridgeSequenceAddAction.h"
#include "BridgeSequenceBreakAction.h"
#include "StartBridgeNodeChangeAction.h"
#include "LoopSequenceInsertAction.h"
#include "HoleSequenceReplaceAction.h"
#include "CutSequenceDeleteAction.h"
#include "CutSequenceSelectAction.h"
#include "SequenceViewAction.h"
#include "CutSequenceOrderChangeActionA.h"
#include "CutSequenceOrderChangeActionB.h"
#include "MovePtAddAction.h"
#include "MovePtDeleteAction.h"
#include "ChainCutSequenceAddAction.h"
#include "ChainNodeInsertAction.h"
#include "ChainNodeDeleteAction.h"
#include "ResetInstructionAddAction.h"
#include "ResetInstructionDeleteAction.h"
#include "GridCutSequenceAddAction.h"
#include "RemnantCutlineAutoAction.h"
#include "RemnantCutlineAddAction.h"
#include "RemnantCutlineDeleteAction.h"
#include "ReplaceAsFlyCutAction.h"
#include "CutSequenceList.h"
#include "CutNodeSimulator.h"
#include "clCutSequenceResource.h"
#include "ConfigData.h"
#include "RealTimeCutSimulator.h"
#include "RealTimeCutAction.h"

#include "ParamConfigItem.h"
#include "LeadConfigItem.h"
#include "SequenceConfigItem.h"
#include "ParamConfig.h"
#include "LoopSequenceDeleteAction.h"


BEGIN_CUTLEADER_NAMESPACE()


SequenceEditor::SequenceEditor(GlViewPortPtr pViewPort)
{
	m_pViewPort = pViewPort;
	m_pAction.reset(new DummyAction(m_pViewPort));

    m_pApplyPartDlg = NULL;
    m_pInsertPosDlg = NULL;
    m_pGenerateRemCutLineDlg = NULL;
}

SequenceEditor::~SequenceEditor(void)
{
}

BOOL SequenceEditor::LButtonDown(double& dPtX, double& dPtY)
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

BOOL SequenceEditor::LButtonUp(double& dPtX, double& dPtY)
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

BOOL SequenceEditor::LButtonDBClk(double& dPtX, double& dPtY)
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

BOOL SequenceEditor::RButtonUp(double& dPtX, double& dPtY)
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

BOOL SequenceEditor::MovePoint(double& dPtX, double& dPtY)
{
	if (m_pAction)
	{
		// 显示“以零件为单位”对话框。
		if (m_pAction->GetActionType() == SEQUENCE_ADD_LOOPSEQ || m_pAction->GetActionType() == SEQUENCE_REMOVE ||
			m_pAction->GetActionType() == SEQUENCE_SORT_A || m_pAction->GetActionType() == SEQUENCE_SORT_B)
		{
			ShowApplyPartDlg();
		}
		else
		{
			HideApplyPartDlg();
		}

		// “指定插入位置”对话框。
		if (m_pAction->GetActionType() == SEQUENCE_INSERT_CHAINNODE)
		{
			ShowInsertPosDlg();
		}
		else
		{
			HideInsertPosDlg();
		}

		// “生成余料切割线”对话框。
		if (m_pAction->GetActionType() != SEQUENCE_AUTO_REM_CUTLINE)
		{
			Hide_GenerateRemCutLineDlg();
		}

		m_pAction->MovePoint(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL SequenceEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

BOOL SequenceEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
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

CString SequenceEditor::GetEditorName()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ);
	return str;
}

BOOL SequenceEditor::SetAction(SequenceActionType actionType)
{
	/************************************************************************/
	// prepare some data.

	// get scheme item for lead assignment.
	ParamConfigItemPtr pParamConfigItem1 = m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_LEAD_ASSIGN);
	LeadConfigItemPtr pLeadConfigItem = boost::dynamic_pointer_cast<LeadConfigItem>(pParamConfigItem1);

	// 工序分配规则。
	ParamConfigItemPtr pParamConfigItem = m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_GENERATE_SEQ);
	SequenceConfigItemPtr pSeqConfigItem = boost::dynamic_pointer_cast<SequenceConfigItem>(pParamConfigItem);

	ConfigDataPtr pConfigData(new ConfigData(pLeadConfigItem, pSeqConfigItem, m_pMatSize));
	/************************************************************************/


	SequenceEditActionBase* pSequenceEditActionBase;
	if (actionType == SEQUENCE_SELECT)
	{
		pSequenceEditActionBase = new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_SORT_A)
	{
		pSequenceEditActionBase = new CutSequenceOrderChangeActionA(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort, m_pApplyPartDlg);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_SORT_B)
	{
		pSequenceEditActionBase = new CutSequenceOrderChangeActionB(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort, m_pApplyPartDlg);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_ADD_LOOPSEQ)
	{
		pSequenceEditActionBase = new LoopSequenceAddAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort, m_pApplyPartDlg);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_ADD_BDGESEQ)
	{
		pSequenceEditActionBase = new BridgeSequenceAddAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_BREAK_BDGESEQ)
	{
		pSequenceEditActionBase = new BridgeSequenceBreakAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_BDGE_START_CHANGE)
	{
		pSequenceEditActionBase = new StartBridgeNodeChangeAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_ADD_CHAINSEQ)
	{
		pSequenceEditActionBase = new ChainCutSequenceAddAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_INSERT_CHAINNODE)
	{
		pSequenceEditActionBase = new ChainNodeInsertAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort, m_pInsertPosDlg);
		m_pAction.reset(pSequenceEditActionBase);
	}
    else if (actionType == SEQUENCE_ADD_RESET_INSTRUCT)
    {
        pSequenceEditActionBase = new ResetInstructionAddAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
        m_pAction.reset(pSequenceEditActionBase);
    }
    else if (actionType == SEQUENCE_DEL_RESET_INSTRUCT)
    {
        pSequenceEditActionBase = new ResetInstructionDeleteAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
        m_pAction.reset(pSequenceEditActionBase);
    }
	else if (actionType == SEQUENCE_DEL_CHAINNODE)
	{
		pSequenceEditActionBase = new ChainNodeDeleteAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_ADD_GRIDSEQ)
	{
		pSequenceEditActionBase = new GridCutSequenceAddAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_REPLACE_AS_FLYCUT)
	{
		pSequenceEditActionBase = new ReplaceAsFlyCutAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_ADD_MOV_PT)
	{
		pSequenceEditActionBase = new MovePtAddAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_DEL_MOV_PT)
	{
		pSequenceEditActionBase = new MovePtDeleteAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_AUTO_REM_CUTLINE)
	{
		pSequenceEditActionBase = new RemnantCutlineAutoAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort, m_pGenerateRemCutLineDlg);
		m_pAction.reset(pSequenceEditActionBase);
		Show_GenerateRemCutLineDlg();
	}
	else if (actionType == SEQUENCE_ADD_REM_CUTLINE)
	{
		pSequenceEditActionBase = new RemnantCutlineAddAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_DEL_REM_CUTLINE)
	{
		pSequenceEditActionBase = new RemnantCutlineDeleteAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_INSERT_LOOPSEQ)
	{
		pSequenceEditActionBase = new LoopSequenceInsertAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_REPLACE_HOLE_SEQ)
	{
		pSequenceEditActionBase = new HoleSequenceReplaceAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		m_pAction.reset(pSequenceEditActionBase);
	}
	else if (actionType == SEQUENCE_REMOVE)
	{
		// 看看当前有没有选中工序。
		CutSequenceListPtr pSelSeqUnitList;
		if (m_pAction->GetActionType() == SEQUENCE_SELECT)
		{
			CutSequenceSelectActionPtr pSeqUnitSelectAction = boost::dynamic_pointer_cast<CutSequenceSelectAction>(m_pAction);
			pSelSeqUnitList = pSeqUnitSelectAction->GetSelSeqUnits();
		}

		// 切换到工序删除动作。
		CutSequenceDeleteAction* pSequenceDelAction = new CutSequenceDeleteAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort, m_pApplyPartDlg);
		pSequenceEditActionBase = pSequenceDelAction;
		m_pAction.reset(pSequenceEditActionBase);

		// 要是有选中的工序，删除。
		if (pSelSeqUnitList && pSelSeqUnitList->size() > 0)
        {
			pSequenceDelAction->SelectSeqUnits(pSelSeqUnitList);
        }
	}
	else if (actionType == SEQUENCE_VIEW)
	{
		// if still running, we do nothing.
		if (!CutNodeSimulator::GetInstance()->IsFinished())
        {
			CutNodeSimulator::GetInstance()->StopSimulation();
        }

		// then, check whether there are sequence unit exist.
		if (m_pCutSequences->size() == 0)
		{
			// warning user.
			CString strTitle = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			CString strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_NO_SEQ_SIM);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strTitle, MB_OK | MB_ICONWARNING);
			return FALSE;
		}

		// create the SequenceViewAction.
		SequenceViewAction* pSequenceViewAction = new SequenceViewAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
		pSequenceViewAction->Start();

		pSequenceEditActionBase = pSequenceViewAction;
		m_pAction.reset(pSequenceEditActionBase);
	}
    else if (actionType == SEQUENCE_Remove_LoopSequence)
    {
        m_pApplyPartDlg->IsApplyPart(FALSE);
        pSequenceEditActionBase = new LoopSequenceDeleteAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort, m_pApplyPartDlg);
        m_pAction.reset(pSequenceEditActionBase);
    }
    else if (actionType == SEQUENCE_RealTime_Cut)
    {
        // if still running, we do nothing.
        if (!RealTimeCutSimulator::GetInstance()->IsFinished())
        {
            RealTimeCutSimulator::GetInstance()->StopSimulation();
        }

        // then, check whether there are sequence unit exist.
        if (m_pCutSequences->size() == 0)
        {
            // warning user.
            CString strTitle = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            CString strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_NO_SEQ_SIM);
            MessageBox(m_pViewPort->GetWnd(), strMsg, strTitle, MB_OK | MB_ICONWARNING);
            return FALSE;
        }

        // create the RealTimeCutAction.
        RealTimeCutAction* pRealTimeCutAction = new RealTimeCutAction(m_pPartLayoutData, m_pCutSequences, pConfigData, m_pViewPort);
        pRealTimeCutAction->Start();

        pSequenceEditActionBase = pRealTimeCutAction;
        m_pAction.reset(pSequenceEditActionBase);
    }

	return TRUE;
}

void SequenceEditor::InitApplyPartDlg()
{
	m_pApplyPartDlg = new ApplyPartDlg();
	m_pApplyPartDlg->Create(IDD_clCutSequenceUI_ApplyPart);
	m_pApplyPartDlg->ShowWindow(SW_HIDE);
}

void SequenceEditor::ShowApplyPartDlg()
{
	if (!m_pApplyPartDlg->IsWindowVisible())
	{
		// 显示对话框。
		m_pApplyPartDlg->SetParent(CWnd::FromHandle(m_pViewPort->GetWnd()));
		m_pApplyPartDlg->ShowWindow(SW_SHOW);
	}

	// 调整位置。
	CRect parentRect, winRect;
	CWnd::FromHandle(m_pViewPort->GetWnd())->GetWindowRect(parentRect);
	m_pApplyPartDlg->GetWindowRect(winRect);
	m_pApplyPartDlg->MoveWindow(parentRect.Width()-winRect.Width()-7, 2, winRect.Width(), winRect.Height());
}

void SequenceEditor::HideApplyPartDlg()
{
	if (m_pApplyPartDlg->IsWindowVisible())
		m_pApplyPartDlg->ShowWindow(SW_HIDE);
}

void SequenceEditor::InitInsertPosDlg()
{
	m_pInsertPosDlg = new InsertPositionDlg();
	m_pInsertPosDlg->Create(IDD_clCutSequenceUI_InsertPosition);
	m_pInsertPosDlg->ShowWindow(SW_HIDE);
}

void SequenceEditor::ShowInsertPosDlg()
{
	if (!m_pInsertPosDlg->IsWindowVisible())
	{
		// 显示对话框。
		m_pInsertPosDlg->SetParent(CWnd::FromHandle(m_pViewPort->GetWnd()));
		m_pInsertPosDlg->ShowWindow(SW_SHOW);
	}

	// 调整位置。
	CRect parentRect, winRect;
	CWnd::FromHandle(m_pViewPort->GetWnd())->GetWindowRect(parentRect);
	m_pInsertPosDlg->GetWindowRect(winRect);
	m_pInsertPosDlg->MoveWindow(parentRect.Width()-winRect.Width()-7, 2, winRect.Width(), winRect.Height());
}

void SequenceEditor::HideInsertPosDlg()
{
	if (m_pInsertPosDlg->IsWindowVisible())
		m_pInsertPosDlg->ShowWindow(SW_HIDE);
}

void SequenceEditor::Init_GenerateRemCutLineDlg()
{
	m_pGenerateRemCutLineDlg = new GenerateRemCutLineDlg();
	m_pGenerateRemCutLineDlg->Create(IDD_clCutSequenceUI_CreateRemnantCutLine);
	m_pGenerateRemCutLineDlg->ShowWindow(SW_HIDE);
}

void SequenceEditor::Show_GenerateRemCutLineDlg()
{
	if (!m_pGenerateRemCutLineDlg->IsWindowVisible())
	{
		// 显示对话框。
		m_pGenerateRemCutLineDlg->SetParent(CWnd::FromHandle(m_pViewPort->GetWnd()));
		m_pGenerateRemCutLineDlg->ShowWindow(SW_SHOW);
	}

	// 调整位置。
	CRect parentRect, winRect;
	CWnd::FromHandle(m_pViewPort->GetWnd())->GetWindowRect(parentRect);
	m_pGenerateRemCutLineDlg->GetWindowRect(winRect);
	m_pGenerateRemCutLineDlg->MoveWindow(parentRect.Width()-winRect.Width()-7, 2, winRect.Width(), winRect.Height());
}

void SequenceEditor::Hide_GenerateRemCutLineDlg()
{
	if (m_pGenerateRemCutLineDlg->IsWindowVisible())
		m_pGenerateRemCutLineDlg->ShowWindow(SW_HIDE);
}

void SequenceEditor::DestroyChildDialogOfSheetView()
{
    if (m_pApplyPartDlg != NULL)
    {
        m_pApplyPartDlg->DestroyWindow();
    }

    if (m_pInsertPosDlg != NULL)
    {
        m_pInsertPosDlg->DestroyWindow();
    }

    if (m_pGenerateRemCutLineDlg != NULL)
    {
        m_pGenerateRemCutLineDlg->DestroyWindow();
    }
}

END_CUTLEADER_NAMESPACE()
