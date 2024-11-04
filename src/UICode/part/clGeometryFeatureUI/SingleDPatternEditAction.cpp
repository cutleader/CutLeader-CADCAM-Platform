#include "StdAfx.h"
#include "SingleDPatternEditAction.h"

#include "GlViewPort.h"
#include "MathEx.h"
#include "ICommand.h"
#include "ValueInputManager.h"
#include "clUILibResource.h"
#include "CursorDrawer.h"

#include "PatternList.h"
#include "SingleDPattern.h"
#include "PatternAddCommand.h"
#include "CommandManager.h"
#include "GripEditCommand.h"
#include "PatternSelectAction.h"
#include "clGeometryFeatureResource.h"
#include "PatternPropertyBar.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

SingleDPatternEditAction::SingleDPatternEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort)
{
	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;
	m_pViewPort = pViewPort;

	m_drawState = DRAW_SINGLED_NOTSTART;
	m_editState = EDIT_SINGLED_NOTSTART;
	memset(m_data, 0, sizeof(double)*5);

	m_iMsgID = IDS_OUTPUT_PATSINGLED_CENTERPT;
}

SingleDPatternEditAction::~SingleDPatternEditAction(void)
{
}

BOOL SingleDPatternEditAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_editState != EDIT_SINGLED_NOTSTART) //EDIT mode
	{
		if (m_editState == EDIT_SINGLED_CENTER_POINT)
		{
			m_data[0] = dPtX;
			m_data[1] = dPtY;
		}
		else if(m_editState == EDIT_SINGLED_TOP_POINT)
		{
			m_data[3] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}
		else if(m_editState == EDIT_SINGLED_BOTTOM_POINT)
		{
			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}

		// create the new RECTHOLE pattern with the new data.
		Point2D basePt(m_data[0], m_data[1]);
		IPatternPtr pNewPat(new SingleDPattern(basePt, m_data[2], m_data[3], m_data[4]));

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
		if (m_drawState == DRAW_SINGLED_NOTSTART || 
		    m_drawState == DRAW_SINGLED_THIRD_POINT) //first click
		{
			m_drawState = DRAW_SINGLED_FIRST_POINT;
			m_iMsgID = IDS_OUTPUT_PATSINGLED_RADIUS;

			m_data[0] = dPtX;
			m_data[1] = dPtY;
			m_data[2] = 0;
			m_data[3] = 0;
			m_data[4] = 0;
		}
		else if(m_drawState == DRAW_SINGLED_FIRST_POINT) //second click, we can create a line
		{
			m_drawState = DRAW_SINGLED_SECOND_POINT;
			m_iMsgID = IDS_OUTPUT_PATSINGLED_WIDTH_ANGLE;

			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}
		else if(m_drawState == DRAW_SINGLED_SECOND_POINT) //second click, we can create a line
		{
			// if the chord length is longer than radius, cannot proceed.
			m_data[3] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
			if (m_data[3] > m_data[2])
			{
				m_data[3] = m_data[2];
				return FALSE;
			}

			m_drawState = DRAW_SINGLED_THIRD_POINT;
			m_iMsgID = IDS_OUTPUT_PATSINGLED_CENTERPT;

			// the angle.
			m_data[4] = MathEx::GetSignedAng(atan2(dPtY-m_data[1], dPtX-m_data[0]));

			// create single D pattern.
			Point2D basePt(m_data[0], m_data[1]);
			IPatternPtr pSingleDPat(new SingleDPattern(basePt, m_data[2], m_data[3], m_data[4]));

			// create command
			EditorData editorData = GetEditorData();
			ICommandPtr pCommand(new PatternAddCommand(editorData, m_pPatList, m_pPatternLoopList, pSingleDPat));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// enter the selecting pattern action.
			EnterPatSelAction(pSingleDPat);
		}
	}

	return TRUE;
}

