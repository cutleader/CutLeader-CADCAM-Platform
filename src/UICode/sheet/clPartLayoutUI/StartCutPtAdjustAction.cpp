#include "StdAfx.h"
#include "StartCutPtAdjustAction.h"

#include "MathEx.h"
#include "LineArc2DList.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "PatternList.h"
#include "PatternLoopNode.h"
#include "PatternLoopNodeList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "IArrayPattern.h"
#include "LoopDrawer.h"
#include "LoopProcessor.h"

#include "ILead.h"
#include "GeometryStartCutPt.h"
#include "CornerStartCutPt.h"
#include "LoopStartCutCache.h"
#include "LoopStartCutData.h"
#include "LoopStartCutDataList.h"
#include "clCutFeatureResource.h"
#include "LoopCutFeatureDrawer.h"
#include "LeadManager.h"
#include "StartCutPtModifyCommand.h"
#include "StartCutPtModifyCommandA.h"
#include "StartCutPt2GeometryCommand.h"
#include "StartCutPt2CornerCommand.h"
#include "LoopStartCutProcessor.h"
#include "LoopToolManager.h"

#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "PartInstanceList.h"
#include "LoopInstanceManager.h"
#include "LoopInstanceList.h"

#include "ParamConfig.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "LeadConfigItem.h"

BEGIN_CUTLEADER_NAMESPACE()

StartCutPtAdjustAction::StartCutPtAdjustAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
										 GlViewPortPtr pViewPort, LeadConfigItemPtr pLeadConfigItem, SpreadSamePartDlg* pSpreadSamePartDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_pLeadConfigItem = pLeadConfigItem;
	m_pSpreadSamePartDlg = pSpreadSamePartDlg;

	m_state = START_PT_ADJUST_NONE;
	m_iMsgID = IDS_STARTPT_ADJUST_SEL_LOOP;
	m_pAllLoopInstances = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
	m_iPatIndex = INVALID_IDX;
}

StartCutPtAdjustAction::~StartCutPtAdjustAction(void)
{
}

BOOL StartCutPtAdjustAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActiveLoopInstance)
	{
		if (m_state == START_PT_ADJUST_NONE)
		{
			// 检查是不是开放回路。
			IPatternLoopPtr pPatternLoop = m_pActiveLoopInstance->GetCacheData()->GetPatternLoop();
			if (pPatternLoop->IsOpenPath())
			{
				CString str, strProductName;
				strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STARTPT_ADJUST_SEL_OPENLOOP);
				MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
				return FALSE;
			}
			else
            {
				m_pSelLoopInst = m_pActiveLoopInstance;
                UpdateNewPosition(Point2D(dPtX, dPtY)); // 新的起切点
            }

			m_iMsgID = IDS_STARTPT_ADJUST_SEL_PT;
			m_state = START_PT_ADJUST_SELECTLOOP;
		}
		else if (m_state == START_PT_ADJUST_SELECTLOOP)
		{
			LoopStartCutDataPtr pLoopStartCutData = m_pSelLoopInst->GetCacheData()->GetLoopStartCutData();
			IPatternLoopPtr pPatternLoop = pLoopStartCutData->GetPatternLoop();
			ILoopStartCutPtPtr pLoopStartCutPt = pLoopStartCutData->GetLoopStartCutPt();

			// 把起切点坐标变换到零件坐标系下。
			Point2D tmpPt = m_newPt;
			Matrix2D mat = m_pSelLoopInst->GetTransformMatrix();
			mat.Inverse();
			tmpPt *= mat;

			// 移动后的起切点位置。
			if (m_iPatIndex == INVALID_IDX)
				return FALSE;
			IPatternPtr pPattern = pPatternLoop->GetPatternByLoopNodeIndex(m_iPatIndex);
			PatternPosition secondPatPos;
			PatternPosition patternPosition = pPattern->GetPatPosByPoint(tmpPt, secondPatPos);

			if (tmpPt != pLoopStartCutData->GetCacheData()->GetStartCutPt()) // 是否做了移动。
			{
				if (LoopProcessor::PtAtLoopCorner(pLoopStartCutData->GetPatternLoop().get(), tmpPt)) // 起切点移到了角上。
				{
					double dCornerAngle = LoopProcessor::GetAngAtLoopCorner(pLoopStartCutData->GetPatternLoop().get(), tmpPt);
					if (!MathEx::Equal(dCornerAngle, PI, GEOM_TOLERANCE))
					{
						// get PatternPosition data of the adjacent geom.
						LONGLONG iNextPatID;
						PatternPosition nextPatPos;
						LoopProcessor::GetNextCornerGeomPos(pPatternLoop.get(), tmpPt, pPattern->GetID(), patternPosition, iNextPatID, nextPatPos);

						// create new start/end cut point info.
						CornerStartCutPtPtr pNewSECutPt(new CornerStartCutPt(pPattern->GetID(), patternPosition, iNextPatID, nextPatPos, pLoopStartCutData->SameDirWithLoop()));
                        pNewSECutPt->SetOverCut(m_pLeadConfigItem->GetOverCutAtCorner());

                        PatternLoopCutOffsetType iLoopCutOffset = LoopStartCutProcessor::CalculateLoopCutOffset(pLoopStartCutData.get());

						// 角上的引入线。
						double dLeadInLen = pLoopStartCutData->GetCacheData()->GetLeadInGeom()->GetTotalLength();
						ILeadPtr pLeadIn = LeadManager::BuildCornerLead_byLeadLength(pPatternLoop.get(), pLoopStartCutData->SameDirWithLoop(), iLoopCutOffset, LeadInOut_In, tmpPt, dLeadInLen);

						// 角上的引出线。
						double dLeadOutLen = pLoopStartCutData->GetCacheData()->GetLeadOutGeom()->GetTotalLength();
						ILeadPtr pLeadOut = LeadManager::BuildCornerLead_byLeadLength(pPatternLoop.get(), pLoopStartCutData->SameDirWithLoop(), iLoopCutOffset, LeadInOut_Out, tmpPt, dLeadOutLen);

						// 把起切点移到角上。
						StartPt2Corner(pLoopStartCutData, pNewSECutPt, pLeadIn, pLeadOut);
					}
					else
					{
						// 这个位置不能放起切点。
						return FALSE;
					}
				}
				else
				{
					// 把起切点移到图形上。
					StartPt2Geom(pLoopStartCutData, pPattern->GetID(), patternPosition);
				}
			}

			// 重设变量。
			m_state = START_PT_ADJUST_NONE;
			m_iMsgID = IDS_STARTPT_ADJUST_SEL_LOOP;
			m_pSelLoopInst.reset();
		}
	}

	return TRUE;
}

