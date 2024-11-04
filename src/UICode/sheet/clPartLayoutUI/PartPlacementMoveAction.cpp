#include "StdAfx.h"
#include "PartPlacementMoveAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"
#include "clUILibResource.h"

#include "IPatternLoop.h"
#include "PatternLoopCache.h"

#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartInstance.h"
#include "PartInstanceList.h"
#include "LoopInstance.h"
#include "LoopInstanceCache.h"
#include "LoopInstanceList.h"
#include "LoopInstanceManager.h"
#include "PartPlacementDrawer.h"
#include "PartPlacementTransformCommand.h"
#include "PartPlacementSelectAction.h"
#include "clPartLayoutResource.h"
#include "PartPlacementManager.h"

#include "DisplayParam.h"
#include "SheetEditParam.h"
#include "CLOptionInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementMoveAction::PartPlacementMoveAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
									 PartPlacementListPtr pSelectedPartPlacements, GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg)
    : PartPlacementPreSelectAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pDisDetectDlg, pViewPort)
{
	m_pSelectedPartPlacements = pSelectedPartPlacements;

	if (m_pSelectedPartPlacements->size() > 0)
	{
		// 选中的“零件放置”的中心点。
		m_partCenterPt = m_pSelectedPartPlacements->GetRectBox().GetCenterPt();

		// “移动的零件放置”的中心点。
		m_movCenterPt = m_partCenterPt;

		// 克隆一份“零件放置”用于移动。
		m_pMovePartPlacements = boost::dynamic_pointer_cast<PartPlacementList>(m_pSelectedPartPlacements->Clone());

		// 为智能靠碰准备数据。
		SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
		if (pSheetEditParam->EnableDisDetect())
		{
			PartInstanceListPtr pFixPartInsts(new PartInstanceList);
			for (unsigned int i = 0; i < m_pPartPlacementList->size(); i++)
			{
				const PartPlacement* pPartPlacement = m_pPartPlacementList->operator [](i).get();
				PartInstanceListPtr pPartInstances = m_pPartInstanceList->GetPartInstanceByPartPlacement(pPartPlacement);
				if (!m_pSelectedPartPlacements->GetPartPlacementByID(pPartPlacement->GetID()))
                {
					pFixPartInsts->insert(pFixPartInsts->end(), pPartInstances->begin(), pPartInstances->end());
                }
			}
			m_pFixPartInstances = pFixPartInsts;

			m_pMaterialSize = pMaterialSize;
			m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
			UpdateBeforeMov();
		}

		m_iMsgID = IDS_OUTPUT_MOVEPARTPMT_NEWLOCATION;
	}
	else
	{
		m_iMsgID = IDS_OUTPUT_SELECT_PRTPMT_MOVE;
	}
}

PartPlacementMoveAction::~PartPlacementMoveAction(void)
{
}

BOOL PartPlacementMoveAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::LButtonDown(dPtX, dPtY);
	}
	else
	{
		// 移动零件放置。
		CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMAND_MOVE_PARTPMT);
		PartPlacementListPtr pSelPartPmts(new PartPlacementList);
		pSelPartPmts->insert(pSelPartPmts->end(), m_pSelectedPartPlacements->begin(), m_pSelectedPartPlacements->end());
		ICommandPtr pCommand(new PartPlacementTransformCommand(str, GetEditorData(), m_pPartPlacementList, pSelPartPmts, m_pMovePartPlacements));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// 选中移动后的零件放置。
        SendMessage2EnterPartPlacementSelectAction(m_pSelectedPartPlacements.get());
	}

	return TRUE;
}

BOOL PartPlacementMoveAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::LButtonUp(dPtX, dPtY);

		if (m_pSelectedPartPlacements->size() > 0)
		{
			// 选中的“零件放置”的中心点。
			m_partCenterPt = m_pSelectedPartPlacements->GetRectBox().GetCenterPt();

			// 克隆一份“零件放置”用于移动。
			m_pMovePartPlacements = boost::dynamic_pointer_cast<PartPlacementList>(m_pSelectedPartPlacements->Clone());

			// “移动的零件放置”的中心点。
			m_movCenterPt = m_partCenterPt;

			// 是否要为智能靠碰准备数据。
			SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
			if (pSheetEditParam->EnableDisDetect())
			{
				PartInstanceListPtr pFixPartInsts(new PartInstanceList);
				for (unsigned int i = 0; i < m_pPartPlacementList->size(); i++)
				{
					const PartPlacement* pPartPlacement = m_pPartPlacementList->operator [](i).get();
					PartInstanceListPtr pPartInstances = m_pPartInstanceList->GetPartInstanceByPartPlacement(pPartPlacement);
					if (!m_pSelectedPartPlacements->GetPartPlacementByID(pPartPlacement->GetID()))
                    {
						pFixPartInsts->insert(pFixPartInsts->end(), pPartInstances->begin(), pPartInstances->end());
                    }
				}
				m_pFixPartInstances = pFixPartInsts;

				m_pMaterialSize = m_pMatSize;
				m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
				UpdateBeforeMov();
			}

			m_iMsgID = IDS_OUTPUT_MOVEPARTPMT_NEWLOCATION;
		}
	}

	return TRUE;
}

BOOL PartPlacementMoveAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D currentPt(dPtX, dPtY);

	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::MovePoint(dPtX, dPtY);
	}
	else
	{
		// 移动的过程中更新一些数据。
		UpdateWhenMov(currentPt);
	}

	return TRUE;
}

BOOL PartPlacementMoveAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pSelectedPartPlacements->size() > 0)
	{
		if (ValueInputManager::GetInstance()->IsInputAdd(nChar))
		{
			UpdateWhenRotate(FALSE);
		}
		else if (ValueInputManager::GetInstance()->IsInputDes(nChar))
		{
			UpdateWhenRotate(TRUE);
		}
	}

	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL PartPlacementMoveAction::DrawBeforeFinish()
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::DrawBeforeFinish();
	}
	else
	{
		DisplayParamPtr pColorInfo = CLOptionInfo::GetInstance()->GetDisplayParam();

		// 绘制选中的“零件放置”。
		PartPlacementDrawer partPlacementDrawer(m_pViewPort);
		partPlacementDrawer.DrawSelectedPartPlacements(m_pSelectedPartPlacements.get());

		if (m_pMovePartPlacements->size() > 0)
		{
			COLORREF dwOldColor;
			m_pViewPort->GetDrawColor(dwOldColor);

			// 绘制牵引线。
			m_pViewPort->SetDrawColor(*pColorInfo->GetDrawAssistColor());
			m_pViewPort->DrawLinePoint2D(m_partCenterPt.X(), m_partCenterPt.Y(), m_movCenterPt.X(), m_movCenterPt.Y());

			// 绘制移动中的“零件放置”。
			if (m_pStickRemRegion)
			{
				for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
                {
					partPlacementDrawer.DrawActivePartPlacement(m_pMovePartPlacements->at(i).get());
                }
			}
			else
            {
				partPlacementDrawer.DrawPartPlacements(m_pMovePartPlacements.get());
            }

			m_pViewPort->SetDrawColor(dwOldColor);
		}

		// draw cursor.
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);
	}

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
