#include "StdAfx.h"
#include "PatternTransformActionBase.h"

#include "ValueInputManager.h"
#include "CommandManager.h"
#include "GlViewPort.h"
#include "clUILibResource.h"
#include "clBaseDataResource.h"

#include "PatternSelectAction.h"
#include "PatternTransformCommand.h"
#include "PatternList.h"
#include "clGeometryFeatureResource.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternTransformActionBase::PatternTransformActionBase(void)
{
	m_actionState = TRANSFORM_PATTERN_NOTSTART;
	m_pNewPats.reset(new PatternList);
}

PatternTransformActionBase::~PatternTransformActionBase(void)
{
}

BOOL PatternTransformActionBase::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		// now PatternPreSelectAction will care the input.

		PatternPreSelectAction::LButtonDown(dPtX, dPtY);
	}
	else
	{
		if (m_actionState == TRANSFORM_PATTERN_NOTSTART || m_actionState == TRANSFORM_PATTERN_SECOND_POINT)
		{
			// we selected base point.

			m_actionState = TRANSFORM_PATTERN_BASE_POINT;
			m_iMsgID = GetMsgID();

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
		else if (m_actionState == TRANSFORM_PATTERN_BASE_POINT)
		{
			// we selected the second point.

			m_actionState = TRANSFORM_PATTERN_SECOND_POINT;
			m_iMsgID = GetMsgID();

			// set the transform matrix.
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			m_dFirstPtX = m_dBasePtX;
			m_dFirstPtY = m_dBasePtY;
			SetMatrixValue();

			// create command to update the pattern(s)
			CString strActionName = GetActionName();
			EditorData editorData = GetEditorData();
			ICommandPtr pCommand(new PatternTransformCommand(editorData, strActionName, m_pPatternLoopList,
														m_pNewPats, m_pSelectedPats));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// enter the selecting pattern action.
			EnterPatSelAction(m_pSelectedPats);
		}
	}

	return TRUE;
}

BOOL PatternTransformActionBase::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		// now PatternPreSelectAction will care the input.

		PatternPreSelectAction::LButtonUp(dPtX, dPtY);
	}

	return TRUE;
}

BOOL PatternTransformActionBase::RButtonUp(double& dPtX, double& dPtY)
{
	if (m_pSelectedPats->size() > 0)
	{
		// press right button while some patterns are selected, we should quit
		// the PRE_SELECT_STATE, then all input will go into TRANSFORM_PATTERN_STATE.

		// we cannot rotate array patterns.
		if (GetActionType() == EDIT_ROTATE)
		{
			for (unsigned int i = 0; i < m_pSelectedPats->size(); i++)
			{
				if (m_pSelectedPats->at(i)->IsArrayPattern())
				{
					// display message to user.
					CString strMsg, strProductName;
					strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
					strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_ROTATEPAT_ARRAY);
					MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
					return TRUE;
				}
			}
		}

		m_preSelectState = PRE_SELECT_INACTIVE;
	}
	else
	{
		// figure out the next action.
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}

	return TRUE;
}

BOOL PatternTransformActionBase::MovePoint(double& dPtX, double& dPtY)
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		// now PatternPreSelectAction will care the input.

		PatternPreSelectAction::MovePoint(dPtX, dPtY);
	}
	else
	{
		if (m_actionState == TRANSFORM_PATTERN_NOTSTART)
		{
			m_iMsgID = GetMsgID();
		}
		else if (m_actionState == TRANSFORM_PATTERN_BASE_POINT)
		{
			// set the transform matrix.
			m_dFirstPtX = m_dSecondPtX;
			m_dFirstPtY = m_dSecondPtY;
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			SetMatrixValue();

			// update the new copy of the selected patterns.
			for (unsigned int i = 0; i < m_pNewPats->size(); i++)
			{
				m_pNewPats->at(i)->Transform(m_modifyPatMat);
			}
		}
	}

	return TRUE;
}

BOOL PatternTransformActionBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

void PatternTransformActionBase::UpdateForUndoRedo()
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
			m_actionState = TRANSFORM_PATTERN_NOTSTART;
			m_iMsgID = IDS_OUTPUT_SELECTPATTERN;

			break;
		}
	}
}

END_CUTLEADER_NAMESPACE()