BOOL SingleDPatternEditAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_editState != EDIT_SINGLED_NOTSTART) //EDIT mode
	{
		if (m_editState == EDIT_SINGLED_CENTER_POINT)
		{
			m_data[0] = dPtX;
			m_data[1] = dPtY;
		}
		else if(m_editState == EDIT_SINGLED_TOP_POINT)
		{
			m_data[3] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}
		else if(m_editState == EDIT_SINGLED_BOTTOM_POINT)
		{
			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}
	}
	else
	{
		if (m_drawState == DRAW_SINGLED_FIRST_POINT)
		{
			m_data[2] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
		}
		else if(m_drawState == DRAW_SINGLED_SECOND_POINT)
		{
			// calculate the chord length.
			m_data[3] = sqrt((dPtX - m_data[0])*(dPtX - m_data[0])+(dPtY - m_data[1])*(dPtY - m_data[1]));
			if (m_data[3] > m_data[2])
				m_data[3] = m_data[2];

			m_data[4] = MathEx::GetSignedAng(atan2(dPtY-m_data[1], dPtX-m_data[0]));
		}
	}

	return TRUE;
}

void SingleDPatternEditAction::ConvertKBInput(double& dVal1, double& dVal2)
{
	if (m_editState != EDIT_SINGLED_NOTSTART)
	{
		// under "edit" action.

		if (m_editState == EDIT_SINGLED_CENTER_POINT)
		{
			// now, do not need to convert.
			;
		}
		else if(m_editState == EDIT_SINGLED_TOP_POINT ||
				m_editState == EDIT_SINGLED_BOTTOM_POINT)
		{
			// "dVal1" holds the "width" of the Double-D

			/************************************************************************/
			// get the vector from center point to chord.

			// get the vector along the Double D angle.
			Vector2D v1;
			v1.SetAngle(m_data[4]);

			v1.Orthogonal();
			v1.Magnitude(dVal1);
			/************************************************************************/

			// get the point on the chord.
			Point2D centerPt(m_data[0], m_data[1]);
			centerPt.Transfer(v1);

			dVal1 = centerPt.X();
			dVal2 = centerPt.Y();
		}
		else if (m_editState == EDIT_SINGLED_RIGHT_POINT || m_editState == EDIT_SINGLED_LEFT_POINT)
		{
			// "dVal1" holds the "radius" of the Double-D

			// get the vector along the Double D angle.
			Vector2D v1;
			v1.SetAngle(m_data[4]);
			v1.Magnitude(dVal1);

			// get the point on the arc.
			Point2D centerPt(m_data[0], m_data[1]);
			centerPt.Transfer(v1);

			dVal1 = centerPt.X();
			dVal2 = centerPt.Y();
		}
	}
	else
	{
		// under "draw" action.

		if (m_drawState == DRAW_SINGLED_NOTSTART || m_drawState == DRAW_SINGLED_THIRD_POINT)
		{
			// now, do not need to convert.
			;
		}
		else if(m_drawState == DRAW_SINGLED_FIRST_POINT)
		{
			// "dVal1" holds the "radius" of the Double-D

			// get the vector along the Double D angle.
			Vector2D v1;
			v1.Magnitude(dVal1);

			// get the point on the arc.
			Point2D centerPt(m_data[0], m_data[1]);
			centerPt.Transfer(v1);

			dVal1 = centerPt.X();
			dVal2 = centerPt.Y();
		}
		else if(m_drawState == DRAW_SINGLED_SECOND_POINT)
		{
			// "dVal1" holds the "width" of the Double-D
			// "dVal2" holds the "angle" of the Double-D
			// notes: we should return a point and can input to "LButtonDown".

			/************************************************************************/
			// get the vector along the Double D angle.

			Vector2D v1;
			v1.SetAngle(dVal2);

			v1.Magnitude(dVal1);
			/************************************************************************/

			// get the point on the chord.
			Point2D centerPt(m_data[0], m_data[1]);
			centerPt.Transfer(v1);

			dVal1 = centerPt.X();
			dVal2 = centerPt.Y();
		}
	}
}

BOOL SingleDPatternEditAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

