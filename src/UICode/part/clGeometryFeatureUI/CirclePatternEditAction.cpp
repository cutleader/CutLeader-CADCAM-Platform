#include "StdAfx.h"
#include "CirclePatternEditAction.h"

#include "Point2D.h"
#include "GlViewPort.h"
#include "ICommand.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "clUILibResource.h"
#include "CursorDrawer.h"
#include "Point2DList.h"

#include "PatternSelectAction.h"
#include "CirclePattern.h"
#include "PatternList.h"
#include "PatternAddCommand.h"
#include "GripEditCommand.h"
#include "clGeometryFeatureResource.h"
#include "PatternPropertyBar.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

CirclePatternEditAction::CirclePatternEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort)
{
	m_drawState = DRAW_CIRCLE_NOTSTART;
	m_editState = EDIT_CIRCLE_NOTSTART;
	memset(m_data, 0, sizeof(double)*3);

	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;
	m_pViewPort = pViewPort;

	m_iMsgID = IDS_OUTPUT_PATCIRCLE_CENTERPT;
}

CirclePatternEditAction::~CirclePatternEditAction(void)
{
}

BOOL CirclePatternEditAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_editState != EDIT_CIRCLE_NOTSTART)
	{
		if (m_editState == EDIT_CIRCLE_CENTER_POINT)
		{
			m_data[0] = dPtX;
			m_data[1] = dPtY;
		}
		else
		{
			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}

		// create the new circle pattern with the new data.
		Point2D basePt(m_data[0], m_data[1]);	
		IPatternPtr pNewPat(new CirclePattern(basePt, m_data[2]));

		// create command
		EditorData editorData = GetEditorData();
		ICommandPtr pCommand(new GripEditCommand(editorData, pNewPat, m_pPatternLoopList, m_pGripEditPat));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// enter the selecting pattern action.
		EnterPatSelAction(m_pGripEditPat);
	}
	else
	{
		if (m_drawState == DRAW_CIRCLE_NOTSTART || 
		    m_drawState == DRAW_CIRCLE_END_POINT) //first click
		{
			m_drawState = DRAW_CIRCLE_CENTER_POINT;
			m_iMsgID = IDS_OUTPUT_PATCIRCLE_RADIUS;

			m_data[0] = dPtX;
			m_data[1] = dPtY;
			m_data[2] = 0;
		}
		else if(m_drawState == DRAW_CIRCLE_CENTER_POINT) //second click, we can create a circle
		{
			m_drawState = DRAW_CIRCLE_END_POINT;
			m_iMsgID = IDS_OUTPUT_PATCIRCLE_CENTERPT;

			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0]) + (dPtY - m_data[1])*(dPtY - m_data[1]));

			// create circle pattern.
			Point2D basePt(m_data[0], m_data[1]);
			IPatternPtr pCirclePat(new CirclePattern(basePt, m_data[2]));

			// create command
			EditorData editorData = GetEditorData();
			ICommandPtr pCommand(new PatternAddCommand(editorData, m_pPatList, m_pPatternLoopList, pCirclePat));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// enter the selecting pattern action.
			EnterPatSelAction(pCirclePat);
		}
	}

	return TRUE;
}

BOOL CirclePatternEditAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_editState != EDIT_CIRCLE_NOTSTART) //EDIT mode
	{
		if (m_editState == EDIT_CIRCLE_CENTER_POINT)
		{
			m_data[0] = dPtX;
			m_data[1] = dPtY;
		}
		else
		{
			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}
	}
	else
	{
		if(m_drawState == DRAW_CIRCLE_CENTER_POINT)
		{
			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}
	}

	return TRUE;
}

