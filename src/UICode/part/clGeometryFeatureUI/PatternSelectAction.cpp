#include "StdAfx.h"
#include "PatternSelectAction.h"

#include "MathEx.h"
#include "GlViewPort.h"
#include "Rect2D.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "clUILibResource.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "Point2DList.h"
#include "clBaseDataResource.h"

#include "PatternDrawer.h"
#include "SnapPtDrawer.h"
#include "PatternPropertyBar.h"
#include "PatternDeleteCommand.h"
#include "IPattern.h"
#include "IArrayPattern.h"
#include "PatternList.h"
#include "PatternLoopList.h"
#include "clGeometryFeatureResource.h"
#include "LinePatternEditAction.h"
#include "ArcPatternEditAction.h"
#include "ArcPattern3PtEditAction.h"
#include "MultiLinePatternEditAction.h"
#include "RectLinesDrawAction.h"
#include "RoundCornerHolePatternEditAction.h"
#include "EllipsePatternEditAction.h"
#include "RoundCornerLineArcsDrawAction.h"
#include "RectHolePatternEditAction.h"
#include "CirclePatternEditAction.h"
#include "SingleDPatternEditAction.h"
#include "DoubleDPatternEditAction.h"
#include "LineArrayPatternEditAction.h"
#include "ArcArrayPatternEditAction.h"
#include "CircleArrayPatternEditAction.h"
#include "GridArrayPatternEditAction.h"
#include "TriangleHolePatternEditAction.h"
#include "OBroundPatternEditAction.h"
#include "RadSlotPatternEditAction.h"
#include "DrawAssistor.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternSelectAction::PatternSelectAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort)
{
	m_bSupportDrawAssit = FALSE;

	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;
	m_pViewPort = pViewPort;

	m_pActivePat.reset();
	m_pSelectedPats.reset(new PatternList);
	m_pSnapPt.reset();

	m_state = DRAW_SELECT_NONE;

	m_bDisplayValInput = FALSE;
	m_iMsgID = IDS_OUTPUT_SELECTPATTERN;
}

PatternSelectAction::~PatternSelectAction(void)
{
}

BOOL PatternSelectAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (CheckGripEdit(dPtX, dPtY))
	{
		// if the click drives "grip edit", "grip edit" action will be 
		// the "next" action.
		Point2D snapPt(m_pSnapPt->X(), m_pSnapPt->Y());
		SwitchToGripEdit(m_pActivePat, snapPt);

		return TRUE;
	}

	if (m_state == DRAW_SELECT_NONE || m_state == DRAW_SELECT_SECOND_INPUT)
	{
		m_state = DRAW_SELECT_FIRST_INPUT;
		m_iMsgID = IDS_OUTPUT_SELECTPAT_NEXTPT;

		m_data[0] = dPtX;
		m_data[1] = dPtY;
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL PatternSelectAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_state == DRAW_SELECT_FIRST_INPUT)
	{
		m_state = DRAW_SELECT_SECOND_INPUT;
		m_iMsgID = IDS_OUTPUT_SELECTPATTERN;

		m_data[2] = dPtX;
		m_data[3] = dPtY;

		// calc new selected patterns.
		PatternListPtr pSelPatList = CalcSelectPat();
		if (pSelPatList->size() > 0)
		{
			// if "ctrl" key is down, we should not clear the previous selection.
			if (!ValueInputManager::GetInstance()->IsInputCtrl())
				m_pSelectedPats->clear();

			// update all selected patterns.
			m_pSelectedPats->AddPatterns(pSelPatList);

			// display and update PatternPropertyBar.
			PatternPropertyBar::GetInstance()->DisplayPatProp(m_pPatternLoopList, m_pSelectedPats);
		}
		else
		{
			// reset the status.
			m_state = DRAW_SELECT_NONE;
		}
	}

	return TRUE;
}

BOOL PatternSelectAction::LButtonDBClk(double& dPtX, double& dPtY)
{
	m_state = DRAW_SELECT_SECOND_INPUT;
	m_iMsgID = IDS_OUTPUT_SELECTPATTERN;

	m_data[0] = dPtX;
	m_data[1] = dPtY;
	m_data[2] = dPtX;
	m_data[3] = dPtY;

	// after left button up, select patterns.
	CalcSelectPat();

	// display and update PatternPropertyBar.
	PatternPropertyBar* pPatPropBar = PatternPropertyBar::GetInstance();
	pPatPropBar->DisplayPatProp(m_pPatternLoopList, m_pSelectedPats);

	return TRUE;
}

