#include "StdAfx.h"
#include "PatternCopyAction.h"

#include "ValueInputManager.h"
#include "CommandManager.h"
#include "GlViewPort.h"
#include "clUILibResource.h"
#include "CursorDrawer.h"

#include "IPattern.h"
#include "PatternSelectAction.h"
#include "PatternCopyCommand.h"
#include "PatternDrawer.h"
#include "PatternList.h"
#include "clGeometryFeatureResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternCopyAction::PatternCopyAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList,
									 GlViewPortPtr pViewPort, PatternListPtr pSelPats)
{
	m_pNewPats.reset(new PatternList);

	m_pViewPort = pViewPort;
	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;
	m_pSelectedPats = pSelPats;

	// if some patterns were pre-selected, disable PatternPreSelectAction.
	if (pSelPats->size() > 0)
	{
		m_preSelectState = PRE_SELECT_INACTIVE;
		m_iMsgID = IDS_OUTPUT_COPYPAT_BASEPT;
	}
	else
	{
		m_bSupportDrawAssit = FALSE;
	}

	m_actionState = COPY_PATTERN_NOTSTART;
	m_bDisplayValInput = TRUE;
}

PatternCopyAction::~PatternCopyAction(void)
{
}

BOOL PatternCopyAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::LButtonDown(dPtX, dPtY);
	}
	else
	{
		if (m_actionState == COPY_PATTERN_NOTSTART || m_actionState == COPY_PATTERN_DEST_POINT)
		{
			m_actionState = COPY_PATTERN_BASE_POINT;
			m_iMsgID = IDS_OUTPUT_COPYPAT_NEWLOCATION;

			// set the transform matrix.
			m_dBasePtX = dPtX;
			m_dBasePtY = dPtY;
			m_dFirstPtX = dPtX;
			m_dFirstPtY = dPtY;
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			SetMatrixValue();

			// create a new copy of selected patterns.
			for (unsigned int i = 0; i < m_pSelectedPats->size(); i++)
			{
				IDataPtr pTmpData = m_pSelectedPats->at(i)->Clone();
				IPatternPtr pPattern = boost::dynamic_pointer_cast<IPattern>(pTmpData);
				m_pNewPats->push_back(pPattern);
			}
		}
		else if (m_actionState == COPY_PATTERN_BASE_POINT) //select second point
		{
			m_actionState = COPY_PATTERN_DEST_POINT;
			m_iMsgID = IDS_OUTPUT_COPYPAT_BASEPT;

			// set the transform matrix.
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			m_dFirstPtX = m_dBasePtX;
			m_dFirstPtY = m_dBasePtY;
			SetMatrixValue();

			// create command to add the new pattern(s)
			EditorData editorData = GetEditorData();
			ICommandPtr pCommand(new PatternCopyCommand(editorData, m_pPatList, m_pPatternLoopList, m_pNewPats));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// enter the selecting pattern action.
			EnterPatSelAction(m_pNewPats);
		}
	}

	return TRUE;
}

BOOL PatternCopyAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::LButtonUp(dPtX, dPtY);
	}

	return TRUE;
}

BOOL PatternCopyAction::RButtonUp(double& dPtX, double& dPtY)
{
	if (m_pSelectedPats->size() > 0)
	{
		// 退出选择图形动作。
		m_preSelectState = PRE_SELECT_INACTIVE;
	}
	else
	{
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}

	return TRUE;
}

BOOL PatternCopyAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::MovePoint(dPtX, dPtY);
	}
	else
	{
		if (m_actionState == COPY_PATTERN_BASE_POINT)
		{
			// set the transform matrix.
			m_dFirstPtX = m_dSecondPtX;
			m_dFirstPtY = m_dSecondPtY;
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			SetMatrixValue();

			// update the new copy of the selected patterns.
			for(unsigned int i = 0; i < m_pNewPats->size(); i++)
			{
				m_pNewPats->at(i)->Transform(m_modifyPatMat);
			}
		}
	}

	return TRUE;
}

BOOL PatternCopyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PatternEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	// get the user input from output bar.
	double d1, d2;
	if (ValueInputManager::GetInstance()->IsInputEnd(nChar))
	{
		if (!ValueInputManager::GetInstance()->GetInput(d1, d2))
		{
			// user just presses the "Enter" key without inputting value.
			m_pViewPort->GetCursorPos(d1, d2);
		}

		// convert the keyboard input to the values that can be transferred
		// into "LButtonDown" and "MovePoint" function.
		ConvertKBInput(d1, d2);

		// here, we should simulate the "mouse input".
		MovePoint(d1, d2);
		LButtonDown(d1, d2);
	}

	ValueInputManager::GetInstance()->AcceptInputChar(nChar);

	return TRUE;
}

BOOL PatternCopyAction::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL PatternCopyAction::DrawBeforeFinish()
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		PatternPreSelectAction::DrawBeforeFinish();
	}
	else
	{
		CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
		DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();

		// draw the selected patterns.
		PatternDrawer patDrawer(m_pViewPort);
		patDrawer.DrawSelPatList(m_pSelectedPats);

		// keep the old color.
		COLORREF dwOldColor;
		m_pViewPort->GetDrawColor(dwOldColor);

		//
		m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());

		if (m_actionState == COPY_PATTERN_BASE_POINT)
		{
			// draw the trace line.
			m_pViewPort->DrawLinePoint2D(m_dBasePtX, m_dBasePtY, m_dSecondPtX, m_dSecondPtY);

			// update the cache data of the pattern object.
			m_pNewPats->UpdateCache();

			// draw the new copy of the selected patterns.
			patDrawer.DrawPatList(m_pNewPats);
		}

		// draw cursor.
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);

		// restore the draw color
		m_pViewPort->SetDrawColor(dwOldColor);
	}

	return TRUE;
}

void PatternCopyAction::UpdateForUndoRedo()
{
	for (unsigned int i = 0; i < m_pSelectedPats->size(); i++)
	{
		IPatternPtr pPattern = m_pSelectedPats->at(i);
		if (!m_pPatList->GetPatternFromID(pPattern->GetID()))
		{
			// reset the action status.

			m_pSelectedPats->clear();
			m_pNewPats->clear();
			m_pActivePat.reset();

			m_preSelectState = PRE_SELECT_ACTIVE;
			m_actionState = COPY_PATTERN_NOTSTART;
			m_iMsgID = IDS_OUTPUT_SELECTPATTERN;

			break;
		}
	}
}

void PatternCopyAction::SetMatrixValue()
{
	m_modifyPatMat.Reset();

	Vector2D vect;
	vect.X((m_dSecondPtX - m_dFirstPtX));
	vect.Y((m_dSecondPtY - m_dFirstPtY));

	m_modifyPatMat.Transfer(vect);
}

END_CUTLEADER_NAMESPACE()
