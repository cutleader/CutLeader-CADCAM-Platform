#include "StdAfx.h"
#include "GeometryFeatureEditor.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "ZoomBaseAction.h"
#include "ZoomInAction.h"
#include "ZoomOutAction.h"
#include "DummyAction.h"
#include "clBaseDataResource.h"

#include "clGeometryFeatureResource.h"
#include "IPattern.h"
#include "DrawAssistor.h"
#include "PatternList.h"
#include "PatternPropertyBar.h"
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

#include "PatternCopyAction.h"
#include "PatternSelectAction.h"
#include "PatternExplodeAction.h"
#include "PatternDeleteAction.h"
#include "PatternMoveAction.h"
#include "PatternRotateAction.h"
#include "PatternLoopOffsetAction.h"

BEGIN_CUTLEADER_NAMESPACE()

GeometryFeatureEditor::GeometryFeatureEditor(GlViewPortPtr pViewPort, DrawAssistorPtr pDrawAssistor)
{
	m_pViewPort = pViewPort;
	m_pAction.reset(new DummyAction(m_pViewPort));

	m_pDrawAssistor = pDrawAssistor;
}

GeometryFeatureEditor::~GeometryFeatureEditor(void)
{
}

BOOL GeometryFeatureEditor::LButtonDown(double& dPtX, double& dPtY)
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

BOOL GeometryFeatureEditor::LButtonUp(double& dPtX, double& dPtY)
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

BOOL GeometryFeatureEditor::LButtonDBClk(double& dPtX, double& dPtY)
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

BOOL GeometryFeatureEditor::RButtonUp(double& dPtX, double& dPtY)
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

BOOL GeometryFeatureEditor::MovePoint(double& dPtX, double& dPtY)
{
	if (m_pAction)
	{
		// whether the action support "draw assist".
		IPatternEditAction* pPatManAction = dynamic_cast<IPatternEditAction*>(m_pAction.get());
		if (pPatManAction)
		{
			if (pPatManAction->SupportDrawAssit())
			{
				Point2DPtr ptPtr = m_pAction->GetFirstInput();

				m_pDrawAssistor->Init(m_pPatList, ptPtr);
				m_pDrawAssistor->RefreshAssistor(dPtX, dPtY);
			}
			else
            {
				m_pDrawAssistor->Reset();
            }
		}

		m_pAction->MovePoint(dPtX, dPtY);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
        {
			m_pAction = pNextAction;
        }

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL GeometryFeatureEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pAction)
	{
		m_pAction->OnKeyDown(nChar, nRepCnt, nFlags);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

BOOL GeometryFeatureEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pAction)
	{
		m_pAction->OnKeyUp(nChar, nRepCnt, nFlags);

		// check whether we will enter other action.
		IActionPtr pNextAction = m_pAction->GetNextAction();
		if (pNextAction)
			m_pAction = pNextAction;

		UpdateOutputBar();
	}

	return TRUE;
}

CString GeometryFeatureEditor::GetEditorName()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY);
	return str;
}

BOOL GeometryFeatureEditor::SetDrawAction(DRAW_ACTION_TYPE drawType)
{
	GlViewPortPtr pViewPort = m_pViewPort;

	// hide pattern prop bar.
	PatternPropertyBar::GetInstance()->ShowPane(FALSE, FALSE, TRUE);

	if (drawType == DRAW_LINE)
		m_pAction.reset(new LinePatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_MULTILINE)
		m_pAction.reset(new MultiLinePatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_RECTLINES)
		m_pAction.reset(new RectLinesDrawAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_RECT_HOLE)
		m_pAction.reset(new RectHolePatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_TR_HOLE)
		m_pAction.reset(new TriangleHolePatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_ROUNDHOLE)
		m_pAction.reset(new CirclePatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_OBROUND)
		m_pAction.reset(new OBroundPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_ELLIPSE)
		m_pAction.reset(new EllipsePatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_ARC_STD)
		m_pAction.reset(new ArcPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_ARC_3PT)
		m_pAction.reset(new Arc3PtPatEditor(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_SINGLED)
		m_pAction.reset(new SingleDPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_DOUBLED)
		m_pAction.reset(new DoubleDPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_RC_HOLE)
		m_pAction.reset(new RoundCornerHolePatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_RC_LINES)
		m_pAction.reset(new RoundCornerLineArcsDrawAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_PATTERNLINE)
		m_pAction.reset(new LineArrayPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_PATTERNARC)
		m_pAction.reset(new ArcArrayPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_PATTERNCIRCLE)
		m_pAction.reset(new CircleArrayPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_PATTERNGRID)
		m_pAction.reset(new GridArrayPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));
	else if(drawType == DRAW_RADSLOT)
		m_pAction.reset(new RadSlotPatternEditAction(m_pPatList, m_pPatternLoopList, pViewPort));

	return TRUE;
}

