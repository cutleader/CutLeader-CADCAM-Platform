#include "StdAfx.h"
#include "PartPlacementCopyAction.h"

#include "baseConst.h"
#include "Point2D.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "ICommand.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"
#include "clUILibResource.h"

#include "SnapPtDrawer.h"
#include "PatternDrawer.h"
#include "LoopStartCutDataList.h"
#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopStartCutProcessor.h"

#include "PartCamData.h"
#include "PartItem.h"
#include "Part.h"

#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartPlacementDrawer.h"
#include "PartPlacementCopyCommand.h"
#include "PartPlacementSelectAction.h"
#include "clPartLayoutResource.h"
#include "PartPlacementManager.h"
#include "PartLayoutHelper.h"
#include "PartGroup.h"
#include "PartGroupList.h"

#include "DisplayParam.h"
#include "SheetEditParam.h"
#include "CLOptionInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementCopyAction::PartPlacementCopyAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
									 PartPlacementListPtr pSelectedPartPlacements, GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg)
    : PartPlacementPreSelectAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pDisDetectDlg, pViewPort)
{
	m_pSelectedPartPlacements = pSelectedPartPlacements;

	if (pSelectedPartPlacements->size() > 0)
	{
		// ��¡һ�ݡ�������á������ƶ���
		PartPlacementListPtr pMovePartPlacements(new PartPlacementList);
		for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
			pMovePartPlacements->push_back(PartPlacementManager::ClonePartPmt(m_pSelectedPartPlacements->at(i).get()));
		m_pMovePartPlacements = pMovePartPlacements;

		// ���ƶ���������á������ĵ㡣
		m_movCenterPt = m_pSelectedPartPlacements->GetRectBox().GetCenterPt();

		// �Ƿ�ҪΪ���ܿ���׼�����ݡ�
		SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
		if (pSheetEditParam->EnableDisDetect())
		{
			m_pMaterialSize = pMaterialSize;
			m_pFixPartInstances = m_pPartInstanceList;
			m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
			UpdateBeforeMov();
		}

		m_iMsgID = IDS_OUTPUT_COPYPARTPMT_NEWLOCATION;
	}
	else
		m_iMsgID = IDS_OUTPUT_SELECT_PRTPMT_COPY;
}

PartPlacementCopyAction::~PartPlacementCopyAction(void)
{
}

BOOL PartPlacementCopyAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::LButtonDown(dPtX, dPtY);
	}
	else
	{
        // ��¡������ú�Ҳ��¡��ص�����飬����ֻ���¡��������顣
        PartGroupListPtr pPartGroupList_new = PartPlacementManager::ClonePartGroups_whenClonePartPlacements_2(m_pPartGroupList.get(),
            m_pSelectedPartPlacements.get(), m_pMovePartPlacements.get());

        // ����Ҫ�½����������
        PartGroupListPtr pPartGroups_willAdded(new PartGroupList);
        for (unsigned int i = 0; i < pPartGroupList_new->size(); i++)
        {
            PartGroupPtr pPartGroup = pPartGroupList_new->operator [](i);
            pPartGroups_willAdded->push_back(pPartGroup);
            PartGroupListPtr pPartGroups_tmp = pPartGroup->GetSubPartGroupsRecursively();
            pPartGroups_willAdded->insert(pPartGroups_willAdded->end(), pPartGroups_tmp->begin(), pPartGroups_tmp->end());
        }

		// ����������á�
		ICommandPtr pCommand(new PartPlacementCopyCommand(GetEditorData(), m_pPartPlacementList, m_pPartInstanceList, m_pPartGroupList, m_pMovePartPlacements, pPartGroups_willAdded));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// ѡ�п������������á�
        SendMessage2EnterPartPlacementSelectAction(m_pMovePartPlacements.get());
	}

	return TRUE;
}

BOOL PartPlacementCopyAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::LButtonUp(dPtX, dPtY);

		if (m_pSelectedPartPlacements->size() > 0)
		{
			// ��¡һ�ݡ�������á������ƶ���
			PartPlacementListPtr pMovePartPlacements(new PartPlacementList);
			for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
				pMovePartPlacements->push_back(PartPlacementManager::ClonePartPmt(m_pSelectedPartPlacements->at(i).get()));
			m_pMovePartPlacements = pMovePartPlacements;

			// ���ƶ���������á������ĵ㡣
			m_movCenterPt = m_pSelectedPartPlacements->GetRectBox().GetCenterPt();

			// �Ƿ�ҪΪ���ܿ���׼�����ݡ�
			SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
			if (pSheetEditParam->EnableDisDetect())
			{
				m_pMaterialSize = m_pMatSize;
				m_pFixPartInstances = m_pPartInstanceList;
				m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
				UpdateBeforeMov();
			}

			m_iMsgID = IDS_OUTPUT_COPYPARTPMT_NEWLOCATION;
		}
	}

	return TRUE;
}

BOOL PartPlacementCopyAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D currentPt(dPtX, dPtY);

	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::MovePoint(dPtX, dPtY);
	}
	else
	{
		// �ƶ��Ĺ����и���һЩ���ݡ�
		UpdateWhenMov(currentPt);
	}

	return TRUE;
}

BOOL PartPlacementCopyAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

BOOL PartPlacementCopyAction::DrawBeforeFinish()
{
	DisplayParamPtr pColorInfo = CLOptionInfo::GetInstance()->GetDisplayParam();

	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::DrawBeforeFinish();
	}
	else
	{
		// ����ѡ�еġ�������á���
		PartPlacementDrawer partPlacementDrawer(m_pViewPort);
		partPlacementDrawer.DrawSelectedPartPlacements(m_pSelectedPartPlacements.get());

		if (m_pMovePartPlacements->size() > 0)
		{
			// �����ƶ��еġ�������á���
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
		}

		// draw cursor.
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);
	}

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
