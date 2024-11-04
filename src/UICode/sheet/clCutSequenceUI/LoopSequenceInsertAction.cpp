#include "StdAfx.h"
#include "LoopSequenceInsertAction.h"

#include "GlViewPort.h"
#include "Line2D.h"
#include "Rect2D.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "LoopDrawer.h"
#include "PartPlacementList.h"
#include "PartInstanceList.h"
#include "LoopInstanceCache.h"
#include "LoopInstanceList.h"
#include "LoopInstance.h"
#include "LoopInstanceManager.h"
#include "LoopToolManager.h"

#include "PartLayoutData.h"
#include "ICutSequence.h"
#include "CutSequenceList.h"
#include "clCutSequenceResource.h"
#include "LoopSequenceInsertCommand.h"
#include "LoopSequenceManager.h"
#include "LoopSequence.h"
#include "CutNodeManager.h"
#include "CutSequenceDrawer.h"
#include "CutSequenceSelectAction.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

LoopSequenceInsertAction::LoopSequenceInsertAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
										   : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_state = SEQUENCE_INSERT_NONE;
	m_iMsgID = IDS_SEQ_INSERT_BASE_SEQ;

	// �õ����С���·ʵ������
	m_pLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(m_pPartLayoutData->GetPartInstances());

	// ������������˹���ġ���·ʵ������
	m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();
}

LoopSequenceInsertAction::~LoopSequenceInsertAction(void)
{
}

BOOL LoopSequenceInsertAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString strMsg, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_state == SEQUENCE_INSERT_NONE || m_state == SEQUENCE_INSERT_SELECT_NEWLOOPINST)
	{
		if (!m_pActiveCutSeq)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_NO_SEQ_SELECT);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
		else
		{
			m_state = SEQUENCE_INSERT_SELECT_BASE_SEQ;
			m_iMsgID = IDS_SEQ_INSERT_NEWLOOP;
		}
	}
	else if (m_state == SEQUENCE_INSERT_SELECT_BASE_SEQ) 
	{
		if (!m_pActiveLoopInstance)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_SELECT_LOOPINST);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return TRUE;
		}

		// check whether the selected loop is sequenced.
		BOOL bLoopSequenced = m_pCutSequences->LoopInstSeqed(m_pActiveLoopInstance.get());
		if (bLoopSequenced)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_SEQUENCED);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
		else
		{
			m_state = SEQUENCE_INSERT_SELECT_NEWLOOPINST;
			m_iMsgID = IDS_SEQ_INSERT_BASE_SEQ;

			// create new sequence unit here, the trace data of it will be processed in "LoopSequenceInsertCommand".
			LoopSequencePtr pNewSeqUnit(new LoopSequence);
			pNewSeqUnit->SetLoopInst(m_pActiveLoopInstance);

			// create command to insert the sequence.
			ICommandPtr pCommand(new LoopSequenceInsertCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pActiveCutSeq, pNewSeqUnit));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// ������������˹���ġ���·ʵ������
			m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();

			// ���¡��и�ڵ㡱���档
			RefreshCutNodes();

			// �������ݡ�
			m_pActiveCutSeq.reset();
			m_pActiveLoopInstance.reset();
		}
	}

	return TRUE;
}

BOOL LoopSequenceInsertAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_state == SEQUENCE_INSERT_NONE || m_state == SEQUENCE_INSERT_SELECT_NEWLOOPINST)
	{
		// �����˹���ġ���·ʵ����������������Ǹ���
		LoopInstancePtr pCurrentLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pAllSequencedLoopInstances, pt);

		// �õ������������·ʵ�����ϵĹ���
		if (pCurrentLoopInstance)
			m_pActiveCutSeq = m_pCutSequences->GetCutSequenceByLoopInstance(pCurrentLoopInstance.get());
		else
			m_pActiveCutSeq.reset();
	}
	else if (m_state == SEQUENCE_INSERT_SELECT_BASE_SEQ)
	{
		m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInstEx(m_pLoopInstanceList, pt);
	}

	return TRUE;
}

BOOL LoopSequenceInsertAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL LoopSequenceInsertAction::DrawBeforeFinish()
{
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	if (m_state == SEQUENCE_INSERT_NONE || m_state == SEQUENCE_INSERT_SELECT_NEWLOOPINST)
	{
		// draw the active sequence unit in highlight mode.
		if (m_pActiveCutSeq)
        {
			cutSequenceDrawer.DrawActiveCutSequence(m_pActiveCutSeq.get(), m_pCutNodeList.get());
        }
	}
	else if (m_state == SEQUENCE_INSERT_SELECT_BASE_SEQ)
	{
		// ���ƻ�׼����
		if (m_pActiveCutSeq)
        {
			cutSequenceDrawer.DrawSelectedCutSequence(m_pActiveCutSeq.get(), m_pCutNodeList.get());
        }

		// ���Ƶ�ǰ�ġ���·ʵ������
		if (m_pActiveLoopInstance)
		{
            const LoopToolData* pLoopToolData = m_pActiveLoopInstance->GetCacheData()->GetLoopTool().get();
            LoopDrawer loopDrawer(m_pViewPort);
            loopDrawer.DrawActiveLoop(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), LoopToolManager::GetLoopDisplayColor(pLoopToolData));
		}
	}

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
