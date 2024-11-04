#include "StdAfx.h"
#include "BridgeSequenceAddAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"

#include "IPatternLoop.h"
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
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "BridgeNodeAddCommand.h"
#include "BridgeSequenceAddCommand.h"
#include "BridgeNodeInsertCommand.h"
#include "BridgeSequenceLinkCommand.h"
#include "PartLayoutData.h"
#include "CutNodeManager.h"
#include "BridgeSequence.h"
#include "BridgeSequenceManager.h"
#include "LoopSequenceManager.h"
#include "CutSequenceSelectAction.h"
#include "ConfigData.h"

#include "BridgeCutParam.h"
#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

BridgeSequenceAddAction::BridgeSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
								   : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iState = NONE;
	m_iMsgID = IDS_SEQ_SEL_BDGE_PT;

	// �õ�������Χ����·ʵ������
	m_pOuterLoopInsts = LoopInstanceManager::GetOuterLoopInsts(m_pPartLayoutData->GetPartInstances());

	// �������ʵ���и�����·ʵ������Ӿ��Ρ�
	m_partRectMap = m_pPartLayoutData->GetPartInstances()->GetLoopInstanceRect();

	m_iFstLoopNodeIdx = INVALID_IDX;
	m_iSndLoopNodeIdx = INVALID_IDX;
}

BridgeSequenceAddAction::~BridgeSequenceAddAction(void)
{
}

BOOL BridgeSequenceAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_iState == NONE || m_iState == SECOND_BDGE_PT_SELECTED)
	{
		if (!m_pFstLoopInst)
        {
			return TRUE;
        }

        if (m_pFstLoopInst->GetCacheData()->GetLoopTool()->GetIsToolDiabled()) // �������ʵ���ǡ����и�ġ�
        {
            strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LoopNoCutting);
            MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            return TRUE;
        }

		// ����ܷ����������·ʵ�����ϼ��Žӡ�
		if (LoopCanAddBdge(m_pFstLoopInst))
		{
			m_iState = FIRST_BDGE_PT_SELECTED;
			m_pSndLoopInst.reset();
		}
	}
	else if (m_iState == FIRST_BDGE_PT_SELECTED)
	{
		if (!m_pSndLoopInst)
        {
			return TRUE;
        }

        if (m_pSndLoopInst->GetCacheData()->GetLoopTool()->GetIsToolDiabled()) // �������ʵ���ǡ����и�ġ�
        {
            strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LoopNoCutting);
            MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            return TRUE;
        }

		// ������ͬһ��������
		if ((m_pFstLoopInst->GetPartInstance()->GetID() == m_pSndLoopInst->GetPartInstance()->GetID()) && (m_pFstLoopInst->GetPatternLoopID() == m_pSndLoopInst->GetPatternLoopID()))
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_BDGE_SAME_LOOP);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return TRUE;
		}

        // ��������������ĵ��߳ߴ�
        if (!m_pFstLoopInst->GetCacheData()->GetLoopTool()->IsSameToolSize(m_pSndLoopInst->GetCacheData()->GetLoopTool()))
        {
            strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_BDGE_LOOPTOOL_WARN);
            MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            return TRUE;
        }

		// ����ܷ����������·ʵ�����ϼ��Žӡ�
		if (!LoopCanAddBdge(m_pSndLoopInst))
			return TRUE;

		// �����2����·���ܲ����Žӡ�
		if (!BridgeSequenceManager::TwoLoopCanBdge(m_pCutSequences, m_pFstLoopInst.get(), m_pSndLoopInst.get()))
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOTALLOW_BDGE);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
		else
		{
			// �����2����֮���ܲ����Žӡ�
			BridgeCutParamPtr pBdgeCutParam = m_pConfigData->GetSeqConfigItem()->GetBdgeCutParam();
			double dWidth = pBdgeCutParam->GetBdgeWidth();
			if (!BridgeSequenceManager::TwoLoopCanBdge(m_pFstLoopInst, m_iFstLoopNodeIdx, m_fstBdgePt, m_pSndLoopInst, m_iSndLoopNodeIdx, m_sndBdgePt, dWidth))
			{
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_PT_NOTALLOW_BDGE);
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			}
			else
			{
				// ��ѡ�е�2������·ʵ�����������Žӡ�
				AddBdgeCut();

				// �������ݡ�
				m_iState = NONE;
				m_pFstLoopInst.reset();
			}
		}
	}

	return TRUE;
}

