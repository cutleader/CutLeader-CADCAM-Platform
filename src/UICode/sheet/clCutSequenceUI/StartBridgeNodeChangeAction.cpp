#include "StdAfx.h"
#include "StartBridgeNodeChangeAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "LoopDrawer.h"
#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "LoopInstanceList.h"
#include "LoopInstanceManager.h"
#include "LoopToolManager.h"
#include "clCutSequenceResource.h"
#include "CutNodeList.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "CutNodeManager.h"
#include "BridgeNode.h"
#include "BridgeSequence.h"
#include "BridgeSequenceManager.h"
#include "PartLayoutData.h"
#include "StartBridgeNodeChangeCommand.h"
#include "CutSequenceSelectAction.h"


BEGIN_CUTLEADER_NAMESPACE()

StartBridgeNodeChangeAction::StartBridgeNodeChangeAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
											 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iMsgID = IDS_SEQ_BDGE_START_SELECT;

	// �������С��Žӹ����еġ���·ʵ������
	m_pBdgeLoopInsts.reset(new LoopInstanceList);
	CutSequenceListPtr pBdgeSeqUnits = m_pCutSequences->GetCutSequencesByType(CutSequence_Bridge);
	for (unsigned int i = 0; i < pBdgeSeqUnits->size(); i++)
	{
		BridgeSequencePtr pBridgeSequence = boost::dynamic_pointer_cast<BridgeSequence>(pBdgeSeqUnits->at(i));
		pBridgeSequence->GetRootBdgeNode()->GetAllLoopInsts(m_pBdgeLoopInsts);
	}
}

StartBridgeNodeChangeAction::~StartBridgeNodeChangeAction(void)
{
}

BOOL StartBridgeNodeChangeAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActiveLoopInstance)
	{
		// �õ���ǰ����·ʵ�������ڵġ��Žӹ��򡱺͡�����ڵ㡱��
		BridgeSequencePtr pCurBdgeSeqUnit;
		BridgeNodePtr pCurBdgeNode;
		CutSequenceListPtr pBdgeSeqUnits = m_pCutSequences->GetCutSequencesByType(CutSequence_Bridge);
		for (unsigned int i = 0; i < pBdgeSeqUnits->size(); i++)
		{
			BridgeSequencePtr pBridgeSequence = boost::dynamic_pointer_cast<BridgeSequence>(pBdgeSeqUnits->at(i));
			BridgeNodePtr pBridgeNode = pBridgeSequence->GetBdgeNodeByLoopInst(m_pActiveLoopInstance);
			if (pBridgeNode)
			{
				pCurBdgeSeqUnit = pBridgeSequence;
				pCurBdgeNode = pBridgeNode;
				break;
			}
		}

		// �޸ġ��Žӹ��򡱵���ʼ��·��
		ICommandPtr pCommand(new StartBridgeNodeChangeCommand(GetEditorData(), m_pCutSequences, m_pConfigData,
												   pCurBdgeSeqUnit, pCurBdgeNode));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// ���¡��и�ڵ㡱���档
		RefreshCutNodes();
	}

	return TRUE;
}

BOOL StartBridgeNodeChangeAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);
	m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pBdgeLoopInsts, pt);

	return TRUE;
}

BOOL StartBridgeNodeChangeAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL StartBridgeNodeChangeAction::DrawBeforeFinish()
{
	LoopDrawer loopDrawer(m_pViewPort);

	// ���Ƶ�ǰ�ġ���·ʵ������
	if (m_pActiveLoopInstance)
	{
		const LoopToolData* pLoopToolData = m_pActiveLoopInstance->GetCacheData()->GetLoopTool().get();
		loopDrawer.DrawActiveLoop(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), LoopToolManager::GetLoopDisplayColor(pLoopToolData), m_pActiveLoopInstance->GetTransformMatrix());
	}

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void StartBridgeNodeChangeAction::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
