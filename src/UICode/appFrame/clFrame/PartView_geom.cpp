#include "StdAfx.h"
#include "PartView.h"

#include "InteractBar.h"
#include "ProgressData.h"
#include "ProgressDlg.h"

#include "PatternList.h"
#include "PatternLoopList.h"
#include "GeometryFeatureEditor.h"
#include "PatternPropertyBar.h"
#include "LoopOptTask.h"
#include "clGeometryFeatureResource.h"
#include "GeometryOptimizeDlg.h"

#include "PartCadData.h"
#include "PartItem.h"

#include "UIManager.h"

#include "CLOptionInfo.h"
#include "ImpExpConfig.h"

BEGIN_CUTLEADER_NAMESPACE()

void PartView::OnDrawStandardline()
{
	SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_LINE);
}

void PartView::OnDrawMultiplelines()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_MULTILINE);
}

void PartView::OnDrawRectanglelines()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_RECTLINES);
}

void PartView::OnDrawRectangleHole()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_RECT_HOLE);
}

void PartView::OnDrawTriangleHole()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_TR_HOLE);
}

void PartView::OnDrawArcStandardarc()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_ARC_STD);
}

void PartView::OnDrawArc3pointsarc()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_ARC_3PT);
}

void PartView::OnDrawRCHole()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_RC_HOLE);
}

void PartView::OnDrawRCLines()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_RC_LINES);
}

void PartView::OnDrawRoundHole()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_ROUNDHOLE);
}

void PartView::OnDrawOBround()
{
	m_pGeometryEditor->SetDrawAction(DRAW_OBROUND);
	m_pCurEditor = m_pGeometryEditor;
}

void PartView::OnDrawEllipse()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_ELLIPSE);
}

void PartView::OnDrawSingleD()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_SINGLED);
}

void PartView::OnDrawDoubleD()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_DOUBLED);
}

void PartView::OnDrawPatternLine()
{
	m_pGeometryEditor->SetDrawAction(DRAW_PATTERNLINE);
	m_pCurEditor = m_pGeometryEditor;
}

void PartView::OnDrawPatternArc()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_PATTERNARC);
}

void PartView::OnDrawPatternCircle()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_PATTERNCIRCLE);
}

void PartView::OnDrawPatternGrid()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_PATTERNGRID);
}

void PartView::OnDrawRadSlot()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetDrawAction(DRAW_RADSLOT);
}

void PartView::OnZoomAll()
{
	// 客户区。
	CRect wndRect;
	GetClientRect(&wndRect);

	PartCadDataPtr pPartCadData = GetCurrentPart()->GetCadData();
	PatternListPtr pPatList = pPartCadData->GetPatternList();
	if (pPatList->size() > 0)
	{
		// 图形的外围矩形。
		Rect2D geomRect = pPartCadData->GetPatternLoopList()->GetRectBox();

		// 设置绘图区域。
		double dLeftBottomPtX = .0, dLeftBottomPtY = .0;
		double dXDirRange = m_pViewPort->GetFitAllParam(wndRect.Width(), wndRect.Height(), geomRect.GetXMin(), geomRect.GetXMax(),
														geomRect.GetYMin(), geomRect.GetYMax(), 1.2, dLeftBottomPtX, dLeftBottomPtY);
		m_pViewPort->SetDrawingArea(1.1 * dXDirRange, wndRect.Width(), wndRect.Height(), dLeftBottomPtX, dLeftBottomPtY);
	}
	else
	{
		// 设置绘图区域。
		double dLeftBottomPtX = .0, dLeftBottomPtY = .0;
		double dXDirRange = m_pViewPort->GetFitAllParam(wndRect.Width(), wndRect.Height(), 0, 10.0, 0, 10.0, 1.2, dLeftBottomPtX, dLeftBottomPtY);
		m_pViewPort->SetDrawingArea(1.1 * dXDirRange, wndRect.Width(), wndRect.Height(), dLeftBottomPtX, dLeftBottomPtY);
	}

	Invalidate();
}

void PartView::OnZoomIn()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetViewAction(ACTION_ZOOM_IN);

	Invalidate();
}

void PartView::OnZoomOut()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetViewAction(ACTION_ZOOM_OUT);

	Invalidate();
}

void PartView::OnSelectPattern()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetEditAction(EDIT_SELECTPATTERN);
}

void PartView::OnDeletePattern()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetEditAction(EDIT_DELETE);

	Invalidate();
}

void PartView::OnExplodeGridPat()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetEditAction(EDIT_EXPLODE);

	Invalidate();
}

void PartView::OnCopyPattern()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetEditAction(EDIT_COPY);
}

void PartView::OnMovePattern()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetEditAction(EDIT_MOVE);
}

void PartView::OnRotatePattern()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetEditAction(EDIT_ROTATE);
}

void PartView::OnOptimizePattern()
{
	// get the pattern and loop list of the part.
	PartItemPtr pPartItem = GetCurrentPart();
	PartCadDataPtr pPartCadData = pPartItem->GetCadData();
	PatternListPtr pPatList = pPartCadData->GetPatternList();
	PatternLoopListPtr pPatternLoopList = pPartCadData->GetPatternLoopList();

	// 参数设置。
	ImpExpConfigPtr pImpExpConfig = CLOptionInfo::GetInstance()->GetImpExpConfig();
	GeometryOptimizeDlg dlg;
	dlg.SetdAllowOffset(pImpExpConfig->GetAllowOffset());
	if (dlg.DoModal() != IDOK)
		return;

	/************************************************************************/
	// create a task to optimize the loop.

	// init the task.
	double dAllowOffset = dlg.GetdAllowOffset();
	LoopOptTaskPtr pLoopOptTask(new LoopOptTask(pPatList, pPatternLoopList, dAllowOffset));

	// display the progress of the task.
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_OPT_TASK);
	ProgressDlg progressDlg;
	progressDlg.Init(str, pLoopOptTask);
	progressDlg.DoModal();
	/************************************************************************/

	// 通知图形集合的所有观察者。
	pPatList->NotifyAllObserver(INVALID_ID, VARIANT());

	// 设置当前动作为“空动作”。
	m_pGeometryEditor->SetDummyAction();
	Invalidate();
}

void PartView::OnOffsetLoop()
{
    SwitchEditor(m_pGeometryEditor);
	m_pGeometryEditor->SetEditAction(EDIT_OFFSET);
}

END_CUTLEADER_NAMESPACE()
