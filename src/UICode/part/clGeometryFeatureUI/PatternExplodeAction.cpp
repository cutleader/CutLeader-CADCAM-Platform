#include "StdAfx.h"
#include "PatternExplodeAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "clUILibResource.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "PatternDrawer.h"
#include "IArrayPattern.h"
#include "PatternList.h"
#include "ArrayPatternExplodeCommand.h"
#include "StdPatternExplodeCommand.h"
#include "clGeometryFeatureResource.h"
#include "PatternSelectAction.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternExplodeAction::PatternExplodeAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort)
{
	m_bSupportDrawAssit = FALSE;

	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;
	m_pViewPort = pViewPort;

	m_pActivePat.reset();
	m_pStdArrayPat.reset();

	m_state = EXPLODE_PAT_NOTSTART;

	m_bDisplayValInput = FALSE;
	m_iMsgID = IDS_OUTPUT_SELECT_STD_ARRAY_PAT;
}

PatternExplodeAction::~PatternExplodeAction(void)
{
}

BOOL PatternExplodeAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActivePat)
	{
		// check whether the active pattern is std/array pattern.
		if (m_pActivePat->IsArrayPattern() || m_pActivePat->IsStdPattern())
		{
			m_pStdArrayPat = m_pActivePat;

			m_state = EXPLODE_PAT_SELECTPAT;
			m_iMsgID = IDS_OUTPUT_RIGHT_CLICK_EXPLODE;
		}
		else
		{
			// display message to user.
			CString strMsg, strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_SELECT_STD_ARRAY_PAT);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
	}

	return TRUE;
}

BOOL PatternExplodeAction::RButtonUp(double& dPtX, double& dPtY)
{
	if (m_pStdArrayPat && m_state == EXPLODE_PAT_SELECTPAT)
	{
		m_state = EXPLODE_PAT_NOTSTART;
		m_iMsgID = IDS_OUTPUT_SELECT_STD_ARRAY_PAT;

		// explode the pattern.
		ExplodePat(m_pStdArrayPat);
	}

	return TRUE;
}

BOOL PatternExplodeAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// get the pattern which is closest to cursor.
	m_pActivePat = m_pPatList->GetPatternClosestTo(pt);

	return TRUE;
}

BOOL PatternExplodeAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PatternEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	return TRUE;
}

BOOL PatternExplodeAction::DrawBeforeFinish()
{
	PatternDrawer patDrawer(m_pViewPort);

	// draw the active pattern in highlight mode.
	if (m_pActivePat)
		patDrawer.DrawActivePat(m_pActivePat);

	// draw the selected std/array pattern.
	if (m_pStdArrayPat)
		patDrawer.DrawSelPat(m_pStdArrayPat);

	CursorDrawer::DrawRectSelCursor(m_pViewPort);

	return TRUE;
}

void PatternExplodeAction::UpdateForUndoRedo()
{
	// if the active pattern is lost, reset it.
	if (m_pActivePat && !m_pPatList->GetPatternFromID(m_pActivePat->GetID()))
		m_pActivePat.reset();

	// if the selected pattern is lost, reset the action.
	if (m_pStdArrayPat && !m_pPatList->GetPatternFromID(m_pStdArrayPat->GetID()))
	{
		m_pActivePat.reset();
		m_pStdArrayPat.reset();

		m_state = EXPLODE_PAT_NOTSTART;
		m_iMsgID = IDS_OUTPUT_SELECT_STD_ARRAY_PAT;
	}
}

void PatternExplodeAction::ExplodePat(IPatternPtr pStdArrayPat)
{
	/************************************************************************/		
	// create command.

	ICommandPtr pCommand;
	EditorData editorData = GetEditorData();

	// select command based on the pattern.
	if (pStdArrayPat->IsArrayPattern())
	{
		IArrayPatternPtr pGridPattern = boost::dynamic_pointer_cast<IArrayPattern>(pStdArrayPat);
		pCommand.reset(new ArrayPatternExplodeCommand(editorData, m_pPatList, m_pPatternLoopList, pGridPattern));
	}
	else if (pStdArrayPat->IsStdPattern())
	{
		pCommand.reset(new StdPatternExplodeCommand(editorData, m_pPatList, m_pPatternLoopList, pStdArrayPat));
	}

	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);
	/************************************************************************/

	// enter the selecting pattern action.
	PatternSelectActionPtr pPatSelectAction(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	m_pNextAction = pPatSelectAction;
}

END_CUTLEADER_NAMESPACE()
