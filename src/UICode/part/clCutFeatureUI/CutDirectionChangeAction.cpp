#include "StdAfx.h"
#include "CutDirectionChangeAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "DummyAction.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"

#include "PatternList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "LoopDrawer.h"

#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopStartCutData.h"
#include "clCutFeatureResource.h"
#include "CutDirectionChangeCommand.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

CutDirectionChangeAction::CutDirectionChangeAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort)
    : CutFeatureEditBase(pPatList, pPatternLoopList, pLoopFeatureList, pViewPort)
{
	m_iMsgID = IDS_CHANGE_DIRECTION;
}

CutDirectionChangeAction::~CutDirectionChangeAction(void)
{
}

BOOL CutDirectionChangeAction::LButtonDown(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_pActiveLoop)
	{
		// get the pierce info of the selected loop.
		LoopCutFeaturePtr pLoopFeature = m_pLoopFeatureList->FindByPatternLoop(m_pActiveLoop);
		LoopStartCutDataPtr pLoopStartCutData = pLoopFeature->GetLoopStartCutData();

		// create the command to modify the start point of the loop.
		EditorData editorData = GetEditorData();
		ICommandPtr pCommand(new CutDirectionChangeCommand(editorData, pLoopStartCutData));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}

	return TRUE;
}

BOOL CutDirectionChangeAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// get the loop which is nearest to the cursor.
	IPatternPtr pPattern = m_pPatList->GetPatternClosestTo(pt);
	if (pPattern)
    {
		m_pActiveLoop = m_pPatternLoopList->FindLoopByPat(pPattern.get());
    }

	return TRUE;
}

BOOL CutDirectionChangeAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CutFeatureEditBase::OnKeyDown(nChar, nRepCnt, nFlags);

	return TRUE;
}

BOOL CutDirectionChangeAction::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL CutDirectionChangeAction::DrawBeforeFinish()
{
	// get the color setting.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();

	// keep the old setting.
	COLORREF dwOldColor;
	ROP_MODE ropMode;
	m_pViewPort->GetDrawColor(dwOldColor);
	m_pViewPort->GetROP(ropMode);

	/************************************************************************/
	// draw the selected loop in highlight mode.

	if (m_pActiveLoop)
	{
		m_pViewPort->SetROP(ROP_COPY);
		m_pViewPort->SetLineWidth(2);
		m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());

		// draw the loop.
		LoopDrawer loopDrawer(m_pViewPort);
		loopDrawer.DrawLoopData(m_pActiveLoop);

		m_pViewPort->SetLineWidth(1);
	}
	/************************************************************************/

	// restore the draw setting.
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetROP(ropMode);

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
