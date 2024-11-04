#include "StdAfx.h"
#include "CutSequenceOrderChangeActionB.h"

#include "boost/bind.hpp"
#include "baseConst.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"

#include "LoopInstanceList.h"
#include "LoopInstance.h"
#include "LoopInstanceManager.h"

#include "PartLayoutData.h"
#include "ICutPartSequence.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "clCutSequenceResource.h"
#include "CutNodeManager.h"
#include "SequenceLocationMoveCommand.h"
#include "CutSequenceSelectAction.h"
#include "ConfigData.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

CutSequenceOrderChangeActionB::CutSequenceOrderChangeActionB(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, 
									   ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg)
									   : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_pApplyPartDlg = pApplyPartDlg;

	m_iMsgID = IDS_SEQ_SELECT_SEQ_3;

	m_pCurrentCutSequences.reset(new CutSequenceList);
	m_pBaseCutSequence.reset();

	// ������������˹���ġ���·ʵ������
	m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();
}

CutSequenceOrderChangeActionB::~CutSequenceOrderChangeActionB(void)
{
}

BOOL CutSequenceOrderChangeActionB::LButtonDown(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
	if (m_pCurrentCutSequences->size() > 0)
	{
		// �Ȱѹ�����˳����һ����
		sort(m_pCurrentCutSequences->begin(), m_pCurrentCutSequences->end(), boost::bind(&IsSeqBefore, _1, _2, m_pCutSequences));

		if (!m_pBaseCutSequence)
		{
			m_pBaseCutSequence = m_pCurrentCutSequences->at(m_pCurrentCutSequences->size()-1);
		}
		else
		{
			// Ҫ�ƶ��Ĺ����ܰ�������׼���򡱡�
			if (m_pCurrentCutSequences->GetCutSequenceIndex(m_pBaseCutSequence.get()) != INVALID_IDX)
				return FALSE;

			// ִ�����
			ICommandPtr pCommand(new SequenceLocationMoveCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pCurrentCutSequences, m_pBaseCutSequence, 
												  m_pPartLayoutData->GetPartInstances()));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// ���¡��и�ڵ㡱���档
			RefreshCutNodes();

			m_pBaseCutSequence = m_pCurrentCutSequences->at(m_pCurrentCutSequences->size()-1);
			m_pCurrentCutSequences->clear();
			m_iMsgID = IDS_SEQ_SELECT_SEQ_2;
		}
	}
	else
	{
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_NO_SEQ_SELECT);
		MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
	}

	return TRUE;
}

BOOL CutSequenceOrderChangeActionB::MovePoint(double& dPtX, double& dPtY)
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
				// ����������ڵ�����������Ƿ�������Ž�/�����
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

BOOL CutSequenceOrderChangeActionB::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL CutSequenceOrderChangeActionB::DrawBeforeFinish()
{
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// ���ơ����һ�������Ĺ��򡱡�
	if (m_pBaseCutSequence)
    {
		cutSequenceDrawer.DrawSelectedCutSequence(m_pBaseCutSequence.get(), m_pCutNodeList.get());
    }

	// ���Ƶ�ǰ�Ĺ���
	if (m_pCurrentCutSequences->size() > 0)
    {
		cutSequenceDrawer.DrawActiveCutSequences(m_pCurrentCutSequences.get(), m_pCutNodeList.get());
    }

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void CutSequenceOrderChangeActionB::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
