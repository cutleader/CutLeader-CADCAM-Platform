#include "StdAfx.h"
#include "LinePatternEditAction.h"

#include "Point2D.h"
#include "GlViewPort.h"
#include "ICommand.h"
#include "ValueInputManager.h"
#include "clUILibResource.h"
#include "CommandManager.h"
#include "Point2DList.h"
#include "CursorDrawer.h"

#include "LinePattern.h"
#include "PatternList.h"
#include "PatternLoopList.h"
#include "PatternSelectAction.h"
#include "PatternAddCommand.h"
#include "GripEditCommand.h"
#include "clGeometryFeatureResource.h"
#include "PatternPropertyBar.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

LinePatternEditAction::LinePatternEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort)
{
	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;

	m_pViewPort = pViewPort;

	m_drawState = DRAW_LINE_NOTSTART;
	m_editState = EDIT_LINE_NOTSTART;
	memset(m_data, 0, sizeof(double)*4);

	m_iMsgID = IDS_OUTPUT_DRAWLINE_STARTPT;
}

LinePatternEditAction::~LinePatternEditAction(void)
{
}

BOOL LinePatternEditAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_editState != EDIT_LINE_NOTSTART) // EDIT mode
	{
		if (m_editState == EDIT_LINE_START_POINT)
		{
			m_data[0] = dPtX;
			m_data[1] = dPtY;
		}
		else if(m_editState == EDIT_LINE_CENTER_POINT)
		{
			double dOriCenterX = (m_data[0]+m_data[2])/2;
			double dOriCenterY = (m_data[1]+m_data[3])/2;

			m_data[0] += dPtX - dOriCenterX;
			m_data[1] += dPtY - dOriCenterY;
			m_data[2] += dPtX - dOriCenterX;
			m_data[3] += dPtY - dOriCenterY;
		}
		else if(m_editState == EDIT_LINE_END_POINT)
		{
			m_data[2] = dPtX;
			m_data[3] = dPtY;
		}

		// create the new line pattern with the new data.
		Point2D basePt(m_data[0], m_data[1]);
		Vector2D v(m_data[2] - m_data[0], m_data[3] - m_data[1]);	
		IPatternPtr pNewPat(new LinePattern(basePt, v));

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
		// first click
		if (m_drawState == DRAW_LINE_NOTSTART || m_drawState == DRAW_LINE_END_POINT)
		{
			m_drawState = DRAW_LINE_START_POINT;
			m_iMsgID = IDS_OUTPUT_DRAWLINE_ENDPT;

			m_data[0] = dPtX;
			m_data[1] = dPtY;
			m_data[2] = dPtX;
			m_data[3] = dPtY;

			// add the first input point to "m_pInputPtList"
			m_pInputPtList->push_back(Point2D(dPtX, dPtY));
		}
		else if (m_drawState == DRAW_LINE_START_POINT)
		{
			// second click, we can create a line.

			m_drawState = DRAW_LINE_END_POINT;
			m_iMsgID = IDS_OUTPUT_DRAWLINE_STARTPT;

			m_data[2] = dPtX;
			m_data[3] = dPtY;

			// create line pattern.
			Point2D basePt(m_data[0], m_data[1]);
			Vector2D v(m_data[2] - m_data[0], m_data[3] - m_data[1]);
			IPatternPtr pLinePat(new LinePattern(basePt, v));

			// create command
			EditorData editorData = GetEditorData();
			ICommandPtr pCommand(new PatternAddCommand(editorData, m_pPatList, m_pPatternLoopList, pLinePat));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// finished drawing line, clear "m_pInputPtList".
			m_pInputPtList->clear();

			// enter the selecting pattern action.
			EnterPatSelAction(pLinePat);
		}
	}

	return TRUE;
}

BOOL LinePatternEditAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_editState != EDIT_LINE_NOTSTART) // EDIT mode
	{
		if (m_editState == EDIT_LINE_START_POINT)
		{
			m_data[0] = dPtX;
			m_data[1] = dPtY;
		}
		else if(m_editState == EDIT_LINE_CENTER_POINT)
		{
			double dOriCenterX = (m_data[0]+m_data[2])/2;
			double dOriCenterY = (m_data[1]+m_data[3])/2;

			m_data[0] += dPtX - dOriCenterX;
			m_data[1] += dPtY - dOriCenterY;
			m_data[2] += dPtX - dOriCenterX;
			m_data[3] += dPtY - dOriCenterY;
		}
		else if(m_editState == EDIT_LINE_END_POINT)
		{
			m_data[2] = dPtX;
			m_data[3] = dPtY;
		}
	}
	else
	{
		if (m_drawState == DRAW_LINE_START_POINT)
		{
			m_data[2] = dPtX;
			m_data[3] = dPtY;
		}
	}

	return TRUE;
}

BOOL LinePatternEditAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

		// here, we should simulate the "mouse input".
		MovePoint(d1, d2);
		LButtonDown(d1, d2);
	}

	ValueInputManager::GetInstance()->AcceptInputChar(nChar);

	return TRUE;
}

BOOL LinePatternEditAction::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL LinePatternEditAction::DrawBeforeFinish()
{
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();

	// keep the old color.
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	//
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());

	if (m_drawState == DRAW_LINE_START_POINT || m_editState != EDIT_LINE_NOTSTART)
		m_pViewPort->DrawLinePoint2D(m_data[0], m_data[1], m_data[2], m_data[3]);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

void LinePatternEditAction::UpdateForUndoRedo()
{
	if (m_editState != EDIT_LINE_NOTSTART && !m_pPatList->GetPatternFromID(m_pGripEditPat->GetID()))
	{
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}
}

void LinePatternEditAction::SetGripEditPattern(IPatternPtr pPattern)
{
	m_pGripEditPat = pPattern;

	LinePattern* pLinePat = dynamic_cast<LinePattern*>(pPattern.get());

	// fill the m_data with the data in line pattern.
	Point2D startPt = pLinePat->GetBasePoint();
	m_data[0] = startPt.X();
	m_data[1] = startPt.Y();

	Point2D endPt =	pLinePat->GetEndPoint();
	m_data[2] = endPt.X();
	m_data[3] = endPt.Y();
}

void LinePatternEditAction::SetGripEditPt(double dPtX, double dPtY)
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
				m_editState = EDIT_LINE_START_POINT;
				m_iMsgID = IDS_OUTPUT_DRAWLINE_STARTPT;
			}
			else if (i == 1)
			{
				m_editState = EDIT_LINE_CENTER_POINT;
				m_iMsgID = IDS_OUTPUT_NEWPOSITION;
			}
			else if (i == 2)
			{
				m_editState = EDIT_LINE_END_POINT;
				m_iMsgID = IDS_OUTPUT_DRAWLINE_ENDPT;
			}
			return;
		}
	}
}

END_CUTLEADER_NAMESPACE()
