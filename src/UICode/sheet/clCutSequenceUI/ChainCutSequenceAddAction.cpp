#include "StdAfx.h"
#include "ChainCutSequenceAddAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"
#include "Point2DList.h"

#include "IArrayPattern.h"
#include "IPatternLoop.h"
#include "PatternLoopNode.h"
#include "PatternLoopNodeList.h"
#include "LoopDrawer.h"
#include "LoopToolData.h"
#include "LoopCutFeatureDrawer.h"

#include "PartPlacementList.h"
#include "PartInstance.h"
#include "PartInstanceList.h"
#include "LoopInstanceList.h"
#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "PartPlacementManager.h"
#include "LoopInstanceManager.h"

#include "clCutSequenceResource.h"
#include "CutNodeList.h"
#include "PtChainNode.h"
#include "LoopChainNode.h"
#include "ChainNodeList.h"
#include "ChainCutSequence.h"
#include "CutSequenceList.h"
#include "ChainCutSequenceAddCommand.h"
#include "PtChainNodeAddCommand.h"
#include "LoopChainNodeAddCommand.h"
#include "CutSequenceDrawer.h"
#include "PartLayoutData.h"
#include "CutNodeManager.h"
#include "CutSequenceSelectAction.h"
#include "ChainCutSequenceManager.h"
#include "ConfigData.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "SheetEditParam.h"

BEGIN_CUTLEADER_NAMESPACE()

ChainCutSequenceAddAction::ChainCutSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, 
									 ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
									 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iState = NONE;
	m_iMsgID = IDS_SEQ_SEL_PT_LOOP_4_CHAIN;

	m_pCutPts.reset(new Point2DList);
	m_pChainSeq.reset();

	// �õ�������Χ����·ʵ������
	m_pOuterLoopInsts = LoopInstanceManager::GetOuterLoopInsts(m_pPartLayoutData->GetPartInstances());
}

ChainCutSequenceAddAction::~ChainCutSequenceAddAction(void)
{
}

BOOL ChainCutSequenceAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_iState == NONE)
	{
		if (m_pActiveLoopInstance) // ��ǰѡ����������
		{
            if (m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->GetIsToolDiabled()) // �������ʵ���ǡ����и�ġ�
            {
                strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LoopNoCutting);
                MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
                return TRUE;
            }

			// ����ܷ����������·ʵ�����ϼ����
			if (Check4CreateChain(m_pActiveLoopInstance))
			{
				// ����������򡱡�
				CreateChainSeq();

				m_iState = CHAIN_SEQ_CREATED;
			}
		}
		else
		{
			m_pCutPts->push_back(Point2D(dPtX, dPtY));
		}
	}
	else if (m_iState == CHAIN_SEQ_CREATED)
	{
		// �����С�����򡱺���ӽڵ㡣
		AddChainNode();
	}

	return TRUE;
}

BOOL ChainCutSequenceAddAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// ���µ�ǰ���㡣
	m_currentPt = pt;

	// �õ����������ľ��롣
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	SheetEditParamPtr pSheetEditParam = pCLOptionInfo->GetSheetEditParam();
	double dCapLoopDis = pSheetEditParam->GetLoopDetectDis();
	m_pViewPort->DToW(dCapLoopDis);

	// �����굱ǰ�Ƿ���ס����������
	LoopInstancePtr pLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pOuterLoopInsts, pt);
	if (pLoopInstance && pLoopInstance->DistanceTo(pt) < dCapLoopDis)
	{
		m_pActiveLoopInstance = pLoopInstance;

		// �Ƿ���ס�����������е㡣
		Point2D startCutPt = m_pActiveLoopInstance->GetStartCutPt();
		if (startCutPt.DistanceTo(pt) < dCapLoopDis)
			m_bOldStartPt = TRUE;
		else
		{
			m_bOldStartPt = FALSE;

			// �µ����е㡣
			Point2D pt(dPtX, dPtY);
			IPatternLoopPtr pPatternLoop = m_pActiveLoopInstance->GetCacheData()->GetPatternLoop();
			Matrix2D mat = m_pActiveLoopInstance->GetTransformMatrix();
			if (pPatternLoop->GetPatternLoopType() == PatternLoop_Array)
			{
				PatternLoopNodePtr pPatternLoopNode = pPatternLoop->GetPatternLoopNodes()->at(0);
				IArrayPatternPtr pArrayPattern = boost::dynamic_pointer_cast<IArrayPattern>(pPatternLoopNode->GetPattern());
				IPatternPtr pFirstPat = pArrayPattern->GetFirstInstance();
				m_newPt = pFirstPat->GetClosestPt(pt, mat);
				m_iNodeIndex = 0;
			}
			else
				pPatternLoop->GetClosestPt(pt, m_newPt, m_iNodeIndex, mat);
		}
	}
	else
		m_pActiveLoopInstance.reset();

	return TRUE;
}

