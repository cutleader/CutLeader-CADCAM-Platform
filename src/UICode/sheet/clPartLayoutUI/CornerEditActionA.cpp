#include "StdAfx.h"
#include "CornerEditAction2.h"

#include "CommandManager.h"
#include "GlViewPort.h"
#include "DummyAction.h"
#include "Point2DList.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"

#include "PatternLoopList.h"
#include "PatternList.h"
#include "PolygonPatternLoop.h"

#include "RadiusCorner.h"
#include "CornerModifyCommand.h"
#include "CornerDeleteCommand.h"
#include "LoopCutFeatureList.h"
#include "LoopCutFeature.h"
#include "LoopCornerDataList.h"
#include "LoopCornerData.h"
#include "CornerList.h"
#include "CutFeatureEditor.h"
#include "CornerPropertyBar.h"
#include "LoopCutFeatureDrawer.h"

#include "LoopInstanceList.h"
#include "PartInstanceList.h"
#include "PartPlacementList.h"
#include "LoopInstanceManager.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

CornerEditAction2::CornerEditAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
									 SpreadSamePartDlg* pSpreadSamePartDlg, CornerConfigItemPtr pCornerConfigItem)
    : CornerPreSelectAction2(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_pSpreadSamePartDlg = pSpreadSamePartDlg;
	m_pSpreadSamePartDlg->AddObserver(this);
	m_pAllLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
    m_pCornerConfigItem = pCornerConfigItem;
}

CornerEditAction2::~CornerEditAction2(void)
{
	m_pSpreadSamePartDlg->RemoveObserver(GetID());
}

BOOL CornerEditAction2::LButtonDown(double& dPtX, double& dPtY)
{
	CornerPreSelectAction2::LButtonDown(dPtX, dPtY);

	return TRUE;
}

BOOL CornerEditAction2::LButtonUp(double& dPtX, double& dPtY)
{
	CornerPreSelectAction2::LButtonUp(dPtX, dPtY);

	DisplayCornerData();

	return TRUE;
}

BOOL CornerEditAction2::MovePoint(double& dPtX, double& dPtY)
{
	CornerPreSelectAction2::MovePoint(dPtX, dPtY);

	return TRUE;
}

BOOL CornerEditAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CornerPreSelectAction2::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		CornerPropertyBar::GetInstance()->ShowPane(FALSE, FALSE, TRUE);
	}
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		// ��ʾѡ�еġ������������ԡ�
		if (m_selCornerPtPairs.size() > 0)
		{
			LoopCornerDataListPtr pLoopCornerList = m_pPartPlacementList->GetAllLoopCornerList();
			CornerListPtr pCornerList(new CornerList);
			for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
				pCornerList->push_back(m_selCornerPtPairs.at(i).first);
			CornerPropertyBar::GetInstance()->DisplayCornerData_4_edit(GetEditorData(), pLoopCornerList, pCornerList, m_pCornerConfigItem);
		}
	}
	else if (ValueInputManager::GetInstance()->IsInputDel(nChar))
	{
		// ɾ��ѡ�еġ�����������
		if (m_selCornerPtPairs.size() > 0)
		{
			LoopCornerDataListPtr pLoopCornerList = m_pPartPlacementList->GetAllLoopCornerList();
			CornerListPtr pCornerList(new CornerList);
			for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
				pCornerList->push_back(m_selCornerPtPairs.at(i).first);
			ICommandPtr pCommand(new CornerDeleteCommand(GetEditorData(), pLoopCornerList, pCornerList));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// ���ѡ�еġ�����������
			m_selCornerPtPairs.clear();

			// ���ؽ��������Խ��档
			CornerPropertyBar::GetInstance()->ShowBar(FALSE);
		}
	}

	return TRUE;
}

BOOL CornerEditAction2::DrawBeforeFinish()
{
	CornerPreSelectAction2::DrawBeforeFinish();

	return TRUE;
}

void CornerEditAction2::UpdateForUndoRedo()
{
	// ����ʾ����ǰ�ġ��͡�ѡ�еġ��������㡣
	m_pActiveCornerPt.reset();
	m_selCornerPtPairs.clear();

	// ���ؽ��������Խ��档
	CornerPropertyBar::GetInstance()->ShowBar(FALSE);
}

void CornerEditAction2::Update(int iEventID, VARIANT varEventContent)
{
	// SpreadSamePartDlg�б仯��������Ҫ������ʾһ��
	DisplayCornerData();
}

void CornerEditAction2::DisplayCornerData()
{
	// ��ʾ�������༭���档
	if (m_selCornerPtPairs.size() > 0)
	{
		LoopCornerDataListPtr pLoopCornerList = m_pPartPlacementList->GetAllLoopCornerList();
		CornerListPtr pCornerList(new CornerList);
		for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
		{
			pCornerList->push_back(m_selCornerPtPairs.at(i).first);
		}
		if (m_pSpreadSamePartDlg->IsSpread())
		{
			CornerListPtr pCornerList_involveSameParts(new CornerList);
			for (unsigned int i = 0; i < pCornerList->size(); i++)
			{
				ICornerPtr pCorner = pCornerList->at(i);
				for (unsigned int j = 0; j < pLoopCornerList->size(); j++)
				{
					LoopCornerDataPtr pLoopCorner = pLoopCornerList->at(j);
					ICornerPtr pCorner_involveSameParts = pLoopCorner->GetCornerByPat(pCorner->GetFirstPatternID(), pCorner->GetSecondPatternID());
					if (pCorner_involveSameParts)
					{
						pCornerList_involveSameParts->push_back(pCorner_involveSameParts);
					}
				}
			}
			CornerPropertyBar::GetInstance()->DisplayCornerData_4_edit(GetEditorData(), pLoopCornerList, pCornerList_involveSameParts, m_pCornerConfigItem);
		}
		else
		{
			CornerPropertyBar::GetInstance()->DisplayCornerData_4_edit(GetEditorData(), pLoopCornerList, pCornerList, m_pCornerConfigItem);
		}
	}
}

END_CUTLEADER_NAMESPACE()
