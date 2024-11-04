#include "StdAfx.h"
#include "CutSideChangeAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "DummyAction.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "LineArc2DList.h"

#include "PatternList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "LoopDrawer.h"

#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopStartCutData.h"
#include "LoopStartCutCache.h"
#include "clCutFeatureResource.h"
#include "CutSideChangeCommand.h"
#include "LoopStartCutProcessor.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;
using namespace std::tr1;

BEGIN_CUTLEADER_NAMESPACE()

CutSideChangeAction::CutSideChangeAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort,
								   LeadConfigItemPtr pLeadConfigItem)
    : CutFeatureEditBase(pPatList, pPatternLoopList, pLoopFeatureList, pViewPort)
{
	m_pLeadConfigItem = pLeadConfigItem;
	m_iMsgID = IDS_CHANGE_CUTSIDE;
}

CutSideChangeAction::~CutSideChangeAction(void)
{
}

BOOL CutSideChangeAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActiveLoop)
	{
		// 检查是不是开放回路。
		if (m_pActiveLoop->IsOpenPath())
		{
			CString str, strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CHANGE_CUTSIDE_OPENLOOP);
			MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
			return FALSE;
		}

		// get the pierce info of the selected loop.
		LoopCutFeaturePtr pLoopFeature = m_pLoopFeatureList->FindByPatternLoop(m_pActiveLoop);
		LoopStartCutDataPtr pLoopStartCutData = pLoopFeature->GetLoopStartCutData();

		// 如果是第一次点击这个回路，记录上面的引线信息
		if (m_leadDataList_4_keepIt.find(m_pActiveLoop->GetID()) == m_leadDataList_4_keepIt.end())
		{
            PatternLoopCutOffsetType iCutSide = LoopStartCutProcessor::CalculateLoopCutOffset(pLoopStartCutData.get());
			double dLeadInLen = pLoopStartCutData->GetCacheData()->GetLeadInGeom()->GetTotalLength();
			double dLeadOutLen = pLoopStartCutData->GetCacheData()->GetLeadOutGeom()->GetTotalLength();
			m_leadDataList_4_keepIt[m_pActiveLoop->GetID()] = make_tuple(iCutSide, pLoopStartCutData->GetLeadIn(), pLoopStartCutData->GetLeadOut(), dLeadInLen, dLeadOutLen);
		}

		// create the command to modify the start point of the loop.
		EditorData editorData = GetEditorData();
		ICommandPtr pCommand(new CutSideChangeCommand(editorData, pLoopStartCutData, m_leadDataList_4_keepIt[m_pActiveLoop->GetID()], m_pLeadConfigItem));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);
	}

	return TRUE;
}

BOOL CutSideChangeAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// get the loop which is nearest to the cursor.
	IPatternPtr pPattern = m_pPatList->GetPatternClosestTo(pt);
	if (pPattern)
    {
		m_pActiveLoop = m_pPatternLoopList->FindLoopByPat(pPattern.get());
    }

	return TRUE;
}

BOOL CutSideChangeAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CutFeatureEditBase::OnKeyDown(nChar, nRepCnt, nFlags);

	return TRUE;
}

BOOL CutSideChangeAction::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return TRUE;
}

BOOL CutSideChangeAction::DrawBeforeFinish()
{
	// get the color setting.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();

	// keep the old setting.
	COLORREF dwOldColor;
	ROP_MODE ropMode;
	m_pViewPort->GetDrawColor(dwOldColor);
	m_pViewPort->GetROP(ropMode);

	// 绘制当前的回路。
	if (m_pActiveLoop)
	{
		m_pViewPort->SetROP(ROP_COPY);
		m_pViewPort->SetLineWidth(2);
		m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());

		// draw the loop.
		LoopDrawer loopDrawer(m_pViewPort);
		loopDrawer.DrawLoopData(m_pActiveLoop);

		m_pViewPort->SetLineWidth(1);
	}

	// restore the draw setting.
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetROP(ropMode);

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