BOOL BridgeSequenceAddAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_iState == NONE || m_iState == SECOND_BDGE_PT_SELECTED)
	{
		m_pFstLoopInst = LoopInstanceManager::GetClosestLoopInst(m_pOuterLoopInsts, pt);
		if (m_pFstLoopInst)
		{
			IPatternLoopPtr pPatternLoop = m_pFstLoopInst->GetCacheData()->GetPatternLoop();
			pPatternLoop->GetClosestPt(pt, m_fstBdgePt, m_iFstLoopNodeIdx, m_pFstLoopInst->GetTransformMatrix());
		}
	}
	else if (m_iState == FIRST_BDGE_PT_SELECTED)
	{
		m_pSndLoopInst = LoopInstanceManager::GetClosestLoopInst(m_pOuterLoopInsts, pt);
		if (m_pSndLoopInst)
		{
			IPatternLoopPtr pPatternLoop = m_pSndLoopInst->GetCacheData()->GetPatternLoop();
			pPatternLoop->GetClosestPt(pt, m_sndBdgePt, m_iSndLoopNodeIdx, m_pSndLoopInst->GetTransformMatrix());
		}
	}

	return TRUE;
}

BOOL BridgeSequenceAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL BridgeSequenceAddAction::DrawBeforeFinish()
{
    const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();
    LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	m_pViewPort->SetDrawColor(*pDisplayParam->GetSelectObjColor());
	if (m_iState == NONE || m_iState == SECOND_BDGE_PT_SELECTED)
	{
		// ���Ƶ�һ����������ġ���Ȧ����
		if (m_pFstLoopInst)
			loopFeatureDrawer.DrawPosRing(m_fstBdgePt);
	}
	else if (m_iState == FIRST_BDGE_PT_SELECTED)
	{
		// ����ǰһ���Žӵ㡣
		loopFeatureDrawer.DrawFeatureLoc(m_fstBdgePt, RGB(255,0,0));

		// ���Ƶڶ�����������ġ���Ȧ����
		if (m_pSndLoopInst)
			loopFeatureDrawer.DrawPosRing(m_sndBdgePt);
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void BridgeSequenceAddAction::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

BOOL BridgeSequenceAddAction::LoopCanAddBdge(LoopInstancePtr pLoopInstance)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	ICutPartSequencePtr pCutSeqUnit = m_pCutSequences->GetCutSequenceByLoopInstance(pLoopInstance.get());
	if (pCutSeqUnit)
	{
		if (pCutSeqUnit->GetCutSequenceType() == CutSequence_CommonCut || pCutSeqUnit->GetCutSequenceType() == CutSequence_ChainCut)
		{
			if (pCutSeqUnit->GetCutSequenceType() == CutSequence_CommonCut)
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_BDGE_1);
			else if (pCutSeqUnit->GetCutSequenceType() == CutSequence_ChainCut)
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_NOT_BDGE_2);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
			return FALSE;
		}
	}

	return TRUE;
}

