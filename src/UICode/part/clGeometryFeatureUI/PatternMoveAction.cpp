#include "StdAfx.h"
#include "PatternMoveAction.h"

#include "baseConst.h"
#include "CommandManager.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"

#include "clGeometryFeatureResource.h"
#include "PatternTransformCommand.h"
#include "PatternDrawer.h"
#include "PatternList.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternMoveAction::PatternMoveAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList,
									  GlViewPortPtr pViewPort, PatternListPtr pSelPats)
{
	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;

	m_pViewPort = pViewPort;
	m_pSelectedPats = pSelPats;

	// if some patterns were pre-selected, disable PatternPreSelectAction.
	if (pSelPats->size() > 0)
		m_preSelectState = PRE_SELECT_INACTIVE;
	else
	{
		m_bSupportDrawAssit = FALSE;
	}

	m_bDisplayValInput = TRUE;
}

PatternMoveAction::~PatternMoveAction(void)
{
}

CString PatternMoveAction::GetActionName()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMAND_MOVE_PAT);

	return str;
}

void PatternMoveAction::SetMatrixValue()
{
	m_modifyPatMat.Reset();

	Vector2D vect;
	vect.X((m_dSecondPtX - m_dFirstPtX));
	vect.Y((m_dSecondPtY - m_dFirstPtY));

	m_modifyPatMat.Transfer(vect);
}

void PatternMoveAction::ConvertKBInput(double& dVal1, double& dVal2)
{
	// here, we do not need to convert.
}

int PatternMoveAction::GetMsgID()
{
	int iMsgID = INVALID_ID;

	if (m_actionState == TRANSFORM_PATTERN_NOTSTART || m_actionState == TRANSFORM_PATTERN_SECOND_POINT)
	{
		iMsgID = IDS_OUTPUT_MOVEPAT_BASEPT;
	}
	else if (m_actionState == TRANSFORM_PATTERN_BASE_POINT)
	{
		iMsgID = IDS_OUTPUT_MOVEPAT_NEWLOCATION;
	}

	return iMsgID;
}

BOOL PatternMoveAction::DrawBeforeFinish()
{
	if (m_preSelectState != PRE_SELECT_INACTIVE)
	{
		// now PatternPreSelectAction will care the draw.

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

		if (m_actionState == TRANSFORM_PATTERN_BASE_POINT)
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

END_CUTLEADER_NAMESPACE()
