#include "StdAfx.h"
#include "HoleSequenceReplaceAction.h"

#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"
#include "CommandManager.h"

#include "IPatternLoop.h"
#include "PartItem.h"

#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "LoopInstanceList.h"
#include "PartInstance.h"
#include "PartInstanceList.h"
#include "PartPlacementDrawer.h"
#include "LoopInstanceManager.h"

#include "PartLayoutData.h"
#include "clCutSequenceResource.h"
#include "LoopSequence.h"
#include "GridLoopSequence.h"
#include "CutSequenceList.h"
#include "CutNodeManager.h"
#include "CutSequenceDrawer.h"
#include "CutSequenceSelectAction.h"
#include "CutSequenceManager.h"
#include "ConfigData.h"
#include "HoleSequenceReplaceCommand.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

HoleSequenceReplaceAction::HoleSequenceReplaceAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
    : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iMsgID = IDS_SEQ_REPLACE_HOLESEQ_SELECT_PART;

	m_pBasePartInst.reset();

	// �õ����С���·ʵ������
	m_pLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(m_pPartLayoutData->GetPartInstances());

	// �������ʵ���и�����·ʵ������Ӿ��Ρ�
	PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances();
	m_partRectMap = pPartInstances->GetLoopInstanceRect();
}

HoleSequenceReplaceAction::~HoleSequenceReplaceAction(void)
{
}

BOOL HoleSequenceReplaceAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// ��һЩ��顣
	{
		// ��û��ѡ�������
		if (!m_pBasePartInst)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REPLACE_HOLESEQ_SELECT_PART);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return TRUE;
		}

		// ����ڲ���������Ҫ����1��
		LoopInstanceListPtr pInnerLoopInsts_source = LoopInstanceManager::GetInnerLoopInsts(m_pBasePartInst);
		if (pInnerLoopInsts_source->size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REPLACE_HOLESEQ_2);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return TRUE;
		}

		// Դ����ڲ������Ƿ�����˹���
		map<int, ICutSequencePtr> innerLoopSeqs_source = m_pCutSequences->GetCutSequencesByLoopInstances_2(pInnerLoopInsts_source);
		if (innerLoopSeqs_source.size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REPLACE_HOLESEQ_1);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return TRUE;
		}

		// Դ������ڲ������Ĺ����ǲ��������ġ�
		if (!IsSeqLinked(innerLoopSeqs_source))
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REPLACE_HOLESEQ_3);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return TRUE;
		}

		// ��û���ֵ������
		PartInstanceListPtr pPartInsts_target = m_pPartLayoutData->GetPartInstances()->GetPartInstances_brother(m_pBasePartInst);
		if (pPartInsts_target->size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REPLACE_HOLESEQ_5);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return TRUE;
		}
	}

	// ִ���滻��
	strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REPLACE_HOLESEQ_4);
	if (MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		// ���ӹ���
		PartInstanceListPtr pPartInsts_target = m_pPartLayoutData->GetPartInstances()->GetPartInstances_brother(m_pBasePartInst);
		ICommandPtr pCommand(new HoleSequenceReplaceCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartInstances(), m_pBasePartInst, pPartInsts_target));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// ���¡��и�ڵ㡱���档
		RefreshCutNodes();

		// ���������
		m_pBasePartInst.reset();
	}

	return TRUE;
}

BOOL HoleSequenceReplaceAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// �õ���������ġ����ʵ������
	PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances();
	m_pBasePartInst = pPartInstances->GetClosestPartInstance(pt, m_partRectMap);

	return TRUE;
}

BOOL HoleSequenceReplaceAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL HoleSequenceReplaceAction::DrawBeforeFinish()
{
	// ���Ƶ�ǰ�ġ����ʵ������
	if (m_pBasePartInst)
	{
		PartPlacementDrawer partPlacementDrawer(m_pViewPort);
		partPlacementDrawer.DrawActivePartInstance(m_pBasePartInst.get());
	}

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void HoleSequenceReplaceAction::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

BOOL HoleSequenceReplaceAction::IsSeqLinked(map<int, ICutSequencePtr> seqMap)
{
	int iPreIndex = -1;
	for (map<int, ICutSequencePtr>::iterator iter = seqMap.begin(); iter != seqMap.end(); iter++)
	{
		LONGLONG iIndex = iter->first;
		if (iPreIndex == -1)
		{
			iPreIndex = iIndex;
		}
		else
		{
			if (iIndex == (iPreIndex+1))
				iPreIndex = iIndex;
			else
				return FALSE;
		}
	}

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