void PatternSelectAction::SelectPats(PatternListPtr pPatList, BOOL bShowPropBar)
{
	m_pSelectedPats = pPatList;
	m_state = DRAW_SELECT_SECOND_INPUT;

	// whether need to show prop bar.
	PatternPropertyBar* pPatternPropBar = PatternPropertyBar::GetInstance();
	if (bShowPropBar && !pPatternPropBar->IsVisible())
		pPatternPropBar->ShowPane(TRUE, FALSE, TRUE);

	// if PatternPropertyBar is visible, we should update it.
	PatternPropertyBar* pPatPropBar = PatternPropertyBar::GetInstance();
	if (pPatPropBar->IsVisible())
		pPatPropBar->DisplayPatProp(m_pPatternLoopList, m_pSelectedPats);
}

BOOL PatternSelectAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// get the nearest pattern.
	double dDistance = .0;
	m_pActivePat = m_pPatternLoopList->GetClosestPat(pt, dDistance);

	// we should support intelli-snap for the selected pattern.
	if (m_pSelectedPats->size() > 0)
		CheckSnapPt(dPtX, dPtY);

	// if left button down, we should draw a rectangle in stipple mode.
	if (m_state == DRAW_SELECT_FIRST_INPUT)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL PatternSelectAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PatternEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputDel(nChar))
	{
		DeleteSelectedPats();
	}

	return TRUE;
}

BOOL PatternSelectAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();
	PatternDrawer patDrawer(m_pViewPort);
	SnapPtDrawer snapPtDrawer(m_pViewPort);

	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	// if left button down, we should draw a rectangle in stipple mode.
	if (m_state == DRAW_SELECT_FIRST_INPUT)
	{
		// set the old stipple mode.
		bool oldStipple = false;
		int oldRepeat = 1;
		unsigned short oldPattern = 0xffff;
		m_pViewPort->GetLineStipple(oldStipple, oldRepeat, oldPattern);

		// draw the selection rectangle.
		m_pViewPort->SetLineStipple(true, 2, 0xcccc);
		m_pViewPort->DrawRect(m_data[0], m_data[1], m_data[2], m_data[3]);

		// restore the drawer
		m_pViewPort->SetLineStipple(oldStipple, oldRepeat, oldPattern);
	}

	// draw the "active" pattern in highlight mode.
	// notes:
	//   1) as long as we move the mouse, we need to highlight the active pattern.
	//   2) if the active pattern has been selected, do not highlight.
	if (m_pActivePat && m_pSelectedPats->GetPatternIndex(m_pActivePat.get()) == INVALID_IDX)
    {
		patDrawer.DrawActivePat(m_pActivePat);
    }

	// draw the selected patterns and the control points of selected patterns.
	patDrawer.DrawSelPatList(m_pSelectedPats);
	snapPtDrawer.DrawPatListCtrlPt(m_pSelectedPats);

	// draw the snap point.
	// notes:
	//   1) use the color for the selected object.
	if (m_pSnapPt)
    {
		snapPtDrawer.DrawSnapPt(m_pSnapPt, *pColorInfo->GetDrawAssistColor());
    }

	// draw cursor.
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

void PatternSelectAction::UpdateForUndoRedo()
{
	// hide PatternPropertyBar.
	PatternPropertyBar::GetInstance()->ShowBar(FALSE);

	// if the active pattern is lost, reset it.
	if (m_pActivePat && !m_pPatList->GetPatternFromID(m_pActivePat->GetID()))
		m_pActivePat.reset();

	// if some selected pattern are lost, remove it from "m_pSelectedPats".
	PatternListPtr m_pLostPats(new PatternList);
	for (unsigned int i = 0; i < m_pSelectedPats->size(); i++)
	{
		if (!m_pPatList->GetPatternFromID(m_pSelectedPats->at(i)->GetID()))
			m_pLostPats->push_back(m_pSelectedPats->at(i));
	}
	m_pSelectedPats->RemovePatterns(m_pLostPats.get());
}

