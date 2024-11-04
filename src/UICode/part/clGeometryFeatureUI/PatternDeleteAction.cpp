#include "StdAfx.h"
#include "PatternDeleteAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "clUILibResource.h"
#include "CursorDrawer.h"
#include "ValueInputManager.h"

#include "IArrayPattern.h"
#include "PatternList.h"
#include "PatternDrawer.h"
#include "clGeometryFeatureResource.h"
#include "PatternSelectAction.h"
#include "PatternDeleteCommand.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternDeleteAction::PatternDeleteAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList,
								 GlViewPortPtr pViewPort, PatternListPtr pSelPats)
{
	m_bSupportDrawAssit = FALSE;

	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;

	m_pViewPort = pViewPort;
	m_pSelectedPats = pSelPats;

	// if some patterns were pre-selected, disable PatternPreSelectAction.
	if (pSelPats->size() > 0)
		m_preSelectState = PRE_SELECT_INACTIVE;
}

PatternDeleteAction::~PatternDeleteAction(void)
{
}

BOOL PatternDeleteAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::LButtonDown(dPtX, dPtY);
	}

	return TRUE;
}

BOOL PatternDeleteAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::LButtonUp(dPtX, dPtY);
	}

	return TRUE;
}

BOOL PatternDeleteAction::RButtonUp(double& dPtX, double& dPtY)
{
	if (m_pSelectedPats->size() > 0)
	{
		// press right button while some patterns are selected, we should quit
		// the PRE_SELECT_STATE, then all input will go into this class.

		m_preSelectState = PRE_SELECT_INACTIVE;

		// delete the selected patterns.
		DeleteSelectedPats();
	}
	else
	{
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}

	return TRUE;
}

BOOL PatternDeleteAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::MovePoint(dPtX, dPtY);
	}

	return TRUE;
}

BOOL PatternDeleteAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::OnKeyDown(nChar, nRepCnt, nFlags);
	}
	else
	{
		PatternEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);
	}

	return TRUE;
}

BOOL PatternDeleteAction::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL PatternDeleteAction::DrawBeforeFinish()
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::DrawBeforeFinish();
	}
	else
	{
		CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
		DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();

		// keep the old color.
		COLORREF dwOldColor;
		m_pViewPort->GetDrawColor(dwOldColor);

		// draw cursor.
		m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);

		// restore the draw color
		m_pViewPort->SetDrawColor(dwOldColor);
	}

	return TRUE;
}

void PatternDeleteAction::UpdateForUndoRedo()
{
	for (unsigned int i = 0; i < m_pSelectedPats->size(); i++)
	{
		IPatternPtr pPattern = m_pSelectedPats->at(i);
		if (!m_pPatList->GetPatternFromID(pPattern->GetID()))
		{
			// reset the action status.
			m_pSelectedPats->clear();
			m_pActivePat.reset();

			break;
		}
	}
}

void PatternDeleteAction::DeleteSelectedPats()
{
	if (m_pSelectedPats->size() > 0)
	{
		// when delete the grid pattern, the control pattern will also be deleted.
		PatternListPtr pSelPatList(new PatternList());
		for (unsigned int i = 0; i < m_pSelectedPats->size(); i++)
		{
			IPatternPtr pPattern = m_pSelectedPats->at(i);
			pSelPatList->push_back(pPattern);

			// check whether it is a grid pattern.
			if (pPattern->IsArrayPattern())
			{
				IArrayPatternPtr pArrayPattern = boost::dynamic_pointer_cast<IArrayPattern>(pPattern);
				IPatternPtr pCtrlPat = pArrayPattern->GetControlPattern();
				pSelPatList->push_back(pCtrlPat);
			}
		}

		// create "Delete Pattern" command
		EditorData editorData = GetEditorData();
		ICommandPtr pCommand(new PatternDeleteCommand(editorData, m_pPatList, m_pPatternLoopList, pSelPatList));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// clear selected patterns.
		m_pSelectedPats->clear();

		// figure out the next action.
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}
}

END_CUTLEADER_NAMESPACE()
