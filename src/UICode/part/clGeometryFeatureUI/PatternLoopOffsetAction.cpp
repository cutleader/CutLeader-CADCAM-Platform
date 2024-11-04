#include "StdAfx.h"
#include "PatternLoopOffsetAction.h"

#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "IPattern.h"
#include "PatternList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "LoopDrawer.h"
#include "LoopProcessor.h"
#include "clGeometryFeatureResource.h"
#include "PatternLoopOffsetDlg.h"

#include "DisplayParam.h"
#include "CLOptionInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternLoopOffsetAction::PatternLoopOffsetAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, GlViewPortPtr pViewPort)
{
	m_bDisplayValInput = FALSE;
	m_iMsgID = IDS_OUTPUT_OFFSETLOOP_SELECTLOOP;
	m_pViewPort = pViewPort;

	m_pPatList = pPatList;
	m_pPatternLoopList = pPatternLoopList;
	m_bSupportDrawAssit = FALSE;

	m_state = OFFSET_LOOP_NOTSTART;
	m_pActiveLoop.reset();
}

PatternLoopOffsetAction::~PatternLoopOffsetAction(void)
{
}

BOOL PatternLoopOffsetAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString strMsg, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_pActiveLoop)
	{
		// ����ǲ��Ǳպϻ�·��
		if (m_pActiveLoop->IsOpenPath())
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_OFFSETLOOP_OPEN_LOOP);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
		else
		{
			// ����ƫ�Ʋ�����
			PatternLoopOffsetDlg dlg;
			if (dlg.DoModal() == IDOK)
			{
				// ִ��ƫ�ơ�
				BOOL bInside = dlg.IsInside();
				double dOffsetVal = dlg.GetOffsetVal();
				if (!LoopProcessor::OffsetPolyLoop(m_pPatList, m_pActiveLoop, bInside, dOffsetVal))
				{
					strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_OFFSETLOOP_PROP_VAL);
					MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
				}
			}
		}

		// ����ѡ��ͼ��ģʽ��
		EnterPatSelAction(m_pActiveLoop->GetPatternList());
	}

	return TRUE;
}

BOOL PatternLoopOffsetAction::MovePoint(double& dPtX, double& dPtY)
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

BOOL PatternLoopOffsetAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PatternEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	return TRUE;
}

BOOL PatternLoopOffsetAction::DrawBeforeFinish()
{
	// get the color setting.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();

	// keep the old setting.
	COLORREF dwOldColor;
	ROP_MODE ropMode;
	m_pViewPort->GetDrawColor(dwOldColor);
	m_pViewPort->GetROP(ropMode);

	/************************************************************************/
	// draw the selected loop in highlight mode.

	if (m_pActiveLoop)
	{
		m_pViewPort->SetROP(ROP_COPY);
		m_pViewPort->SetLineWidth(2);

		// draw the loop.
		LoopDrawer loopDrawer(m_pViewPort);
		loopDrawer.DrawLoopData(m_pActiveLoop);

		m_pViewPort->SetLineWidth(1);
	}
	/************************************************************************/

	// restore the draw setting.
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetROP(ropMode);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
