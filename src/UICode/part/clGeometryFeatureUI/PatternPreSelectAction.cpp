#include "StdAfx.h"
#include "PatternPreSelectAction.h"

#include "MathEx.h"
#include "GlViewPort.h"
#include "Rect2D.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "clUILibResource.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "PatternDrawer.h"
#include "PatternList.h"
#include "clGeometryFeatureResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternPreSelectAction::PatternPreSelectAction()
{
	m_pActivePat.reset();
	m_pSelectedPats.reset(new PatternList);

	m_preSelectState = PRE_SELECT_ACTIVE;

	m_bDisplayValInput = FALSE;
	m_iMsgID = IDS_OUTPUT_SELECTPATTERN;
}

PatternPreSelectAction::~PatternPreSelectAction(void)
{
}

BOOL PatternPreSelectAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_ACTIVE || m_preSelectState == PRE_SELECT_SECOND_INPUT)
	{
		m_preSelectState = PRE_SELECT_FIRST_INPUT;
		m_iMsgID = IDS_OUTPUT_SELECTPAT_NEXTPT;

		m_data[0] = dPtX;
		m_data[1] = dPtY;
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL PatternPreSelectAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_FIRST_INPUT)
	{
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

			m_preSelectState = PRE_SELECT_SECOND_INPUT;
		}
		else
		{
			// reset the status.
			m_preSelectState = PRE_SELECT_ACTIVE;
		}

		// 确定提示信息。
		if (m_pSelectedPats->size() == 0)
			m_iMsgID = IDS_OUTPUT_NOPAT_SELECT;
		else
			m_iMsgID = IDS_OUTPUT_PAT_SELECT;
	}

	return TRUE;
}

BOOL PatternPreSelectAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// get the nearest pattern.
	m_pActivePat = m_pPatList->GetPatternClosestTo(pt, true);

	// if left button down, we should draw a rectangle in stipple mode.
	if (m_preSelectState == PRE_SELECT_FIRST_INPUT)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL PatternPreSelectAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PatternEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	// 选中全部图形。
	if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
		m_pSelectedPats->insert(m_pPatList->end(), m_pPatList->begin(), m_pPatList->end());

	return TRUE;
}

BOOL PatternPreSelectAction::DrawBeforeFinish()
{
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();
	PatternDrawer patDrawer(m_pViewPort);

	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	// 绘制选择框。
	if (m_preSelectState == PRE_SELECT_FIRST_INPUT)
	{
		// set the stipple mode.
		bool oldStipple = false;
		int oldRepeat = 1;
		unsigned short oldPattern = 0xffff;
		m_pViewPort->GetLineStipple(oldStipple, oldRepeat, oldPattern);
		m_pViewPort->SetLineStipple(true, 2, 0xcccc);

		// draw the selection rectangle.
		m_pViewPort->DrawRect(m_data[0], m_data[1], m_data[2], m_data[3]);

		// restore the drawer
		m_pViewPort->SetLineStipple(oldStipple, oldRepeat, oldPattern);
	}

	// 绘制当前图形。
	if (m_pActivePat && m_pSelectedPats->GetPatternIndex(m_pActivePat.get()) == INVALID_IDX)
    {
		patDrawer.DrawActivePat(m_pActivePat);
    }

	// 绘制选中的图形。
	patDrawer.DrawSelPatList(m_pSelectedPats);

	// draw cursor.
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());
	CursorDrawer::DrawRectSelCursor(m_pViewPort);

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

PatternListPtr PatternPreSelectAction::CalcSelectPat()
{
	PatternListPtr pSelPatList(new PatternList);

	// if left button down and up are at same position. only one pattern will be selected.
	if (MathEx::Equal(m_data[0], m_data[2]) && MathEx::Equal(m_data[1], m_data[3]))
	{
		Point2D pt(m_data[0], m_data[1]);
		IPatternPtr pPattern = m_pPatList->GetPatternClosestTo(pt);
		if (pPattern)
			pSelPatList->push_back(pPattern);
	}
	else
	{
		// select pattern with the rectangle range.
		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);

		pSelPatList = m_pPatList->GetPatternInRect(rect);
	}

	return pSelPatList;
}

END_CUTLEADER_NAMESPACE()