BOOL SingleDPatternEditAction::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL SingleDPatternEditAction::DrawBeforeFinish()
{
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();

	// keep the old color.
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	//
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());

	if (m_drawState == DRAW_SINGLED_FIRST_POINT)
	{
		m_pViewPort->DrawCircle2D(m_data[0], m_data[1], m_data[2]);		 
	}
	else if (m_drawState == DRAW_SINGLED_SECOND_POINT || m_editState != EDIT_SINGLED_NOTSTART)
	{
		double dTang = acos(m_data[3]/m_data[2]);
		double dStartAng = m_data[4] + dTang - PI/2;
		double dEndAng = m_data[4] - dTang + PI*3/2;
		m_pViewPort->DrawArc2D(m_data[0], m_data[1], m_data[2], dStartAng, dEndAng-dStartAng);

		double dChordStartX = m_data[0] + m_data[2]*cos(dEndAng);
		double dChordStartY = m_data[1] + m_data[2]*sin(dEndAng);
		double dChordEndX = m_data[0] + m_data[2]*cos(dStartAng);
		double dChordEndY = m_data[1] + m_data[2]*sin(dStartAng);
		m_pViewPort->DrawLinePoint2D(dChordStartX, dChordStartY, dChordEndX, dChordEndY);
	}

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void SingleDPatternEditAction::UpdateForUndoRedo()
{
	PatternEditActionBase::UpdateForUndoRedo();

	if (m_editState != EDIT_SINGLED_NOTSTART && !m_pPatList->GetPatternFromID(m_pGripEditPat->GetID()))
	{
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}
}

void SingleDPatternEditAction::SetGripEditPattern(IPatternPtr pPattern)
{
	m_pGripEditPat = pPattern;

	SingleDPatternPtr pSingleDPat = boost::dynamic_pointer_cast<SingleDPattern>(pPattern);

	// fill the m_data with the data in single D pattern.
	Point2D basePt = pSingleDPat->GetBasePoint();
	m_data[0] = basePt.X();
	m_data[1] = basePt.Y();

	m_data[2] =	pSingleDPat->GetRadius();
	m_data[3] =	pSingleDPat->GetToChord();
	m_data[4] =	pSingleDPat->GetAngle();
}

void SingleDPatternEditAction::SetGripEditPt(double dPtX, double dPtY)
{
	double dTang = acos(m_data[3]/m_data[2]);
	double dStartAng = m_data[4] + dTang - PI/2 ;
	double dEndAng = m_data[4] - dTang + PI*3/2;
	double dChordStartX = m_data[0] + m_data[2]*cos(dEndAng);
	double dChordStartY = m_data[1] + m_data[2]*sin(dEndAng);
	double dChordEndX = m_data[0] + m_data[2]*cos(dStartAng);
	double dChordEndY = m_data[1] + m_data[2]*sin(dStartAng);

	double dEndPtX1 = m_data[0] + m_data[2]*cos(m_data[4]);
	double dEndPtY1 = m_data[1] + m_data[2]*sin(m_data[4]);

	double dEndPtX2 = m_data[0] + m_data[2]*cos(m_data[4] + PI/2.0);
	double dEndPtY2 = m_data[1] + m_data[2]*sin(m_data[4] + PI/2.0);

	double dEndPtX3 = m_data[0] + m_data[2]*cos(m_data[4] + PI);
	double dEndPtY3 = m_data[1] + m_data[2]*sin(m_data[4] + PI);

	if (MathEx::PointEqual(dPtX, dPtY, dChordStartX, dChordStartY) ||
		MathEx::PointEqual(dPtX, dPtY, dChordEndX, dChordEndY) ||
		MathEx::PointEqual(dPtX, dPtY, dEndPtX1, dEndPtY1) ||
		MathEx::PointEqual(dPtX, dPtY, dEndPtX2, dEndPtY2) ||
		MathEx::PointEqual(dPtX, dPtY, dEndPtX3, dEndPtY3))
	{
		m_editState = EDIT_SINGLED_BOTTOM_POINT;
		m_iMsgID = IDS_OUTPUT_PATSINGLED_RADIUS;
	}
	else if(MathEx::PointEqual(dPtX, dPtY, 
		(dChordStartX + dChordEndX)/2.0, 
		(dChordStartY + dChordEndY)/2.0))
	{
		m_editState = EDIT_SINGLED_TOP_POINT;
		m_iMsgID = IDS_OUTPUT_PATSINGLED_WIDTH;
	}
	else if(MathEx::PointEqual(dPtX, dPtY, m_data[0], m_data[1]))
	{
		m_editState = EDIT_SINGLED_CENTER_POINT;
		m_iMsgID = IDS_OUTPUT_PATSINGLED_CENTERPT;
	}
}

END_CUTLEADER_NAMESPACE()
