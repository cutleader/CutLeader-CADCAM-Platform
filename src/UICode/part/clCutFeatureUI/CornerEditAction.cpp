#include "StdAfx.h"
#include "CornerEditAction.h"

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
#include "CornerPropertyBar.h"
#include "LoopCutFeatureDrawer.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

CornerEditAction::CornerEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort,
                                   CornerConfigItemPtr pCornerConfigItem)
    : CornerPreSelectAction(pPatList, pPatternLoopList, pLoopFeatureList, pViewPort)
{
    m_pCornerConfigItem = pCornerConfigItem;
}

CornerEditAction::~CornerEditAction(void)
{
}

BOOL CornerEditAction::LButtonDown(double& dPtX, double& dPtY)
{
	CornerPreSelectAction::LButtonDown(dPtX, dPtY);

	return TRUE;
}

BOOL CornerEditAction::LButtonUp(double& dPtX, double& dPtY)
{
	CornerPreSelectAction::LButtonUp(dPtX, dPtY);

	if (m_selCornerPtPairs.size() > 0)
	{
		// ��ʾ�������༭���档
		CornerListPtr pCornerList(new CornerList);
		for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
        {
			pCornerList->push_back(m_selCornerPtPairs.at(i).first);
        }
		CornerPropertyBar::GetInstance()->DisplayCornerData_4_edit(GetEditorData(), m_pLoopFeatureList->GetLoopCornerList(), pCornerList, m_pCornerConfigItem);
	}

	return TRUE;
}

BOOL CornerEditAction::MovePoint(double& dPtX, double& dPtY)
{
	CornerPreSelectAction::MovePoint(dPtX, dPtY);

	return TRUE;
}

BOOL CornerEditAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CornerPreSelectAction::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		CornerPropertyBar::GetInstance()->ShowPane(FALSE, FALSE, TRUE);
	}
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		// ��ʾѡ�еġ������������ԡ�
		if (m_selCornerPtPairs.size() > 0)
		{
			CornerListPtr pCornerList(new CornerList);
			for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
				pCornerList->push_back(m_selCornerPtPairs.at(i).first);
			CornerPropertyBar::GetInstance()->DisplayCornerData_4_edit(GetEditorData(), m_pLoopFeatureList->GetLoopCornerList(), pCornerList, m_pCornerConfigItem);
		}
	}
	else if (ValueInputManager::GetInstance()->IsInputDel(nChar))
	{
		// ɾ��ѡ�еġ�����������
		if (m_selCornerPtPairs.size() > 0)
		{
			CornerListPtr pCornerList(new CornerList);
			for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
				pCornerList->push_back(m_selCornerPtPairs.at(i).first);
			ICommandPtr pCommand(new CornerDeleteCommand(GetEditorData(), m_pLoopFeatureList->GetLoopCornerList(), pCornerList));
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

BOOL CornerEditAction::DrawBeforeFinish()
{
	CornerPreSelectAction::DrawBeforeFinish();

	return TRUE;
}

void CornerEditAction::UpdateForUndoRedo()
{
	// ����ʾ����ǰ�ġ��͡�ѡ�еġ��������㡣
	m_selCornerPtPairs.clear();

	// ���ؽ��������Խ��档
	CornerPropertyBar::GetInstance()->ShowBar(FALSE);
}

END_CUTLEADER_NAMESPACE()