BOOL ChainCutSequenceAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL ChainCutSequenceAddAction::DrawBeforeFinish()
{
    const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();
    LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	if (m_iState == NONE)
	{
		// ������ѡ�е�����ߡ�
		if (m_pCutPts->size() > 1)
		{
			for (unsigned int i = 0; i < m_pCutPts->size()-1; i++)
			{
				Point2D startPt = m_pCutPts->at(i);
				Point2D endPt = m_pCutPts->at(i+1);
				m_pViewPort->DrawLinePoint2D(startPt.X(), startPt.Y(), endPt.X(), endPt.Y());
			}
		}

		// ������Ƥ������е㡣
		if (m_pActiveLoopInstance)
		{
			if (m_bOldStartPt)
			{
				// ��Ƥ�
				if (m_pCutPts->size() > 0)
				{
					Point2D piercePt = LoopInstanceManager::GetPiercePt(m_pActiveLoopInstance);
					m_pViewPort->DrawLinePoint2D(m_pCutPts->back().X(), m_pCutPts->back().Y(), piercePt.X(), piercePt.Y());
				}

				// �������е㡣
				loopFeatureDrawer.DrawFeatureLoc(m_pActiveLoopInstance->GetStartCutPt(), RGB(255,0,0));
			}
			else
			{
				// ��Ƥ�
				if (m_pCutPts->size() > 0)
					m_pViewPort->DrawLinePoint2D(m_pCutPts->back().X(), m_pCutPts->back().Y(), m_newPt.X(), m_newPt.Y());

				// ���ƻ���
				loopFeatureDrawer.DrawPosRing(m_newPt);
			}
		}
		else
		{
			if (m_pCutPts->size() > 0)
				m_pViewPort->DrawLinePoint2D(m_pCutPts->back().X(), m_pCutPts->back().Y(), m_currentPt.X(), m_currentPt.Y());
		}
	}
	else if (m_iState == CHAIN_SEQ_CREATED)
	{
		// ���ơ�����򡱵Ľ��������Ƥ�
		Point2D lastCutPt = m_pChainSeq->GetLastCutPt();
		if (m_pActiveLoopInstance)
		{
			if (m_bOldStartPt)
			{
				// ��Ƥ�
				if (m_pCutPts->size() > 0)
				{
					Point2D piercePt = LoopInstanceManager::GetPiercePt(m_pActiveLoopInstance);
					m_pViewPort->DrawLinePoint2D(lastCutPt.X(), lastCutPt.Y(), piercePt.X(), piercePt.Y());
				}

				// �������е㡣
				loopFeatureDrawer.DrawFeatureLoc(m_pActiveLoopInstance->GetStartCutPt(), RGB(255,0,0));
			}
			else
			{
				// ��Ƥ�
				if (m_pCutPts->size() > 0)
					m_pViewPort->DrawLinePoint2D(lastCutPt.X(), lastCutPt.Y(), m_newPt.X(), m_newPt.Y());

				// ���ƻ���
				loopFeatureDrawer.DrawPosRing(m_newPt);
			}
		}
		else
		{
			if (m_pCutPts->size() > 0)
				m_pViewPort->DrawLinePoint2D(lastCutPt.X(), lastCutPt.Y(), m_currentPt.X(), m_currentPt.Y());
		}
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void ChainCutSequenceAddAction::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();

	if (m_pChainSeq)
	{
		// ������������Ĺ������û�ˣ��ص����������״̬��
		if (m_pCutSequences->GetCutSequenceIndex(m_pChainSeq.get()) == INVALID_IDX)
			m_iState = NONE;
		else
			m_iState = CHAIN_SEQ_CREATED;
	}
}

BOOL ChainCutSequenceAddAction::Check4CreateChain(LoopInstancePtr pLoopInstance)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	ICutPartSequencePtr pCutSeqUnit = m_pCutSequences->GetCutSequenceByLoopInstance(pLoopInstance.get());
	if (pCutSeqUnit)
	{
		if (pCutSeqUnit->GetCutSequenceType() == CutSequence_CommonCut || pCutSeqUnit->GetCutSequenceType() == CutSequence_Bridge || pCutSeqUnit->GetCutSequenceType() == CutSequence_ChainCut)
		{
			if (pCutSeqUnit->GetCutSequenceType() == CutSequence_CommonCut)
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_CHAIN_1);
			else if (pCutSeqUnit->GetCutSequenceType() == CutSequence_Bridge)
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_CHAIN_2);
			else if (pCutSeqUnit->GetCutSequenceType() == CutSequence_ChainCut)
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_CHAIN_3);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
			return FALSE;
		}
	}

	return TRUE;
}

