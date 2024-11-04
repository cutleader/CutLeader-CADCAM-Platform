#include "StdAfx.h"
#include "PartPlacementPreSelectAction.h"

#include "Point2D.h"
#include "MathEx.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "PartPlacementList.h"
#include "PartPlacement.h"
#include "PartPlacementDrawer.h"
#include "clPartLayoutResource.h"
#include "PartLayoutHelper.h"

#include "DisplayParam.h"
#include "CLOptionInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementPreSelectAction::PartPlacementPreSelectAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, DistanceDetectDlg* pDisDetectDlg, GlViewPortPtr pViewPort)
    : PartPlacementEditAction(pMaterialSize, pPartPlacementList, pPartInstanceList, pPartGroups, pViewPort)
{
	m_pActivePartPlacements.reset(new PartPlacementList);
	m_preSelectState = PRE_SELECT_NO_INPUT;
    m_pSelectedPartPlacements.reset(new PartPlacementList);
    m_pDisDetectDlg = pDisDetectDlg;
}

PartPlacementPreSelectAction::~PartPlacementPreSelectAction(void)
{
}

BOOL PartPlacementPreSelectAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_NO_INPUT || m_preSelectState == PRE_SELECT_LEFT_BTN_UP)
	{
		m_preSelectState = PRE_SELECT_LEFT_BTN_DOWN;

		m_data[0] = dPtX;
		m_data[1] = dPtY;
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL PartPlacementPreSelectAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_preSelectState = PRE_SELECT_LEFT_BTN_UP;

		// 要是没有按住ctrl键，就需要清除现有选择集。
		if (!ValueInputManager::GetInstance()->IsInputCtrl())
        {
			m_pSelectedPartPlacements->clear();
        }

		if (m_pActivePartPlacements->size() > 0)
		{
			m_pSelectedPartPlacements->insert(m_pSelectedPartPlacements->end(), m_pActivePartPlacements->begin(), m_pActivePartPlacements->end());
		}
	}

	return TRUE;
}

BOOL PartPlacementPreSelectAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;

		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);
        std::map<LONGLONG, Rect2D> rects_of_partPmt;
		m_pActivePartPlacements = PartLayoutHelper::GetPartPlacements_byRectSelection(rect, m_pPartPlacementList.get(), m_pPartGroupList.get(), rects_of_partPmt);
	}
	else
	{
		Point2D pt(dPtX, dPtY);
        std::map<LONGLONG, Rect2D> rects_of_partPmt;
		m_pActivePartPlacements = PartLayoutHelper::GetPartPlacements_byClickSelection(pt, m_pPartPlacementList.get(), m_pPartGroupList.get(), rects_of_partPmt);
	}

	return TRUE;
}

BOOL PartPlacementPreSelectAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		m_pSelectedPartPlacements->clear();
		m_pSelectedPartPlacements->insert(m_pSelectedPartPlacements->end(), m_pPartPlacementList->begin(), m_pPartPlacementList->end());
	}

	return PartPlacementEditAction::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL PartPlacementPreSelectAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();
	PartPlacementDrawer partPlacementDrawer(m_pViewPort);

	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	// draw the selected part placement.
	partPlacementDrawer.DrawSelectedPartPlacements(m_pSelectedPartPlacements.get());

	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN) // 框选
	{
		// 绘制框框
		{
			// backup the old draw config.
			bool bOldStipple = false;
			int iOldRepeat = 1;
			unsigned short iOldPattern = 0xffff;
			m_pViewPort->GetLineStipple(bOldStipple, iOldRepeat, iOldPattern);

			// draw the selection rectangle.
			m_pViewPort->SetLineStipple(true, 2, 0xcccc);
			m_pViewPort->DrawRect(m_data[0], m_data[1], m_data[2], m_data[3]);

			// restore the drawer
			m_pViewPort->SetLineStipple(bOldStipple, iOldRepeat, iOldPattern);
		}
	}

    // 高亮零件放置
    for (unsigned int i = 0; i < m_pActivePartPlacements->size(); i++)
    {
        partPlacementDrawer.DrawActivePartPlacement(m_pActivePartPlacements->at(i).get());
    }

	// draw cursor.
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