BOOL GeometryFeatureEditor::SetViewAction(VIEW_ACTION_TYPE viewType)
{
	// hide pattern prop bar.
	PatternPropertyBar::GetInstance()->ShowPane(FALSE, FALSE, TRUE);

	if (viewType == ACTION_ZOOM_IN)
	{
		m_pAction.reset(new ZoomInAction(m_pViewPort));
	}
	else if(viewType == ACTION_ZOOM_OUT)
	{
		m_pAction.reset(new ZoomOutAction(m_pViewPort));
	}

	return TRUE;
}

BOOL GeometryFeatureEditor::SetEditAction(EDIT_ACTION_TYPE editType)
{
	GlViewPortPtr pViewPort = m_pViewPort;

	// hide pattern prop bar.
	PatternPropertyBar::GetInstance()->ShowPane(FALSE, FALSE, TRUE);

	if (editType == EDIT_SELECTPATTERN)
	{
		m_pAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, pViewPort));
		return TRUE;
	}

	// whether some pattern were selected.
	PatternListPtr pSelPats(new PatternList);
	PatternSelectActionPtr pPatSelectAction = boost::dynamic_pointer_cast<PatternSelectAction>(m_pAction);
	if (pPatSelectAction)
		pSelPats = pPatSelectAction->GetSelectedPats();

	if (editType == EDIT_COPY)
	{
		m_pAction.reset(new PatternCopyAction(m_pPatList, m_pPatternLoopList, pViewPort, pSelPats));
	}
	else if (editType == EDIT_DELETE)
	{
		PatternDeleteActionPtr pPatDeleteAction(new PatternDeleteAction(m_pPatList, m_pPatternLoopList, pViewPort, pSelPats));

		// delete the selected patterns, if no one selected, do nothing.
		pPatDeleteAction->DeleteSelectedPats();

		m_pAction = pPatDeleteAction;
	}
	else if (editType == EDIT_EXPLODE)
	{
		PatternExplodeActionPtr pExplodePatAction(new PatternExplodeAction(m_pPatList, m_pPatternLoopList, pViewPort));

		// if select a std/array pattern, explode it.
		if (pSelPats->size() == 1 && (pSelPats->at(0)->IsArrayPattern() || pSelPats->at(0)->IsStdPattern()))
			pExplodePatAction->ExplodePat(pSelPats->at(0));

		m_pAction = pExplodePatAction;
	}
	else if (editType == EDIT_MOVE)
	{
		m_pAction.reset(new PatternMoveAction(m_pPatList, m_pPatternLoopList, pViewPort, pSelPats));
	}
	else if (editType == EDIT_ROTATE)
	{
		// we cannot rotate array patterns.
		for (unsigned int i = 0; i < pSelPats->size(); i++)
		{
			if (pSelPats->at(i)->IsArrayPattern())
			{
				CString strMsg, strProductName;
				strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_ROTATEPAT_ARRAY);
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
				return TRUE;
			}
		}

		m_pAction.reset(new PatternRotateAction(m_pPatList, m_pPatternLoopList, pViewPort, pSelPats));
	}
	else if (editType == EDIT_OFFSET)
	{
		m_pAction.reset(new PatternLoopOffsetAction(m_pPatList, m_pPatternLoopList, pViewPort));
	}

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
