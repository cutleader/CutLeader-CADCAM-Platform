#include "StdAfx.h"
#include "ChainNodeInsertAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"
#include "Point2DList.h"

#include "LoopPt.h"
#include "IArrayPattern.h"
#include "IPatternLoop.h"
#include "PatternLoopNode.h"
#include "PatternLoopNodeList.h"
#include "LoopToolData.h"
#include "LoopDrawer.h"
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
#include "PtChainNodeAddCommand.h"
#include "LoopChainNodeAddCommand.h"
#include "CutSequenceDrawer.h"
#include "PartLayoutData.h"
#include "CutNodeManager.h"
#include "LoopSequenceManager.h"
#include "ChainCutSequenceManager.h"
#include "CutSequenceSelectAction.h"
#include "ConfigData.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "SheetEditParam.h"

BEGIN_CUTLEADER_NAMESPACE()

ChainNodeInsertAction::ChainNodeInsertAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, 
											 ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, InsertPositionDlg* pInsertPosDlg)
											 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_pInsertPosDlg = pInsertPosDlg;

	m_iState = NONE;
	m_iMsgID = IDS_SEQ_CHAIN_SEL_BASE_NODE;
	m_pBaseChainNode.reset();

	// �õ�������Χ����·ʵ������
	m_pOuterLoopInsts = LoopInstanceManager::GetOuterLoopInsts(m_pPartLayoutData->GetPartInstances());
}

ChainNodeInsertAction::~ChainNodeInsertAction(void)
{
}

BOOL ChainNodeInsertAction::LButtonDown(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_iState == NONE)
	{
		if (m_pBaseChainNode)
		{
			m_iState = CHAIN_NODE_SELECTED;
		}
		else
		{
			CString strProductName, strMsg;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_CHAIN_NO_BASE_NODE_SEL);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
		}
	}
	else if (m_iState == CHAIN_NODE_SELECTED)
	{
		InsertChainNode();
	}

	return TRUE;
}

BOOL ChainNodeInsertAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// ���µ�ǰ���㡣
	m_currentPt = pt;

	if (m_iState == NONE)
	{
		m_iMsgID = IDS_SEQ_CHAIN_SEL_BASE_NODE;

		// �õ����������ڵ㡣
		ChainNodeListPtr pChainNodes = ChainCutSequenceManager::GetAllChainNode(m_pCutSequences);
		m_pBaseChainNode = pChainNodes->GetClosestNode(pt);
	}
	else if (m_iState == CHAIN_NODE_SELECTED)
	{
		m_iMsgID = IDS_SEQ_SEL_PT_LOOP_4_CHAIN;

		// �õ����������ľ��롣
		CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
		SheetEditParamPtr pSheetEditParam = pCLOptionInfo->GetSheetEditParam();
		double dCapLoopDis = pSheetEditParam->GetLoopDetectDis();
		m_pViewPort->DToW(dCapLoopDis);

		// �����굱ǰ�Ƿ���ס����������
		LoopInstancePtr pLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pOuterLoopInsts, pt);
		if (pLoopInstance->DistanceTo(pt) < dCapLoopDis)
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
	}

	return TRUE;
}

BOOL ChainNodeInsertAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL ChainNodeInsertAction::DrawBeforeFinish()
{
	const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();
	LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);
	CutSequenceDrawer cutSeqDrawer(m_pViewPort);

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	if (m_iState == NONE)
	{
		// ���Ƶ�ǰ�ġ�����ڵ㡱��
		if (m_pBaseChainNode)
		{
			cutSeqDrawer.DrawActiveChainNode(m_pBaseChainNode.get());
		}

		// draw cursor.
		m_pViewPort->SetDrawColor(*pDisplayParam->GetPatColor());
		CursorDrawer::DrawAimSelCursor(m_pViewPort);
	}
	else if (m_iState == CHAIN_NODE_SELECTED)
	{
		// ���ƻ�׼�ڵ㡣
		cutSeqDrawer.DrawSelectedChainNode(m_pBaseChainNode.get());

		// ����������ʱ���������е㡣
		if (m_pActiveLoopInstance)
		{
			if (m_bOldStartPt)
				loopFeatureDrawer.DrawFeatureLoc(m_pActiveLoopInstance->GetStartCutPt(), RGB(255,0,0));
			else
				loopFeatureDrawer.DrawPosRing(m_newPt);
		}

		// draw cursor.
		m_pViewPort->SetDrawColor(*pDisplayParam->GetPatColor());
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	return TRUE;
}

void ChainNodeInsertAction::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();

	// ����״̬��
	m_iState = NONE;
	m_pBaseChainNode.reset();
}

void ChainNodeInsertAction::InsertChainNode()
{
    CString strProductName, strMsg;
    strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

    if (m_pActiveLoopInstance && m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->GetIsToolDiabled()) // �������ʵ���ǡ����и�ġ�
    {
        strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LoopNoCutting);
        MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
        return;
    }

	// ���ѡ�������������ܲ�����Ϊ����ڵ㡣
	if (m_pActiveLoopInstance)
	{
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
        ChainCutSequencePtr pChainSeq = ChainCutSequenceManager::SearchChainSeq(m_pCutSequences, m_pBaseChainNode);
        LoopInstancePtr pFstLoopInst_of_chainSeq = pChainSeq->GetFstLoopInst();
        if (!m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->IsSameToolSize(pFstLoopInst_of_chainSeq->GetCacheData()->GetLoopTool()))
        {
            strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_CHAIN_LOOPTOOL_WARN);
            MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            return;
        }
    }

	/************************************************************************/
	// ��������ڵ㡣

	// ��ǰ�������
	ChainCutSequencePtr pChainSeq = ChainCutSequenceManager::SearchChainSeq(m_pCutSequences, m_pBaseChainNode);

	// ȷ�������ĸ��ڵ�ǰ�档
	LONGLONG iNextNodeID;
	if (!m_pInsertPosDlg->IsInsertBehind())
		iNextNodeID = m_pBaseChainNode->GetID();
	else
		iNextNodeID = pChainSeq->GetChainNodes()->GetNextNodeID(m_pBaseChainNode);

	// ����������ڵ㡱��
	ICommandPtr pCommand;
	if (m_pActiveLoopInstance)
	{
		LoopChainNodePtr pChainLoopNode(new LoopChainNode(m_pActiveLoopInstance));
		Point2D newStartPt = m_newPt.InverseTransform(m_pActiveLoopInstance->GetTransformMatrix());
		LoopPtPtr pLoopPt;
		if (!m_bOldStartPt)
			pLoopPt.reset(new LoopPt(newStartPt, m_iNodeIndex));
		pCommand.reset(new LoopChainNodeAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartPlacements(), m_pPartLayoutData->GetPartInstances(), m_pOuterLoopInsts, pChainSeq, pChainLoopNode, iNextNodeID, pLoopPt));

		// ���»�׼�ڵ㡣
		m_pBaseChainNode = pChainLoopNode;
	}
	else
	{
		PtChainNodePtr pChainPtNode(new PtChainNode(m_currentPt));
		pCommand.reset(new PtChainNodeAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, pChainSeq, pChainPtNode, iNextNodeID));

		// ���»�׼�ڵ㡣
		m_pBaseChainNode = pChainPtNode;
	}
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);
	/************************************************************************/

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