void ChainCutSequenceAddAction::CreateChainSeq()
{
	// ���ӡ�����򡱡�
	m_pChainSeq = ChainCutSequenceManager::BuildChainSeq(m_pCutSequences, m_pCutPts, m_pActiveLoopInstance);
	Point2D newStartPt = m_newPt.InverseTransform(m_pActiveLoopInstance->GetTransformMatrix());
	LoopPtPtr pLoopPt;
	if (!m_bOldStartPt)
		pLoopPt.reset(new LoopPt(newStartPt, m_iNodeIndex));
	ICommandPtr pCommand(new ChainCutSequenceAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartPlacements(), m_pPartLayoutData->GetPartInstances(), m_pOuterLoopInsts, m_pChainSeq, pLoopPt));
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

void ChainCutSequenceAddAction::AddChainNode()
{
    CString strProductName, strMsg;
    strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// ���ѡ�������������ܲ�����Ϊ����ڵ㡣
	if (m_pActiveLoopInstance)
	{
        if (m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->GetIsToolDiabled()) // �������ʵ���ǡ����и�ġ�
        {
            strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LoopNoCutting);
            MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            return;
        }

		ICutPartSequencePtr pCutSeqUnit = m_pCutSequences->GetCutSequenceByLoopInstance(m_pActiveLoopInstance.get());
		if (pCutSeqUnit)
		{
			if (pCutSeqUnit->GetCutSequenceType() == CutSequence_CommonCut || pCutSeqUnit->GetCutSequenceType() == CutSequence_Bridge || pCutSeqUnit->GetCutSequenceType() == CutSequence_ChainCut)
			{
				if (pCutSeqUnit->GetCutSequenceType() == CutSequence_CommonCut)
					strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_CHAIN_1);
				else if (pCutSeqUnit->GetCutSequenceType() == CutSequence_Bridge)
					strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_CHAIN_2);
				else if (pCutSeqUnit->GetCutSequenceType() == CutSequence_ChainCut)
					strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_CHAIN_3);
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
				return;
			}
		}
	}

    // ��������ĵ��߳ߴ�
	if (m_pActiveLoopInstance)
    {
        LoopInstancePtr pFstLoopInst_of_chainSeq = m_pChainSeq->GetFstLoopInst();
        if (!m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->IsSameToolSize(pFstLoopInst_of_chainSeq->GetCacheData()->GetLoopTool()))
        {
            strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_CHAIN_LOOPTOOL_WARN);
            MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            return;
        }
    }

	// ����������ڵ㡱��
	ICommandPtr pCommand;
	IChainNodePtr pLastNode = m_pChainSeq->GetChainNodes()->back();
	if (m_pActiveLoopInstance)
	{
		LoopChainNodePtr pChainLoopNode(new LoopChainNode(m_pActiveLoopInstance));
		Point2D newStartPt = m_newPt.InverseTransform(m_pActiveLoopInstance->GetTransformMatrix());
		LoopPtPtr pLoopPt;
		if (!m_bOldStartPt)
			pLoopPt.reset(new LoopPt(newStartPt, m_iNodeIndex));
		pCommand.reset(new LoopChainNodeAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartPlacements(), m_pPartLayoutData->GetPartInstances(), m_pOuterLoopInsts, m_pChainSeq, pChainLoopNode, INVALID_ID, pLoopPt));
	}
	else
	{
		PtChainNodePtr pChainPtNode(new PtChainNode(m_currentPt));
		pCommand.reset(new PtChainNodeAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pChainSeq, pChainPtNode, INVALID_ID));
	}
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
