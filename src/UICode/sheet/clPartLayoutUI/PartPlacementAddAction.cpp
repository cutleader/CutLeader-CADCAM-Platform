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

	// ����������ö��󣬲����������ĵ��������ǰ���λ�á�
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

	// ���ƶ���������á������ĵ㡣
	m_movCenterPt = m_pMovePartPlacements->GetRectBox().GetCenterPt();

	// �Ƿ�ҪΪ���ܿ���׼�����ݡ�
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
	// �ѡ�������á��ŵ�����ϡ�
	ICommandPtr pCommand(new PartPlacementAddCommand(GetEditorData(), m_pPartPlacementList, m_pMovePartPlacements->at(0), m_pPartInstanceList));
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	/************************************************************************/	
	// ׼����һ�β��������ݡ�

	// ��һ��Ҫ�ŵ�������á�
	PartPlacementListPtr pMovePartPlacements(new PartPlacementList);
	PartPlacementPtr pPartPlacement = PartPlacementManager::ClonePartPmt(m_pMovePartPlacements->at(0).get());
	pMovePartPlacements->push_back(pPartPlacement);
	m_pMovePartPlacements = pMovePartPlacements;

	// ���ƶ���������á������ĵ㡣
	m_movCenterPt = m_pMovePartPlacements->GetRectBox().GetCenterPt();

	// �Ƿ�ҪΪ���ܿ���׼�����ݡ�
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

	// �ƶ��Ĺ����и���һЩ���ݡ�
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
	// �����ƶ��еġ�������á���
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
	// ����Ϊ���ܿ���׼�����ݡ�
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	if (pSheetEditParam->EnableDisDetect())
	{
		// ������������
		m_pRemnantRegionList = PartPlacementManager::CalcRemnantRegion(m_pMatSize, m_pPartInstanceList, m_dMovPartWidth, m_dMovPartHeight);

		// reset����롰�����ڿס��ġ��⿿���켣��
		m_outerStickPolys.clear();

		// ȡ����ǰ����״̬��
		m_pStickRemRegion.reset();
	}
}

END_CUTLEADER_NAMESPACE()