void BridgeSequenceAddAction::AddBdgeCut()
{
	const IMaterialSize* pMaterialSize = m_pConfigData->GetMatSize();
	const SequenceConfigItem* pSeqConfigItem = m_pConfigData->GetSeqConfigItem();

	// �ж�ѡ�е�2������·ʵ��������û�С��Žӹ��򡱡�
	BOOL bFstHasBdgeSeq = FALSE, bSndHasBdgeSeq = FALSE;
	ICutPartSequencePtr pFstSeqUnit = m_pCutSequences->GetCutSequenceByLoopInstance(m_pFstLoopInst.get());
	if (pFstSeqUnit && pFstSeqUnit->GetCutSequenceType() == CutSequence_Bridge)
		bFstHasBdgeSeq = TRUE;
	ICutPartSequencePtr pSndSeqUnit = m_pCutSequences->GetCutSequenceByLoopInstance(m_pSndLoopInst.get());
	if (pSndSeqUnit && pSndSeqUnit->GetCutSequenceType() == CutSequence_Bridge)
		bSndHasBdgeSeq = TRUE;

	ICommandPtr pCommand;
	if (!bFstHasBdgeSeq && !bSndHasBdgeSeq) // ��2������·ʵ�����϶�û�С��Žӹ��򡱡�
	{
		// ���ӡ��Žӹ��򡱡�
		BridgePtPtr pParentBridgePt = BridgeSequenceManager::BuildBdgePt(m_pFstLoopInst, m_iFstLoopNodeIdx, m_fstBdgePt);
		BridgePtPtr pChildBdgePt = BridgeSequenceManager::BuildBdgePt(m_pSndLoopInst, m_iSndLoopNodeIdx, m_sndBdgePt);
		pCommand.reset(new BridgeSequenceAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartInstances(), m_pFstLoopInst, pParentBridgePt, m_pSndLoopInst, pChildBdgePt));
	}
	else if (bFstHasBdgeSeq && !bSndHasBdgeSeq) // ֻ�е�һ������·ʵ�������С��Žӹ��򡱡�
	{
		BridgeSequencePtr pBridgeSequence = boost::dynamic_pointer_cast<BridgeSequence>(pFstSeqUnit);

		// �����С��Žӽڵ㡱���������Žӽڵ㡱�����
		BridgeNodePtr pParentBdgeNode = pBridgeSequence->GetBdgeNodeByLoopInst(m_pFstLoopInst);
		BridgePtPtr pParentBridgePt = BridgeSequenceManager::BuildBdgePt(m_pFstLoopInst, m_iFstLoopNodeIdx, m_fstBdgePt);
		BridgePtPtr pChildBdgePt = BridgeSequenceManager::BuildBdgePt(m_pSndLoopInst, m_iSndLoopNodeIdx, m_sndBdgePt);
		pCommand.reset(new BridgeNodeAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartInstances(), pBridgeSequence, pParentBdgeNode, pParentBridgePt, m_pSndLoopInst, pChildBdgePt));
	}
	else if (!bFstHasBdgeSeq && bSndHasBdgeSeq) // ֻ�еڶ�������·ʵ�������С��Žӹ��򡱡�
	{
		BridgeSequencePtr pBridgeSequence = boost::dynamic_pointer_cast<BridgeSequence>(pSndSeqUnit);

		// �����С��Žӽڵ㡱ǰ���롰�Žӽڵ㡱�����
		BridgeNodePtr pChildBdgeNode = pBridgeSequence->GetBdgeNodeByLoopInst(m_pSndLoopInst);
		BridgePtPtr pChildBridgePt = BridgeSequenceManager::BuildBdgePt(m_pSndLoopInst, m_iSndLoopNodeIdx, m_sndBdgePt);
		BridgePtPtr pParentBdgePt = BridgeSequenceManager::BuildBdgePt(m_pFstLoopInst, m_iFstLoopNodeIdx, m_fstBdgePt);
		pCommand.reset(new BridgeNodeInsertCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartInstances(), pBridgeSequence, pChildBdgeNode, pChildBridgePt, m_pFstLoopInst, pParentBdgePt));
	}
	else if (bFstHasBdgeSeq && bSndHasBdgeSeq) // 2������·ʵ�������С��Žӹ��򡱡�
	{
		BridgeSequencePtr pFstBdgeSeqUnit = boost::dynamic_pointer_cast<BridgeSequence>(pFstSeqUnit);
		BridgeSequencePtr pSndBdgeSeqUnit = boost::dynamic_pointer_cast<BridgeSequence>(pSndSeqUnit);

		// ����2�����Žӹ��򡱡�
		BridgeNodePtr pFstBdgeNode = pFstBdgeSeqUnit->GetBdgeNodeByLoopInst(m_pFstLoopInst);
		BridgePtPtr pFstBdgePt = BridgeSequenceManager::BuildBdgePt(m_pFstLoopInst, m_iFstLoopNodeIdx, m_fstBdgePt);
		BridgeNodePtr pSndBdgeNode = pSndBdgeSeqUnit->GetBdgeNodeByLoopInst(m_pSndLoopInst);
		BridgePtPtr pSndBdgePt = BridgeSequenceManager::BuildBdgePt(m_pSndLoopInst, m_iSndLoopNodeIdx, m_sndBdgePt);
		pCommand.reset(new BridgeSequenceLinkCommand(GetEditorData(), m_pCutSequences, m_pConfigData, pFstBdgeSeqUnit, pFstBdgeNode, pFstBdgePt, pSndBdgeSeqUnit, pSndBdgeNode, pSndBdgePt));
	}
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
