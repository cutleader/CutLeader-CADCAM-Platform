#include "StdAfx.h"
#include "CutSideAdjustAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "DummyAction.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "LineArc2DList.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "PatternList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "LoopDrawer.h"

#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopStartCutCache.h"
#include "LoopStartCutData.h"
#include "clCutFeatureResource.h"
#include "CutSideChangeCommand.h"
#include "LoopStartCutProcessor.h"

#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "LoopInstanceList.h"
#include "PartInstance.h"
#include "PartInstanceList.h"
#include "LoopInstanceManager.h"
#include "CutSidesChangeCommand.h"

using namespace std;
using namespace std::tr1;

BEGIN_CUTLEADER_NAMESPACE()

CutSideAdjustAction::CutSideAdjustAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort, LeadConfigItemPtr pLeadConfigItem,
								   SpreadSamePartDlg* pSpreadSamePartDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_pLeadConfigItem = pLeadConfigItem;
	m_iMsgID = IDS_CHANGE_CUTSIDE;
	m_pAllLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
    m_pSpreadSamePartDlg = pSpreadSamePartDlg;
}

CutSideAdjustAction::~CutSideAdjustAction(void)
{
}

BOOL CutSideAdjustAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActiveLoopInstance)
	{
        ChangeCutSide_OfSelectedLoopInstance(m_pActiveLoopInstance);
	}

	return TRUE;
}

void CutSideAdjustAction::ChangeCutSide_OfSelectedLoopInstance(LoopInstancePtr pSelectedLoopInstance)
{
    // 检查是不是开放回路。
    IPatternLoopPtr pPatternLoop = pSelectedLoopInstance->GetCacheData()->GetPatternLoop();
    if (pPatternLoop->IsOpenPath())
    {
        CString str, strProductName;
        strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
        str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CHANGE_CUTSIDE_OPENLOOP);
        MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
        return;
    }

    // 得到选中“回路实例”的“回路起切特征”信息。
    LoopStartCutDataPtr pLoopStartCutData = pSelectedLoopInstance->GetCacheData()->GetLoopStartCutData();

    //
    LoopInstanceListPtr pLoopInstanceList(new LoopInstanceList);
    vector<tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double>> leadDataList_4_keepIt;
    for (unsigned int i = 0; i < m_pAllLoopInstanceList->size(); i++)
    {
        LoopInstancePtr pLoopInstance = m_pAllLoopInstanceList->at(i);
        if (pLoopInstance->GetPatternLoopID() == pSelectedLoopInstance->GetPatternLoopID() &&
            (!m_pSpreadSamePartDlg->IsSpread() ? pLoopInstance->GetPartInstance()->GetID() == pSelectedLoopInstance->GetPartInstance()->GetID() : true))
        {
            pLoopInstanceList->push_back(pLoopInstance);

            // 如果是第一次点击这个回路，记录上面的引线信息
            if (m_leadDataList_4_keepIt.find(make_pair(pLoopInstance->GetPartInstance()->GetID(), pLoopInstance->GetPatternLoopID())) == m_leadDataList_4_keepIt.end())
            {
                LoopStartCutDataPtr pLoopStaEndCut_tmp = pLoopInstance->GetCacheData()->GetLoopStartCutData();
                PatternLoopCutOffsetType iCutSide = LoopStartCutProcessor::CalculateLoopCutOffset(pLoopStaEndCut_tmp.get());
                double dLeadInLen = pLoopStartCutData->GetCacheData()->GetLeadInGeom()->GetTotalLength();
                double dLeadOutLen = pLoopStartCutData->GetCacheData()->GetLeadOutGeom()->GetTotalLength();
                tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double> leadData_4_keepIt = make_tuple(iCutSide, pLoopStaEndCut_tmp->GetLeadIn(), pLoopStaEndCut_tmp->GetLeadOut(), dLeadInLen, dLeadOutLen);
                m_leadDataList_4_keepIt[make_pair(pLoopInstance->GetPartInstance()->GetID(), pLoopInstance->GetPatternLoopID())] = leadData_4_keepIt;
            }

            tuple<PatternLoopCutOffsetType, ILeadPtr, ILeadPtr, double, double> leadData_4_keepIt = m_leadDataList_4_keepIt[make_pair(pLoopInstance->GetPartInstance()->GetID(), pLoopInstance->GetPatternLoopID())];
            leadDataList_4_keepIt.push_back(leadData_4_keepIt);
        }
    }

    // 执行命令
    EditorData editorData = GetEditorData();
    ICommandPtr pCommand(new CutSidesChangeCommand(editorData, pLoopInstanceList, leadDataList_4_keepIt, m_pLeadConfigItem));
    pCommand->Do();
    CommandManager::GetInstance()->AddCommand(pCommand);
}

BOOL CutSideAdjustAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);
	m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pAllLoopInstanceList, pt);

	return TRUE;
}

BOOL CutSideAdjustAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PartPlacementEditAction::OnKeyDown(nChar, nRepCnt, nFlags);

	return TRUE;
}

BOOL CutSideAdjustAction::DrawBeforeFinish()
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