void StartCutPtAdjustAction::EditStartCutPoint_OfSelectedLoopInstance(LoopInstancePtr pLoopInstance)
{
    // 检查是不是开放回路。
    if (pLoopInstance->GetCacheData()->GetPatternLoop()->IsOpenPath())
    {
        CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
        CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STARTPT_ADJUST_SEL_LOOP);
        MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
        return;
    }

    m_pActiveLoopInstance = pLoopInstance;
    m_pSelLoopInst = pLoopInstance;
    m_iMsgID = IDS_STARTPT_ADJUST_SEL_PT;
    m_state = START_PT_ADJUST_SELECTLOOP;
}

BOOL StartCutPtAdjustAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_state == START_PT_ADJUST_NONE)
	{
		// 找到距离光标最近的回路。
		m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pAllLoopInstances, pt);
	} 
	else if (m_state == START_PT_ADJUST_SELECTLOOP)
	{
		// 新的起切点
        UpdateNewPosition(Point2D(dPtX, dPtY));
	}

	return TRUE;
}

BOOL StartCutPtAdjustAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL StartCutPtAdjustAction::DrawBeforeFinish()
{
	if (m_state == START_PT_ADJUST_NONE)
	{
		// draw the active loop in highlight mode.
		if (m_pActiveLoopInstance)
		{
			LoopDrawer loopDrawer(m_pViewPort);
            COLORREF iColor = LoopToolManager::GetLoopDisplayColor(m_pActiveLoopInstance->GetCacheData()->GetLoopTool().get());
			loopDrawer.DrawActiveLoop(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), iColor, m_pActiveLoopInstance->GetTransformMatrix());
		}
	} 
	else if (m_state == START_PT_ADJUST_SELECTLOOP)
	{
		// 绘制环。
		LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);
		loopFeatureDrawer.DrawPosRing(m_newPt);
	}

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void StartCutPtAdjustAction::StartPt2Corner(LoopStartCutDataPtr pLoopStartCutData, ILoopStartCutPtPtr pLoopStartCutPt, ILeadPtr pLeadIn, ILeadPtr pLeadOut)
{
	if (m_pSpreadSamePartDlg->IsSpread())
	{
		LoopStartCutDataListPtr pLoopStaEndCuts(new LoopStartCutDataList);
		for (unsigned int i = 0; i < m_pAllLoopInstances->size(); i++)
		{
			LoopInstancePtr pLoopInstance =  m_pAllLoopInstances->at(i);
			if (pLoopInstance->GetCacheData()->GetPatternLoop()->GetID() == pLoopStartCutData->GetPatternLoop()->GetID())
			{
				pLoopStaEndCuts->push_back(pLoopInstance->GetCacheData()->GetLoopStartCutData());
			}
		}

		// 执行命令。
		ICommandPtr pCommand(new StartCutPt2CornerCommand(GetEditorData(), pLoopStaEndCuts, pLoopStartCutPt, pLeadIn, pLeadOut));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}
	else
	{
		// 执行命令。
		ICommandPtr pCommand(new StartCutPtModifyCommandA(GetEditorData(), pLoopStartCutData, pLoopStartCutPt, pLeadIn, pLeadOut));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}
}

