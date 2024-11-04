#include "StdAfx.h"
#include "CutDirectionAdjustAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "DummyAction.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "PatternList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "LoopDrawer.h"

#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopStartCutData.h"
#include "LoopStartCutDataList.h"
#include "clCutFeatureResource.h"
#include "CutDirectionChangeCommand.h"

#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "LoopInstanceList.h"
#include "PartInstanceList.h"
#include "LoopInstanceManager.h"
#include "CutDirectionsChangeCommand.h"

BEGIN_CUTLEADER_NAMESPACE()

CutDirectionAdjustAction::CutDirectionAdjustAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
								 SpreadSamePartDlg* pSpreadSamePartDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_pSpreadSamePartDlg = pSpreadSamePartDlg;
	m_iMsgID = IDS_CHANGE_DIRECTION;
	m_pAllLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
}

CutDirectionAdjustAction::~CutDirectionAdjustAction(void)
{
}

BOOL CutDirectionAdjustAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActiveLoopInstance)
	{
        ChangeCutDirection_OfSelectedLoopInstance(m_pActiveLoopInstance);
	}

	return TRUE;
}

void CutDirectionAdjustAction::ChangeCutDirection_OfSelectedLoopInstance(LoopInstancePtr pSelectedLoopInstance)
{
    // 得到选中“回路实例”的“回路起切特征”信息。
    LoopStartCutDataPtr pLoopStartCutData = pSelectedLoopInstance->GetCacheData()->GetLoopStartCutData();

    if (m_pSpreadSamePartDlg->IsSpread())
    {
        //
        LoopStartCutDataListPtr pLoopStaEndCutList(new LoopStartCutDataList);
        for (unsigned int i = 0; i < m_pAllLoopInstanceList->size(); i++)
        {
            LoopInstancePtr pLoopInstance = m_pAllLoopInstanceList->at(i);
            if (pLoopInstance->GetPatternLoopID() == pSelectedLoopInstance->GetPatternLoopID())
            {
                pLoopStaEndCutList->push_back(pLoopInstance->GetCacheData()->GetLoopStartCutData());
            }
        }

        // 执行命令
        EditorData editorData = GetEditorData();
        ICommandPtr pCommand(new CutDirectionsChangeCommand(editorData, pLoopStaEndCutList));
        pCommand->Do();
        CommandManager::GetInstance()->AddCommand(pCommand);
    }
    else
    {
        // create the command to modify the start point of the loop.
        EditorData editorData = GetEditorData();
        ICommandPtr pCommand(new CutDirectionChangeCommand(editorData, pLoopStartCutData));
        pCommand->Do();
        CommandManager::GetInstance()->AddCommand(pCommand);
    }
}

BOOL CutDirectionAdjustAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);
	m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pAllLoopInstanceList, pt);

	return TRUE;
}

BOOL CutDirectionAdjustAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PartPlacementEditAction::OnKeyDown(nChar, nRepCnt, nFlags);

	return TRUE;
}

BOOL CutDirectionAdjustAction::DrawBeforeFinish()
{
	// get the color setting.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();

	// 绘制当前的“回路实例”。
	if (m_pActiveLoopInstance)
	{
		// get the old drawer config.
		COLORREF dwOldColor;
		m_pViewPort->GetDrawColor(dwOldColor);
		int iOldLineWid = m_pViewPort->GetLineWidth();
		ROP_MODE iOldRopMode;
		m_pViewPort->GetROP(iOldRopMode);

		// draw loop inst.
		m_pViewPort->SetROP(ROP_NORMAL);
		m_pViewPort->SetLineWidth(2);
		m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());
		LoopDrawer loopDrawer(m_pViewPort);
		loopDrawer.DrawLoopData(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), m_pActiveLoopInstance->GetTransformMatrix());

		// restore the old drawer config.
		m_pViewPort->SetLineWidth(1);
		m_pViewPort->SetDrawColor(dwOldColor);
		m_pViewPort->SetROP(iOldRopMode);
	}

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
