#include "StdAfx.h"
#include "StartCutPtChangeAction.h"

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
#include "clCutFeatureResource.h"
#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopCutFeatureDrawer.h"
#include "LeadManager.h"
#include "StartCutPtModifyCommand.h"
#include "StartCutPtModifyCommandA.h"
#include "LoopStartCutProcessor.h"
#include "LoopToolDataList.h"
#include "LoopToolManager.h"

#include "ParamConfig.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "LeadConfigItem.h"

BEGIN_CUTLEADER_NAMESPACE()

StartCutPtChangeAction::StartCutPtChangeAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList,
										 GlViewPortPtr pViewPort, LeadConfigItemPtr pLeadConfigItem)
     : CutFeatureEditBase(pPatList, pPatternLoopList, pLoopFeatureList, pViewPort)
{
    m_iMsgID = IDS_STARTPT_ADJUST_SEL_LOOP;

	m_pLeadConfigItem = pLeadConfigItem;
	m_state = START_PT_CHANGE_NONE;
	m_iPatIndex = INVALID_IDX;
}

StartCutPtChangeAction::~StartCutPtChangeAction(void)
{
}

BOOL StartCutPtChangeAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActiveLoop)
	{
		if (m_state == START_PT_CHANGE_NONE)
		{
			// 检查是不是开放回路。
			if (m_pActiveLoop->IsOpenPath())
			{
				CString str, strProductName;
				strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STARTPT_ADJUST_SEL_OPENLOOP);
				MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
				return FALSE;
			}
			else
            {
				m_pSelectedLoop = m_pActiveLoop;
                UpdateNewPosition(Point2D(dPtX, dPtY)); // 新的起切点
            }

			m_iMsgID = IDS_STARTPT_ADJUST_SEL_PT;
			m_state = START_PT_CHANGE_SELECTLOOP;
		}
		else if (m_state == START_PT_CHANGE_SELECTLOOP)
		{
			LoopCutFeaturePtr pLoopFeature = m_pLoopFeatureList->FindByPatternLoop(m_pSelectedLoop);
			LoopStartCutDataPtr pLoopStartCutData = pLoopFeature->GetLoopStartCutData();
			ILoopStartCutPtPtr pLoopStartCutPt = pLoopStartCutData->GetLoopStartCutPt();

			// 移动后的起切点位置。
			BOOL bDir;
			if (m_iPatIndex == INVALID_IDX)
				return FALSE;
			IPatternPtr pPattern = m_pActiveLoop->GetPatternByLoopNodeIndex(m_iPatIndex, bDir);
			PatternPosition secondPatPos;
			PatternPosition patternPosition = pPattern->GetPatPosByPoint(m_newPt, secondPatPos);

			if (m_newPt != pLoopStartCutData->GetCacheData()->GetStartCutPt()) // 是否做了移动。
			{
				if (LoopProcessor::PtAtLoopCorner(m_pActiveLoop.get(), m_newPt)) // 起切点移到了角上。
				{
					double dCornerAngle = LoopProcessor::GetAngAtLoopCorner(m_pActiveLoop.get(), m_newPt);
					if (!MathEx::Equal(dCornerAngle, PI, GEOM_TOLERANCE))
					{
						// get PatternPosition data of the adjacent geom.
						LONGLONG iNextPatID;
						PatternPosition nextPatPos;
						LoopProcessor::GetNextCornerGeomPos(m_pActiveLoop.get(), m_newPt, pPattern->GetID(), patternPosition, iNextPatID, nextPatPos);

						// create new start/end cut point info.
						CornerStartCutPtPtr pNewSECutPt(new CornerStartCutPt(pPattern->GetID(), patternPosition, iNextPatID, nextPatPos, pLoopStartCutData->SameDirWithLoop()));
                        pNewSECutPt->SetOverCut(m_pLeadConfigItem->GetOverCutAtCorner());

                        PatternLoopCutOffsetType iLoopCutOffset = LoopStartCutProcessor::CalculateLoopCutOffset(pLoopStartCutData.get());

						// 角上的引入线。
						double dLeadInLen = pLoopStartCutData->GetCacheData()->GetLeadInGeom()->GetTotalLength();
						ILeadPtr pLeadIn = LeadManager::BuildCornerLead_byLeadLength(m_pActiveLoop.get(), pLoopStartCutData->SameDirWithLoop(), iLoopCutOffset, LeadInOut_In, m_newPt, dLeadInLen);

						// 角上的引出线。
						double dLeadOutLen = pLoopStartCutData->GetCacheData()->GetLeadOutGeom()->GetTotalLength();
						ILeadPtr pLeadOut = LeadManager::BuildCornerLead_byLeadLength(m_pActiveLoop.get(), pLoopStartCutData->SameDirWithLoop(), iLoopCutOffset, LeadInOut_Out, m_newPt, dLeadOutLen);

						// create the command to modify the start point of the loop.
						ICommandPtr pCommand(new StartCutPtModifyCommandA(GetEditorData(), pLoopStartCutData, pNewSECutPt, pLeadIn, pLeadOut));
						pCommand->Do();
						CommandManager::GetInstance()->AddCommand(pCommand);
					}
					else
					{
						// 这个位置不能放起切点。
						return FALSE;
					}
				}
				else
				{
					if (pLoopStartCutPt->GetSECutPtType() == SECUT_PT_CORNER) // 起切点从角移到图形上。
					{
						ILoopStartCutPtPtr pNewSECutPt(new GeometryStartCutPt(pPattern->GetID(), patternPosition, pLoopStartCutData->SameDirWithLoop()));
						ILeadPtr pLeadIn = LeadManager::BuildGeomLead(pLoopStartCutData->GetPatternLoop().get(), LeadInOut_In, m_pLeadConfigItem.get());
						ILeadPtr pLeadOut = LeadManager::BuildGeomLead(pLoopStartCutData->GetPatternLoop().get(), LeadInOut_Out, m_pLeadConfigItem.get());

						// create the command to modify the start point of the loop.
						ICommandPtr pCommand(new StartCutPtModifyCommandA(GetEditorData(), pLoopStartCutData, pNewSECutPt, pLeadIn, pLeadOut));
						pCommand->Do();
						CommandManager::GetInstance()->AddCommand(pCommand);
					}
					else
					{
						ICommandPtr pCommand(new StartCutPtModifyCommand(GetEditorData(), pLoopStartCutData, pPattern->GetID(), patternPosition));
						pCommand->Do();
						CommandManager::GetInstance()->AddCommand(pCommand);
					}
				}
			}

			// 重设变量。
			m_state = START_PT_CHANGE_NONE;
			m_iMsgID = IDS_STARTPT_ADJUST_SEL_LOOP;
			m_pSelectedLoop.reset();
		}
	}

	return TRUE;
}