void StartCutPtAdjustAction::StartPt2Geom(LoopStartCutDataPtr pLoopStartCutData, LONGLONG iNewPatID, const PatternPosition& newPatPos)
{
	// 如果有起切点从角移到图形上，需要这些变量。
	ILoopStartCutPtPtr pNewSECutPt(new GeometryStartCutPt(iNewPatID, newPatPos, pLoopStartCutData->SameDirWithLoop()));
	IPatternLoopPtr pPatternLoop = pLoopStartCutData->GetPatternLoop();
	ILeadPtr pNewLeadIn = LeadManager::BuildGeomLead(pPatternLoop.get(), LeadInOut_In, m_pLeadConfigItem.get());
	ILeadPtr pNewLeadOut = LeadManager::BuildGeomLead(pPatternLoop.get(), LeadInOut_Out, m_pLeadConfigItem.get());

	if (!m_pSpreadSamePartDlg->IsSpread())
	{
		ILoopStartCutPtPtr pLoopStartCutPt = pLoopStartCutData->GetLoopStartCutPt();
		if (pLoopStartCutPt->GetSECutPtType() == SECUT_PT_CORNER) // 起切点从角移到图形上。
		{
			// 执行命令。
			ICommandPtr pCommand(new StartCutPtModifyCommandA(GetEditorData(), pLoopStartCutData, pNewSECutPt, pNewLeadIn, pNewLeadOut));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);
		}
		else // 起切点从图形移到图形上。
		{
			// 执行命令。
			ICommandPtr pCommand(new StartCutPtModifyCommand(GetEditorData(), pLoopStartCutData, iNewPatID, newPatPos));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);
		}
	}
	else
	{
		// 所有要调整的“回路起切特征”。
		LoopStartCutDataListPtr pGeomStaEndCuts(new LoopStartCutDataList); // 起切点在图形上的。
		LoopStartCutDataListPtr pCornerStaEndCuts(new LoopStartCutDataList); // 起切点在角上的。
		for (unsigned int i = 0; i < m_pAllLoopInstances->size(); i++)
		{
			LoopInstancePtr pLoopInstance =  m_pAllLoopInstances->at(i);
			if (pLoopInstance->GetCacheData()->GetPatternLoop()->GetID() == pLoopStartCutData->GetPatternLoop()->GetID())
			{
				LoopStartCutDataPtr pTmpLoopStaEndCut = pLoopInstance->GetCacheData()->GetLoopStartCutData();
				if (pTmpLoopStaEndCut->GetLoopStartCutPt()->GetSECutPtType() == SECUT_PT_GEOM)
					pGeomStaEndCuts->push_back(pTmpLoopStaEndCut);
				else
					pCornerStaEndCuts->push_back(pTmpLoopStaEndCut);
			}
		}

		// 执行命令。
		ICommandPtr pCommand(new StartCutPt2GeometryCommand(GetEditorData(), pGeomStaEndCuts, iNewPatID, newPatPos, pCornerStaEndCuts, pNewSECutPt, pNewLeadIn, pNewLeadOut));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}
}

void StartCutPtAdjustAction::UpdateNewPosition(const Point2D& cursorPos)
{
    IPatternLoopPtr pPatternLoop = m_pSelLoopInst->GetCacheData()->GetPatternLoop();
    Matrix2D mat = m_pSelLoopInst->GetTransformMatrix();
    if (pPatternLoop->GetPatternLoopType() == PatternLoop_Array)
    {
        PatternLoopNodePtr pPatternLoopNode = pPatternLoop->GetPatternLoopNodes()->at(0);
        IArrayPatternPtr pArrayPattern = boost::dynamic_pointer_cast<IArrayPattern>(pPatternLoopNode->GetPattern());
        IPatternPtr pFirstPat = pArrayPattern->GetFirstInstance();
        m_newPt = pFirstPat->GetClosestPt(cursorPos, mat);
        m_iPatIndex = 0;
    }
    else
    {
        pPatternLoop->GetClosestPt(cursorPos, m_newPt, m_iPatIndex, mat);
    }
}

END_CUTLEADER_NAMESPACE()
