#include "StdAfx.h"
#include "LoopLeadEditAction2.h"

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
#include "LogMgr.h"

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

#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "LoopInstanceList.h"
#include "PartInstanceList.h"
#include "LoopInstanceManager.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

LoopLeadEditAction2::LoopLeadEditAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort, 
										 LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList, SpreadSamePartDlg* pSpreadSamePartDlg)
    : StartCutPtPreSelectAction2(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_pSpreadSamePartDlg = pSpreadSamePartDlg;
	m_pSpreadSamePartDlg->AddObserver(this);
	m_pLeadConfigItem = pLeadConfigItem;
	m_pCondTypeList = pCondTypeList;
	m_pAllLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
}

LoopLeadEditAction2::~LoopLeadEditAction2(void)
{
	m_pSpreadSamePartDlg->RemoveObserver(GetID());
}

BOOL LoopLeadEditAction2::LButtonDown(double& dPtX, double& dPtY)
{
	StartCutPtPreSelectAction2::LButtonDown(dPtX, dPtY);

	return TRUE;
}

BOOL LoopLeadEditAction2::LButtonUp(double& dPtX, double& dPtY)
{
	StartCutPtPreSelectAction2::LButtonUp(dPtX, dPtY);

	DisplayLoopLead();

	return TRUE;
}

BOOL LoopLeadEditAction2::MovePoint(double& dPtX, double& dPtY)
{
	StartCutPtPreSelectAction2::MovePoint(dPtX, dPtY);

	return TRUE;
}

BOOL LoopLeadEditAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	StartCutPtPreSelectAction2::OnKeyDown(nChar, nRepCnt, nFlags);

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

BOOL LoopLeadEditAction2::DrawBeforeFinish()
{
	StartCutPtPreSelectAction2::DrawBeforeFinish();

	return TRUE;
}

void LoopLeadEditAction2::UpdateForUndoRedo()
{
	LoopLeadPropertyBar::GetInstance()->ShowBar(FALSE);
}

void LoopLeadEditAction2::UpdateActionBuffer()
{
	m_pAllLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(m_pPartInstanceList);
}

void LoopLeadEditAction2::Update(int iEventID, VARIANT varEventContent)
{
	// SpreadSamePartDlg�б仯��������Ҫ������ʾһ��
	DisplayLoopLead();
}

void LoopLeadEditAction2::EditLeadsOfSelectedLoopInstances(LoopInstanceListPtr pSelectedLoopInstances)
{
    if (!pSelectedLoopInstances)
    {
        LogMgr::DebugWarn(_T("640218"), CWnd::FromHandle(m_pViewPort->GetWnd()));
        return;
    }

    m_selStaEndPtPairs = pSelectedLoopInstances->GetAllStartCutPtPairs();
    DisplayLoopLead();
}

void LoopLeadEditAction2::DisplayLoopLead()
{
	if (m_selStaEndPtPairs.size() > 0)
	{
		// ��ʾ����·���������ߺ����е㡱���档
		LoopStartCutDataListPtr pLoopStaEndCutList(new LoopStartCutDataList);
		vector<Point2D> startPtList;
		for (unsigned int i = 0; i < m_selStaEndPtPairs.size(); i++)
		{
			pLoopStaEndCutList->push_back(m_selStaEndPtPairs.at(i).first);
			startPtList.push_back(m_selStaEndPtPairs.at(i).second);
		}
		if (m_pSpreadSamePartDlg->IsSpread())
		{
			LoopStartCutDataListPtr pLoopStaEndCutList_involveSameParts(new LoopStartCutDataList);
			vector<Point2D> startPtList_involveSameParts;
			for (unsigned int i = 0; i < pLoopStaEndCutList->size(); i++)
			{
				IPatternLoopPtr pPatternLoop = pLoopStaEndCutList->at(i)->GetPatternLoop();
				for (unsigned int j = 0; j < m_pAllLoopInstanceList->size(); j++)
				{
					LoopInstancePtr pLoopInstance = m_pAllLoopInstanceList->at(j);
					if (pLoopInstance->GetPatternLoopID() == pPatternLoop->GetID())
					{
						pLoopStaEndCutList_involveSameParts->push_back(pLoopInstance->GetCacheData()->GetLoopStartCutData());
						startPtList_involveSameParts.push_back(pLoopInstance->GetStartCutPt());
					}
				}
			}
			LoopLeadPropertyBar::GetInstance()->DisplayLoopLead(GetEditorData(), pLoopStaEndCutList_involveSameParts, startPtList_involveSameParts, m_pLeadConfigItem);
		}
		else
		{
			LoopLeadPropertyBar::GetInstance()->DisplayLoopLead(GetEditorData(), pLoopStaEndCutList, startPtList, m_pLeadConfigItem);
		}
	}
}

END_CUTLEADER_NAMESPACE()