void CirclePatternEditAction::ConvertKBInput(double& dVal1, double& dVal2)
{
	if (m_editState != EDIT_CIRCLE_NOTSTART)
	{
		// under "edit" action.

		if (m_editState == EDIT_CIRCLE_CENTER_POINT)
		{
			// now, do not need to convert.
			;
		}
		else
		{
			// now, user snaps the snap-point to change the radius
			// "dVal1" holds the radius.
			
			/************************************************************************/
			// get the snap point on the right side of the circle.
			
			Point2D centerPt(m_data[0], m_data[1]);

			Vector2D v1;
			v1.Magnitude(dVal1);

			centerPt.Transfer(v1);
			/************************************************************************/

			dVal1 = centerPt.X();
			dVal2 = centerPt.Y();
		}
	}
	else
	{
		// under "draw" action.

		if (m_drawState == DRAW_CIRCLE_NOTSTART || 
			m_drawState == DRAW_CIRCLE_END_POINT)
		{
			// now, do not need to convert.
			;
		}
		else if(m_drawState == DRAW_CIRCLE_CENTER_POINT)
		{
			// now, "m" holds the radius.

			/************************************************************************/
			// get the snap point on the right side of the circle.

			Point2D centerPt(m_data[0], m_data[1]);

			Vector2D v1;
			v1.Magnitude(dVal1);

			centerPt.Transfer(v1);
			/************************************************************************/

			dVal1 = centerPt.X();
			dVal2 = centerPt.Y();
		}
	}
}

BOOL CirclePatternEditAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

BOOL CirclePatternEditAction::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL CirclePatternEditAction::DrawBeforeFinish()
{
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();

	// keep the old color.
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	//
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());

	if (m_drawState == DRAW_CIRCLE_CENTER_POINT || m_editState != EDIT_CIRCLE_NOTSTART)
	{
		m_pViewPort->DrawCircle2D(m_data[0], m_data[1], m_data[2]);
	}

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void CirclePatternEditAction::UpdateForUndoRedo()
{
	if (m_editState != EDIT_CIRCLE_NOTSTART && !m_pPatList->GetPatternFromID(m_pGripEditPat->GetID()))
	{
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}
}

void CirclePatternEditAction::SetGripEditPattern(IPatternPtr pPattern)
{
	m_pGripEditPat = pPattern;

	CirclePattern* pCirclePat = dynamic_cast<CirclePattern*>(pPattern.get());

	// fill the m_data with the data in line pattern.
	Point2D startPt = pCirclePat->GetBasePoint();
	m_data[0] = startPt.X();
	m_data[1] = startPt.Y();

	m_data[2] =	pCirclePat->GetRadius();
}

void CirclePatternEditAction::SetGripEditPt(double dPtX, double dPtY)
{
	// get snap points of the pattern
	Point2DList snapPts;
	m_pGripEditPat->GetSnapPoints(snapPts);

	Point2D tmpPt(dPtX, dPtY);
	for (unsigned int i = 0; i < snapPts.size(); i++)
	{
		Point2D snapPt = (Point2D)snapPts[i];
		if (snapPt.IsEqual(tmpPt))
		{
			if (i == 0)
			{
				m_editState = EDIT_CIRCLE_CENTER_POINT;
				m_iMsgID = IDS_OUTPUT_PATCIRCLE_CENTERPT;
			}
			else if (i == 1)
			{
				m_editState = EDIT_CIRCLE_TOP_POINT;
				m_iMsgID = IDS_OUTPUT_PATCIRCLE_RADIUS;
			}
			else if (i == 2)
			{
				m_editState = EDIT_CIRCLE_LEFT_POINT;
				m_iMsgID = IDS_OUTPUT_PATCIRCLE_RADIUS;
			}
			else if (i == 3)
			{
				m_editState = EDIT_CIRCLE_BOTTOM_POINT;
				m_iMsgID = IDS_OUTPUT_PATCIRCLE_RADIUS;
			}
			else if (i == 4)
			{
				m_editState = EDIT_CIRCLE_RIGHT_POINT;
				m_iMsgID = IDS_OUTPUT_PATCIRCLE_RADIUS;
			}

			return;
		}
	}
}

END_CUTLEADER_NAMESPACE()
