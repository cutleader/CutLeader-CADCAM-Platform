#include "StdAfx.h"
#include "PatternRotateAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "clUILibResource.h"
#include "CursorDrawer.h"

#include "PatternList.h"
#include "PatternDrawer.h"
#include "PatternTransformCommand.h"
#include "clGeometryFeatureResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternRotateAction::PatternRotateAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList,
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

PatternRotateAction::~PatternRotateAction(void)
{
}

CString PatternRotateAction::GetActionName()
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMAND_ROTATE_PAT);

	return str;
}

void PatternRotateAction::SetMatrixValue()
{
	m_modifyPatMat.Reset();

	Point2D basePoint(m_dBasePtX, m_dBasePtY);
	double angle = atan2((m_dSecondPtY - m_dBasePtY), (m_dSecondPtX - m_dBasePtX)) - 
				   atan2((m_dFirstPtY - m_dBasePtY), (m_dFirstPtX - m_dBasePtX));

	m_modifyPatMat.RotateBy(basePoint, angle);
}

void PatternRotateAction::ConvertKBInput(double& dVal1, double& dVal2)
{
	if (m_actionState == TRANSFORM_PATTERN_NOTSTART ||
		m_actionState == TRANSFORM_PATTERN_SECOND_POINT)
	{
		// here, we do not need to convert.
		;
	}
	else if (m_actionState == TRANSFORM_PATTERN_BASE_POINT)
	{
		// "dVal1" holds the rotate angle.

		Point2D basePoint(m_dBasePtX, m_dBasePtY);

		Vector2D v1;
		v1.SetAngle(dVal1*D2R);

		basePoint.Transfer(v1);

		dVal1 = basePoint.X();
		dVal2 = basePoint.Y();
	}
}

int PatternRotateAction::GetMsgID()
{
	int iMsgID = INVALID_ID;

	if (m_actionState == TRANSFORM_PATTERN_NOTSTART ||
		m_actionState == TRANSFORM_PATTERN_SECOND_POINT)
	{
		iMsgID = IDS_OUTPUT_ROTATEPAT_BASEPT;
	}
	else if (m_actionState == TRANSFORM_PATTERN_BASE_POINT)
	{
		iMsgID = IDS_OUTPUT_ROTATEPAT_ANGLE;
	}

	return iMsgID;
}

BOOL PatternRotateAction::DrawBeforeFinish()
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
