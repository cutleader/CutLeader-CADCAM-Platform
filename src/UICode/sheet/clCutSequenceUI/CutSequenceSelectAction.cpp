#include "StdAfx.h"
#include "CutSequenceSelectAction.h"

#include "GlViewPort.h"
#include "Rect2D.h"
#include "CursorDrawer.h"
#include "DummyAction.h"
#include "ValueInputManager.h"
#include "LogMgr.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "LoopInstanceManager.h"
#include "ICutPartSequence.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "CutNodeManager.h"
#include "PartLayoutData.h"
#include "clCutSequenceResource.h"


BEGIN_CUTLEADER_NAMESPACE()

CutSequenceSelectAction::CutSequenceSelectAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
	: SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_pSelSeqUnitList.reset(new CutSequenceList);
	m_pCurCutSeq.reset();
	m_bDisplayValInput = FALSE;
	m_iMsgID = IDS_SEQ_SELECT_SEQ;

	// ������������˹���ġ���·ʵ������
	m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();

	// ���湤��ģ����Ϣ
	m_pSequenceSimulateShapeList = CutNodeManager::CutNode2SimShape(m_pCutNodeList.get());
}

CutSequenceSelectAction::~CutSequenceSelectAction(void)
{
}

BOOL CutSequenceSelectAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pCurCutSeq)
	{
		if (!ValueInputManager::GetInstance()->IsInputCtrl())
			m_pSelSeqUnitList->clear();

		m_pSelSeqUnitList->push_back(m_pCurCutSeq);

		m_pCurCutSeq.reset();
	}

	return TRUE;
}

BOOL CutSequenceSelectAction::MovePoint(double& dPtX, double& dPtY)
{
	// �õ������˹���Ļ�·ʵ��������������ġ�
	Point2D pt(dPtX, dPtY);
	LoopInstancePtr pCurrentLoopInstance = LoopInstanceManager::GetClosestLoopInst_hasValidTool(m_pAllSequencedLoopInstances, pt);

	// �õ������������·ʵ�����ϵĹ���
	if (pCurrentLoopInstance)
	{
		m_pCurCutSeq = m_pCutSequences->GetCutSequenceByLoopInstance(pCurrentLoopInstance.get());

		// ��ʾ��ǰ�Ĺ������͡�
		if (m_pCurCutSeq)
		{
			if (m_pCurCutSeq->GetCutSequenceType() == CutSequence_Loop || m_pCurCutSeq->GetCutSequenceType() == CutSequence_GridLoop)
			{
				m_iMsgID = IDS_SEQ_ACTIVE_IS_LOOP_SEQ;
			}
			else if (m_pCurCutSeq->GetCutSequenceType() == CutSequence_Bridge)
			{
				m_iMsgID = IDS_SEQ_ACTIVE_IS_BDGE_SEQ;
			}
			else if (m_pCurCutSeq->GetCutSequenceType() == CutSequence_CommonCut)
			{
				m_iMsgID = IDS_SEQ_ACTIVE_IS_COMCUT_SEQ;
			}
            else if (m_pCurCutSeq->GetCutSequenceType() == CutSequence_GridCut)
            {
                m_iMsgID = IDS_SEQ_ACTIVE_IS_GridCut_SEQ;
            }
            else if (m_pCurCutSeq->GetCutSequenceType() == CutSequence_FlyCut)
            {
                m_iMsgID = IDS_SEQ_ACTIVE_IS_FlyCut_SEQ;
            }
            else if (m_pCurCutSeq->GetCutSequenceType() == CutSequence_ChainCut)
            {
                m_iMsgID = IDS_SEQ_ACTIVE_IS_ChainCut_SEQ;
            }
            else
            {
                LogMgr::DebugWarn(_T("741395"), CWnd::FromHandle(m_pViewPort->GetWnd()));
            }
		}
	}

	return TRUE;
}

BOOL CutSequenceSelectAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CutSequenceSelectAction::DrawBeforeFinish()
{
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// ���Ƶ�ǰ�Ĺ���
	if (m_pCurCutSeq)
    {
		cutSequenceDrawer.DrawActiveCutSequence(m_pCurCutSeq.get(), m_pCutNodeList.get());
    }

	// ����ѡ�еĹ���
	cutSequenceDrawer.DrawSelectedCutSequences(m_pSelSeqUnitList.get(), m_pCutNodeList.get());

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void CutSequenceSelectAction::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
