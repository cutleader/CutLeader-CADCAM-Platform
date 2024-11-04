#include "StdAfx.h"
#include "PartPlacementAddAction2.h"

#include "DateHelper.h"
#include "CommandManager.h"
#include "ICommand.h"
#include "ValueInputManager.h"
#include "GlViewPort.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "ProductFolderManager.h"
#include "DataBaseManager.h"

#include "LoopStartCutProcessor.h"
#include "LoopCutFeatureList.h"
#include "LoopCutFeature.h"
#include "PartCadData.h"
#include "PartCamData.h"
#include "Part.h"
#include "PartItem.h"
#include "PartItemList.h"
#include "PartLoader.h"
#include "PartManager.h"

#include "clPartLayoutResource.h"
#include "PartLayoutEditor.h"
#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartPlacementAddCommand.h"
#include "PartPlacementManager.h"
#include "PartPlacementDrawer.h"
#include "PartPlacementSelectAction.h"

#include "DisplayParam.h"
#include "SheetEditParam.h"
#include "CLOptionInfo.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementAddAction2::PartPlacementAddAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
									   PartItemListPtr pPartItemList, const std::map<LONGLONG, std::vector<Matrix2D>>& matrixData_of_sameShapePart, FolderNodePtr pSaveFolder, DistanceDetectDlg* pDisDetectDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_pPartItemList = pPartItemList;
	m_bPartItemSaved = FALSE;
	m_pSaveFolder = pSaveFolder;
	m_iMsgID = IDS_OUTPUT_PARTPMT_BASEPT;
    m_pDisDetectDlg = pDisDetectDlg;

	// 逐个处理零件项。
	PartPlacementListPtr pMovePartPlacements(new PartPlacementList);
	for (unsigned int i = 0; i < m_pPartItemList->size(); i++)
	{
		PartItemPtr pPartItem = m_pPartItemList->at(i);
        PartPlacementPtr pPartPlacement = PartPlacementManager::BuildPartPmt(pPartItem.get(), Matrix2D(), true);
		pMovePartPlacements->push_back(pPartPlacement);

        // 看看这个零件有没有“相同零件”
		map<LONGLONG, vector<Matrix2D>>::const_iterator iter = matrixData_of_sameShapePart.find(pPartItem->GetPartID());
		if (iter != matrixData_of_sameShapePart.end())
		{
			vector<Matrix2D> matList = iter->second;
			for (unsigned int j = 0; j < matList.size(); j++)
			{
				Matrix2D mat = matList.at(j);
				PartPlacementPtr pPartPmt_samePart = PartPlacementManager::BuildPartPmt(pPartItem.get(), mat, true);
				pMovePartPlacements->push_back(pPartPmt_samePart);
			}
		}
	}
	m_pMovePartPlacements = pMovePartPlacements;

	// “移动的零件放置”的中心点。
	m_movCenterPt = m_pMovePartPlacements->GetRectBox().GetCenterPt();

	// 是否要为智能靠碰准备数据。
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	if (pSheetEditParam->EnableDisDetect())
	{
		m_pMaterialSize = m_pMatSize;
		m_pFixPartInstances = m_pPartInstanceList;
		m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
		UpdateBeforeMov();
	}
}

PartPlacementAddAction2::~PartPlacementAddAction2(void)
{
}

BOOL PartPlacementAddAction2::LButtonDown(double& dPtX, double& dPtY)
{
	// 保存零件项到数据库。
	if (!m_bPartItemSaved)
	{
		for (unsigned int i = 0; i < m_pPartItemList->size(); i++)
		{
			PartItemPtr pPartItem = m_pPartItemList->at(i);
			PartPtr pPart(new Part(pPartItem->GetPartID(), pPartItem->GetPartName(), pPartItem->GetPartComment(), pPartItem->GetCadData(), pPartItem->GetCamData()));
			pPart->SetCreateTime(DateHelper::GetCurrentTime());
			pPart->SetModifyTime(DateHelper::GetCurrentTime());
			PartManager::SaveNewPart(pPart.get(), m_pSaveFolder);
		}

		m_bPartItemSaved = TRUE;
	}

	// 把“零件放置”放到板材上。
	PartPlacementListPtr pPartPlacements_willBeSelected(new PartPlacementList);
	for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
	{
		PartPlacementPtr pPartPlacement = m_pMovePartPlacements->at(i);
		ICommandPtr pCommand(new PartPlacementAddCommand(GetEditorData(), m_pPartPlacementList, pPartPlacement, m_pPartInstanceList));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
        pPartPlacements_willBeSelected->push_back(pPartPlacement);
	}

	// 当添加一个零件时可以连续操作。
	if (m_pMovePartPlacements->size() > 1)
    {
        SendMessage2EnterPartPlacementSelectAction(pPartPlacements_willBeSelected.get());
    }
	else
	{
		/************************************************************************/	
		// 准备下一次操作的数据。

		// 这一次要放的零件放置。
		PartPlacementListPtr pPartPlacements(new PartPlacementList);
		for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
			pPartPlacements->push_back(PartPlacementManager::ClonePartPmt(m_pMovePartPlacements->at(i).get()));
		m_pMovePartPlacements = pPartPlacements;

		// “移动的零件放置”的中心点。
		m_movCenterPt = m_pMovePartPlacements->GetRectBox().GetCenterPt();

		// 是否要为智能靠碰准备数据。
		SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
		if (pSheetEditParam->EnableDisDetect())
		{
			m_pMaterialSize = m_pMatSize;
			m_pFixPartInstances = m_pPartInstanceList;
			m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
			UpdateBeforeMov();
		}
		/************************************************************************/
	}

	return TRUE;
}

BOOL PartPlacementAddAction2::MovePoint(double& dPtX, double& dPtY)
{
	Point2D currentPt(dPtX, dPtY);

	// 移动的过程中更新一些数据。
	UpdateWhenMov(currentPt);

	return TRUE;
}

BOOL PartPlacementAddAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (ValueInputManager::GetInstance()->IsInputAdd(nChar))
	{
		UpdateWhenRotate(FALSE);
	}
	else if (ValueInputManager::GetInstance()->IsInputDes(nChar))
	{
		UpdateWhenRotate(TRUE);
	}

	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL PartPlacementAddAction2::DrawBeforeFinish()
{
	// 绘制移动中的“零件放置”。
	PartPlacementDrawer partPlacementDrawer(m_pViewPort);
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

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void PartPlacementAddAction2::UpdateForUndoRedo()
{
	// 重新为智能靠碰准备数据。
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	if (pSheetEditParam->EnableDisDetect())
	{
		// 更新余料区域。
		m_pRemnantRegionList = PartPlacementManager::CalcRemnantRegion(m_pMatSize, m_pPartInstanceList, m_dMovPartWidth, m_dMovPartHeight);

		// reset零件与“区域内孔”的“外靠碰轨迹”
		m_outerStickPolys.clear();

		// 取消当前靠碰状态。
		m_pStickRemRegion.reset();
	}
}

END_CUTLEADER_NAMESPACE()
