#include "StdAfx.h"
#include "LoopLeadEditAction.h"

#include "baseConst.h"
#include "MathEx.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "Point2DList.h"
#include "LineArc2DList.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "IArrayPattern.h"
#include "PatternLoopNode.h"
#include "PatternLoopNodeList.h"
#include "IPatternLoop.h"
#include "LoopProcessor.h"

#include "LoopStartCutData.h"
#include "LoopStartCutDataList.h"
#include "LoopLeadPropertyBar.h"
#include "LoopCutFeatureDrawer.h"
#include "clCutFeatureResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

LoopLeadEditAction::LoopLeadEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort,
									   LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList)
   : StartCutPtPreSelectAction(pPatList, pPatternLoopList, pLoopFeatureList, pViewPort)
{
	m_pLeadConfigItem = pLeadConfigItem;
	m_pCondTypeList = pCondTypeList;
}

LoopLeadEditAction::~LoopLeadEditAction(void)
{
}

BOOL LoopLeadEditAction::LButtonDown(double& dPtX, double& dPtY)
{
	StartCutPtPreSelectAction::LButtonDown(dPtX, dPtY);

	return TRUE;
}

BOOL LoopLeadEditAction::LButtonUp(double& dPtX, double& dPtY)
{
	StartCutPtPreSelectAction::LButtonUp(dPtX, dPtY);

	if (m_selStaEndPtPairs.size() > 0)
	{
		// ���µ�ǰ��·��
		if (m_selStaEndPtPairs.size() == 1)
			m_pActiveLoop = m_selStaEndPtPairs.at(0).first->GetPatternLoop();

		// ��ʾ����·���������ߺ����е㡱���档
		LoopStartCutDataListPtr pLoopStaEndCutList(new LoopStartCutDataList);
		vector<Point2D> startPtList;
		for (unsigned int i = 0; i < m_selStaEndPtPairs.size(); i++)
		{
			pLoopStaEndCutList->push_back(m_selStaEndPtPairs.at(i).first);
			startPtList.push_back(m_selStaEndPtPairs.at(i).second);
		}
		LoopLeadPropertyBar::GetInstance()->DisplayLoopLead(GetEditorData(), pLoopStaEndCutList, startPtList, m_pLeadConfigItem);
	}

	return TRUE;
}

BOOL LoopLeadEditAction::MovePoint(double& dPtX, double& dPtY)
{
	StartCutPtPreSelectAction::MovePoint(dPtX, dPtY);

	return TRUE;
}

BOOL LoopLeadEditAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	StartCutPtPreSelectAction::OnKeyDown(nChar, nRepCnt, nFlags);

	// ѡ��ȫ������·���С�������
	if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		// ��ʾ����·���������ߺ����е㡱���档
		if (m_selStaEndPtPairs.size() > 0)
		{
			LoopStartCutDataListPtr pLoopStaEndCutList(new LoopStartCutDataList);
			vector<Point2D> startPtList;
			for (unsigned int i = 0; i < m_selStaEndPtPairs.size(); i++)
			{
				pLoopStaEndCutList->push_back(m_selStaEndPtPairs.at(i).first);
				startPtList.push_back(m_selStaEndPtPairs.at(i).second);
			}
			LoopLeadPropertyBar::GetInstance()->DisplayLoopLead(GetEditorData(), pLoopStaEndCutList, startPtList, m_pLeadConfigItem);
		}
	}

	return TRUE;
}

BOOL LoopLeadEditAction::DrawBeforeFinish()
{
	StartCutPtPreSelectAction::DrawBeforeFinish();

	return TRUE;
}

void LoopLeadEditAction::UpdateForUndoRedo()
{
	LoopLeadPropertyBar::GetInstance()->ShowBar(FALSE);
}

END_CUTLEADER_NAMESPACE()