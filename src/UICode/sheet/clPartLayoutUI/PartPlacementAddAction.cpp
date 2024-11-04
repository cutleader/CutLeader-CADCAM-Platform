#include "StdAfx.h"
#include "PartPlacementAddAction.h"

#include "CommandManager.h"
#include "ICommand.h"
#include "ValueInputManager.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "ProductFolderManager.h"
#include "DataBaseManager.h"

#include "PartCadData.h"
#include "PartItem.h"
#include "Part.h"
#include "PartLoader.h"
#include "PartManager.h"
#include "PartBuilder.h"

#include "clPartLayoutResource.h"
#include "PartLayoutEditor.h"
#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartPlacementAddCommand.h"
#include "PartPlacementManager.h"
#include "PartPlacementDrawer.h"

#include "DisplayParam.h"
#include "SheetEditParam.h"
#include "CLOptionInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementAddAction::PartPlacementAddAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
								   LONGLONG iPartID, LONGLONG iShtParamConfigID, DistanceDetectDlg* pDisDetectDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
    m_pDisDetectDlg = pDisDetectDlg;
	m_iMsgID = IDS_OUTPUT_PARTPMT_BASEPT;

	// 生成零件放置对象，并把它的中心点调整到当前光标位置。
	PartPlacementListPtr pMovePartPlacements(new PartPlacementList);
	PartPlacementPtr pNewPartPlacement;
	LONGLONG iCamDataID, iExpLibItemID;
	DBConnectPtr pPartDBCon = DataBaseManager::ConnectPartDB(iPartID);
	if (PartManager::HasCamDatasetA(iPartID, iShtParamConfigID, iCamDataID, iExpLibItemID))
	{
		PartItemPtr pPartItem = PartLoader::LoadPartItem(pPartDBCon, iPartID, iCamDataID, true);
		pNewPartPlacement = PartPlacementManager::BuildPartPmt(pPartItem.get(), Matrix2D(), true);
	}
	else
	{
		PartCadDataPtr pPartCadData = PartLoader::LoadPartCadData(pPartDBCon);
		PartCamDataPtr pNewPartCamData = PartBuilder::BuildPartCamData(pPartCadData, iShtParamConfigID);
		PartPtr pPart = PartLoader::LoadPart(pPartDBCon, iPartID, PARTLOAD_BASIC);
		PartItemPtr pPartItem(new PartItem(iPartID, pPart->GetName(), pPartCadData, pNewPartCamData));
		pNewPartPlacement.reset(new PartPlacement(pPartItem));
	}
	pMovePartPlacements->push_back(pNewPartPlacement);
	m_pMovePartPlacements = pMovePartPlacements;

	// “移动的零件放置”的中心点。
	m_movCenterPt = m_pMovePartPlacements->GetRectBox().GetCenterPt();

	// 是否要为智能靠碰准备数据。
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	if (pSheetEditParam->EnableDisDetect())
	{
		m_pMaterialSize = pMaterialSize;
		m_pFixPartInstances = m_pPartInstanceList;
		m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
		UpdateBeforeMov();
	}
}

PartPlacementAddAction::~PartPlacementAddAction(void)
{
}

BOOL PartPlacementAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	// 把“零件放置”放到板材上。
	ICommandPtr pCommand(new PartPlacementAddCommand(GetEditorData(), m_pPartPlacementList, m_pMovePartPlacements->at(0), m_pPartInstanceList));
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	/************************************************************************/	
	// 准备下一次操作的数据。

	// 这一次要放的零件放置。
	PartPlacementListPtr pMovePartPlacements(new PartPlacementList);
	PartPlacementPtr pPartPlacement = PartPlacementManager::ClonePartPmt(m_pMovePartPlacements->at(0).get());
	pMovePartPlacements->push_back(pPartPlacement);
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
	/************************************************************************/

	return TRUE;
}

BOOL PartPlacementAddAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D currentPt(dPtX, dPtY);

	// 移动的过程中更新一些数据。
	UpdateWhenMov(currentPt);

	return TRUE;
}

BOOL PartPlacementAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

BOOL PartPlacementAddAction::DrawBeforeFinish()
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

void PartPlacementAddAction::UpdateForUndoRedo()
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