BOOL StartCutPtChangeAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_state == START_PT_CHANGE_NONE)
	{
		// 找到距离光标最近的回路。
		IPatternPtr pPattern = m_pPatList->GetPatternClosestTo(pt);
		if (pPattern)
        {
			m_pActiveLoop = m_pPatternLoopList->FindLoopByPat(pPattern.get());
        }
	} 
	else if (m_state == START_PT_CHANGE_SELECTLOOP)
	{
		// 新的起切点
        UpdateNewPosition(Point2D(dPtX, dPtY));
	}

	return TRUE;
}

BOOL StartCutPtChangeAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CutFeatureEditBase::OnKeyDown(nChar, nRepCnt, nFlags);

    if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
    {
        if (m_state == START_PT_CHANGE_SELECTLOOP)
        {
            m_state = START_PT_CHANGE_NONE;
            m_iMsgID = IDS_STARTPT_ADJUST_SEL_LOOP;
            m_pSelectedLoop.reset();
        }
    }

	return TRUE;
}

BOOL StartCutPtChangeAction::DrawBeforeFinish()
{
	if (m_state == START_PT_CHANGE_NONE)
	{
		if (m_pActiveLoop)
		{
            const LoopToolData* pLoopToolData = m_pLoopFeatureList->GetLoopToolList()->GetLoopTool_by_loopID(m_pActiveLoop->GetID()).get();
			LoopDrawer loopDrawer(m_pViewPort);
            loopDrawer.DrawActiveLoop(m_pActiveLoop, LoopToolManager::GetLoopDisplayColor(pLoopToolData));
		}
	} 
	else if (m_state == START_PT_CHANGE_SELECTLOOP)
	{
		// 绘制环。
		LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);
		loopFeatureDrawer.DrawPosRing(m_newPt);
	}

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void StartCutPtChangeAction::UpdateNewPosition(const Point2D& cursorPos)
{
    if (m_pSelectedLoop->GetPatternLoopType() == PatternLoop_Array)
    {
        PatternLoopNodePtr pPatternLoopNode = m_pSelectedLoop->GetPatternLoopNodes()->at(0);
        IArrayPatternPtr pArrayPattern = boost::dynamic_pointer_cast<IArrayPattern>(pPatternLoopNode->GetPattern());
        IPatternPtr pFirstPat = pArrayPattern->GetFirstInstance();
        m_newPt = pFirstPat->GetClosestPt(cursorPos, Matrix2D());
        m_iPatIndex = 0;
    }
    else
    {
        m_pSelectedLoop->GetClosestPt(cursorPos, m_newPt, m_iPatIndex, Matrix2D());
    }
}

END_CUTLEADER_NAMESPACE()
