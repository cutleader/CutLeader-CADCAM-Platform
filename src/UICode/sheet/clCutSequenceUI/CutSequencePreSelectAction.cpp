#include "StdAfx.h"
#include "CutSequencePreSelectAction.h"

#include "GlViewPort.h"
#include "Rect2D.h"
#include "CursorDrawer.h"
#include "DummyAction.h"
#include "ValueInputManager.h"

#include "PartInstanceList.h"
#include "LoopInstance.h"
#include "LoopInstanceManager.h"

#include "ICutPartSequence.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "CutNodeManager.h"
#include "PartLayoutData.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

CutSequencePreSelectAction::CutSequencePreSelectAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, 
								 ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg)
								 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_pApplyPartDlg = pApplyPartDlg;

	m_pCurrentCutSequences.reset(new CutSequenceList);
	m_pSelectCutSequences.reset(new CutSequenceList);

	m_iPreSelectSequenceActionState = PreSelectSequenceAction_active;
	m_bDisplayValInput = FALSE;

	// ������������˹���ġ���·ʵ������
	m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();
}

CutSequencePreSelectAction::~CutSequencePreSelectAction(void)
{
}

BOOL CutSequencePreSelectAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pCurrentCutSequences->size() > 0)
		m_pSelectCutSequences->AppendCutSequences(m_pCurrentCutSequences);

	return TRUE;
}

BOOL CutSequencePreSelectAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	m_pCurrentCutSequences->clear();

	// �����˹���Ļ�·ʵ��������������ġ�
	LoopInstancePtr pCurrentLoopInstance = LoopInstanceManager::GetClosestLoopInst_hasValidTool(m_pAllSequencedLoopInstances, pt);
	if (pCurrentLoopInstance)
	{
		// �����������·ʵ�����ϵĹ���
		ICutPartSequencePtr pCutPartSeq = m_pCutSequences->GetCutSequenceByLoopInstance(pCurrentLoopInstance.get());
		if (m_pApplyPartDlg->IsApplyPart())
		{
			if (pCutPartSeq->GetCutSequenceType() == CutSequence_Bridge || pCutPartSeq->GetCutSequenceType() == CutSequence_ChainCut || pCutPartSeq->GetCutSequenceType() == CutSequence_GridCut)
			{
				PartInstanceListPtr pPartInstances = pCutPartSeq->GetPartInstances(); // �����漰�����ʵ����
				m_pCurrentCutSequences->AppendCutSequences(m_pCutSequences->GetCutSequencesByPartInstances(pPartInstances)); // �õ�������Щ�����ʵ�����ϵĹ���
			}
			else
			{
				// ����������ڵ�����������Ƿ�������Ż��������
				LoopInstancePtr pOutLoopInst = LoopInstanceManager::GetOuterLoopInst(pCurrentLoopInstance->GetPartInstance());
				if (pOutLoopInst)
				{
					ICutPartSequencePtr pCutPartSeq1 = m_pCutSequences->GetCutSequenceByLoopInstance(pOutLoopInst.get());
					if (pCutPartSeq1 && (pCutPartSeq1->GetCutSequenceType() == CutSequence_Bridge || pCutPartSeq1->GetCutSequenceType() == CutSequence_ChainCut || pCutPartSeq->GetCutSequenceType() == CutSequence_GridCut))
					{
						PartInstanceListPtr pPartInstances = pCutPartSeq1->GetPartInstances(); // �����漰�����ʵ����
						m_pCurrentCutSequences->AppendCutSequences(m_pCutSequences->GetCutSequencesByPartInstances(pPartInstances)); // ������Щ����ϵĹ���
					}
					else
						m_pCurrentCutSequences->AppendCutSequences(m_pCutSequences->GetCutSequencesByPartInstance(pCurrentLoopInstance->GetPartInstance()));
				}
				else
					m_pCurrentCutSequences->AppendCutSequences(m_pCutSequences->GetCutSequencesByPartInstance(pCurrentLoopInstance->GetPartInstance()));
			}
		}
		else
			m_pCurrentCutSequences->push_back(pCutPartSeq);
	}

	return TRUE;
}

BOOL CutSequencePreSelectAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	return TRUE;
}

BOOL CutSequencePreSelectAction::DrawBeforeFinish()
{
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// ���Ƶ�ǰ�Ĺ���
	if (m_pCurrentCutSequences->size() > 0)
    {
		cutSequenceDrawer.DrawActiveCutSequences(m_pCurrentCutSequences.get(), m_pCutNodeList.get());
    }

	// ����ѡ�еĹ���
	cutSequenceDrawer.DrawSelectedCutSequences(m_pSelectCutSequences.get(), m_pCutNodeList.get());

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()