BOOL PatternSelectAction::CheckSnapPt(double dPtX, double dPtY)
{
	Point2D tmpPt(dPtX, dPtY);

	// get the snap range.
	double dCaptureRange = SNAP_DISTANCE;
	m_pViewPort->DToW(dCaptureRange);

	// go through each selected pattern.
	for (unsigned int j = 0; j < m_pSelectedPats->size(); j++)
	{
		IPatternPtr pPattern = m_pSelectedPats->at(j);

		// get snap points of the pattern
		Point2DList pts;
		pPattern->GetSnapPoints(pts);

		// go through each snap point, and check which one is "caught".
		for (unsigned int i = 0; i < pts.size(); i++)
		{
			Point2D pt = (Point2D)pts[i];
			double tmpDis = tmpPt.DistanceTo(pt);
			if (tmpDis < dCaptureRange)
			{
				m_pSnapPt.reset(new Point2D);
				m_pSnapPt->X(pt.X());
				m_pSnapPt->Y(pt.Y());

				return TRUE;
			}
		}
	}

	// cannot find a snap point.
	m_pSnapPt.reset();
	return FALSE;
}

PatternListPtr PatternSelectAction::CalcSelectPat()
{
	PatternListPtr pSelPatList(new PatternList);

	// if left button down and up are at same position. only one pattern will be selected.
	if (MathEx::Equal(m_data[0], m_data[2]) && MathEx::Equal(m_data[1], m_data[3]))
	{
		Point2D pt(m_data[0], m_data[1]);
		double dDistance = .0;
		IPatternPtr pPattern = m_pPatternLoopList->GetClosestPat(pt, dDistance);
		if (pPattern)
			pSelPatList->push_back(pPattern);
	}
	else
	{
		// here, you know, we should select pattern with the rectangle range.
		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);

		pSelPatList = m_pPatList->GetPatternInRect(rect);
	}

	return pSelPatList;
}

BOOL PatternSelectAction::CheckGripEdit(double& dPtX, double& dPtY)
{
	// if no pattern selected, of course, cannot enter "grip edit" action.
	if (m_pSelectedPats->size() == 0)
		return FALSE;
	else
	{
		// here, we should check whether we "caught" a snap point.
		if (CheckSnapPt(dPtX, dPtY))
			return TRUE;
	}

	return FALSE;
}

void PatternSelectAction::SwitchToGripEdit(IPatternPtr pPattern, Point2D snapPt)
{
	PatternType emPatternType = pPattern->GetPatternType();

	// for polygon patterns.
	if (emPatternType == PATTERN_LINE)
		m_pNextAction.reset(new LinePatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_ARC)
		m_pNextAction.reset(new ArcPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	// for std patterns.
	else if(emPatternType == PATTERN_RECT_HOLE)
		m_pNextAction.reset(new RectHolePatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_TR_HOLE)
		m_pNextAction.reset(new TriangleHolePatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_ROUNDHOLE)
		m_pNextAction.reset(new CirclePatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_OBROUND)
		m_pNextAction.reset(new OBroundPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_ELLIPSE)
		m_pNextAction.reset(new EllipsePatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_SINGLED)
		m_pNextAction.reset(new SingleDPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_DOUBLED)
		m_pNextAction.reset(new DoubleDPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_RCHOLE)
		m_pNextAction.reset(new RoundCornerHolePatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_RADSLOT)
		m_pNextAction.reset(new RadSlotPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	// for grid patterns.
	else if(emPatternType == PATTERN_PATTERNLINE)
		m_pNextAction.reset(new LineArrayPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_PATTERNARC)
		m_pNextAction.reset(new ArcArrayPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_PATTERNCIRCLE)
		m_pNextAction.reset(new CircleArrayPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	else if(emPatternType == PATTERN_PATTERNGRID)
		m_pNextAction.reset(new GridArrayPatternEditAction(m_pPatList, m_pPatternLoopList, m_pViewPort));

	// init the data for Grip-edit.
	IPatternGripEditAction* pGripEditAction = dynamic_cast<IPatternGripEditAction*>(m_pNextAction.get());
	pGripEditAction->SetGripEditPattern(pPattern);
	pGripEditAction->SetGripEditPt(snapPt.X(), snapPt.Y());
}

void PatternSelectAction::DeleteSelectedPats()
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

		// 隐藏图形属性界面，这个隐藏动作会激发mousemove事件，这样就会更新m_pActivePat变量。
		PatternPropertyBar::GetInstance()->ShowBar(FALSE);
	}
}

END_CUTLEADER_NAMESPACE()
