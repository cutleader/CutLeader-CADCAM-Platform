#include "StdAfx.h"
#include "ArcPattern3PtEditAction.h"

#include "MathEx.h"
#include "GlViewPort.h"
#include "Point2D.h"
#include "CommandManager.h"
#include "ICommand.h"
#include "ValueInputManager.h"
#include "clUILibResource.h"
#include "CursorDrawer.h"

#include "ArcPattern.h"
#include "ArcPatternEditAction.h"
#include "PatternAddCommand.h"
#include "PatternSelectAction.h"
#include "clGeometryFeatureResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

Arc3PtPatEditor::Arc3PtPatEditor(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort)
{
	m_iStatus = DRAW_ARC3P_NOTSTART;
	memset(m_data, 0, sizeof(double)*5);

	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;
	m_pViewPort = pViewPort;

	m_iMsgID = IDS_OUTPUT_PATARC3PT_FIRSTPT;
}

Arc3PtPatEditor::~Arc3PtPatEditor(void)
{
}

BOOL Arc3PtPatEditor::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_iStatus == DRAW_ARC3P_NOTSTART || 
		m_iStatus == DRAW_ARC3P_END_POINT)
	{
		m_iStatus = DRAW_ARC3P_START_POINT;
		m_iMsgID = IDS_OUTPUT_PATARC3PT_SECONDPT;

		m_data[0] = dPtX;
		m_data[1] = dPtY;
		m_data[2] = dPtX;
		m_data[3] = dPtY;
		m_data[4] = dPtX;
		m_data[5] = dPtY;
	}
	else if(m_iStatus == DRAW_ARC3P_START_POINT)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;
		m_data[4] = dPtX;
		m_data[5] = dPtY;

		m_iStatus = DRAW_ARC3P_MID_POINT;
		m_iMsgID = IDS_OUTPUT_PATARC3PT_THIRDPT;
	}
	else if(m_iStatus == DRAW_ARC3P_MID_POINT)
	{
		m_data[4] = dPtX;
		m_data[5] = dPtY;

		// calculate arc form 3 points
		double dCenterX = 0, dCenterY = 0, dRadius = 0, dStartAngle = 0, dMoveAngle = 0;
		MathEx::GetArcFrom3Point(m_data[0], m_data[1], m_data[2], m_data[3], m_data[4], m_data[5],
								 dCenterX, dCenterY, dRadius, dStartAngle, dMoveAngle);

		// check whether 3 pts are on a line.
		if (dRadius == 0)
			return FALSE;

		m_iStatus = DRAW_ARC3P_END_POINT;
		m_iMsgID = IDS_OUTPUT_PATARC3PT_FIRSTPT;

		// create arc pattern.
		Point2D centerPt(dCenterX, dCenterY);
		IPatternPtr pArcPat(new ArcPattern(centerPt, dRadius, dStartAngle, dMoveAngle));

		/************************************************************************/	
		// create command.

		EditorData editorData = GetEditorData();
		ICommandPtr pCommand(new PatternAddCommand(editorData, m_pPatList, m_pPatternLoopList, pArcPat));
		pCommand->Do();

		// save the command.
		CommandManager::GetInstance()->AddCommand(pCommand);
		/************************************************************************/
	}

	return TRUE;
}

BOOL Arc3PtPatEditor::MovePoint(double& dPtX, double& dPtY)
{
	if (m_iStatus == DRAW_ARC3P_START_POINT)  
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;
		m_data[4] = dPtX;
		m_data[5] = dPtY;
	}
	else if(m_iStatus == DRAW_ARC3P_MID_POINT)  
	{
		m_data[4] = dPtX;
		m_data[5] = dPtY;
	}

	return TRUE;
}

BOOL Arc3PtPatEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ValueInputManager* pValInputMgr = ValueInputManager::GetInstance();

	// get the user input from output bar.
	double d1, d2;
	if (pValInputMgr->IsInputEnd(nChar))
	{
		if (!pValInputMgr->GetInput(d1, d2))
		{
			// user just presses the "Enter" key without inputting value.
			m_pViewPort->GetCursorPos(d1, d2);
		}

		// here, we should simulate the "mouse input".
		MovePoint(d1, d2);
		LButtonDown(d1, d2);
	}
	else if (pValInputMgr->IsInputCancel(nChar))
	{
		// figure out the next action.
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	}

	pValInputMgr->AcceptInputChar(nChar);

	return TRUE;
}

BOOL Arc3PtPatEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL Arc3PtPatEditor::DrawBeforeFinish()
{
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();

	// keep the old color.
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	//
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());

	// we should draw line after the first click
	if (m_iStatus == DRAW_ARC3P_START_POINT)  
	{
		m_pViewPort->DrawLinePoint2D(m_data[0], m_data[1], m_data[2], m_data[3]);
	}
	// we should draw arc after second click.
	else if(m_iStatus == DRAW_ARC3P_MID_POINT)  
	{
		double centerX = 0;
		double centerY = 0; 
		double radius = 0;
		double startAngle = 0;
		double moveAngle = 0;

		// calculate arc form 3 points
		MathEx::GetArcFrom3Point(m_data[0], m_data[1], m_data[2], m_data[3], m_data[4], m_data[5],
								 centerX, centerY, radius, startAngle, moveAngle);

		m_pViewPort->DrawArc2D(centerX, centerY, radius, startAngle, moveAngle);
	}

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
