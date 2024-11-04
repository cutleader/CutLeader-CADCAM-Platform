#include "StdAfx.h"
#include "LoopSequenceAddAction.h"

#include "GlViewPort.h"
#include "Line2D.h"
#include "LineArc2DList.h"
#include "CommandManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "ValueInputManager.h"
#include "clBaseDataResource.h"

#include "IPatternLoop.h"
#include "PatternLoopCache.h"
#include "LoopDrawer.h"
#include "LoopToolData.h"
#include "PartPlacementList.h"
#include "PartInstance.h"
#include "PartInstanceList.h"
#include "LoopInstanceList.h"
#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "LoopInstanceManager.h"
#include "PartPlacementDrawer.h"
#include "PartPlacementManager.h"

#include "clCutSequenceResource.h"
#include "CutNode.h"
#include "CutNodeList.h"
#include "LoopSequence.h"
#include "LoopSequenceManager.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "LoopSequenceAddCommand.h"
#include "PartLayoutData.h"
#include "CutNodeManager.h"
#include "CutSequenceSelectAction.h"
#include "ConfigData.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

LoopSequenceAddAction::LoopSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg)
								   : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_pApplyPartDlg = pApplyPartDlg;
	m_iMsgID = IDS_SEQ_ADD_LOOPSEQ;

	// �õ����С���·ʵ������
	m_pLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(m_pPartLayoutData->GetPartInstances());

	// �������ʵ���и�����·ʵ������Ӿ��Ρ�
	m_partRectMap = m_pPartLayoutData->GetPartInstances()->GetLoopInstanceRect();
}

LoopSequenceAddAction::~LoopSequenceAddAction(void)
{
}

BOOL LoopSequenceAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_pApplyPartDlg->IsApplyPart())
	{
		if (m_pActivePartInst)
		{
            if (PartPlacementManager::AllLoopToolsDisabled(m_pActivePartInst.get())) // ������ʵ���ǡ����и�ġ�
            {
                strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_PartNoCutting);
                MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            }
            else
            {
                // ����������ǲ����Ѿ������˹���
                BOOL bPartSequenced = m_pCutSequences->IsPartInstanceSequenced(m_pActivePartInst);
                if (bPartSequenced)
                {
                    strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_PART_SEQUENCED);
                    MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
                }
                else
                {
                    // �����ʵ�����乤��
                    AssignPartSeq(m_pActivePartInst);
                }
            }
		}
		else
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_SELECT_PARTINST);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
	}
	else
	{
		if (m_pActiveLoopInstance)
		{
            if (m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->GetIsToolDiabled()) // �������ʵ���ǡ����и�ġ�
            {
                strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LoopNoCutting);
                MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
            }
            else
            {
                // check whether the active loop is sequenced.
                BOOL bLoopSequenced = m_pCutSequences->LoopInstSeqed(m_pActiveLoopInstance.get());
                if (bLoopSequenced)
                {
                    strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_LOOP_SEQUENCED);
                    MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
                }
                else
                {
                    // ���ӹ���
                    CutSequenceListPtr pNewCutSeqs(new CutSequenceList);
                    LoopSequencePtr pLoopSequence(new LoopSequence(m_pActiveLoopInstance));
                    pNewCutSeqs->push_back(pLoopSequence);
                    ICommandPtr pCommand(new LoopSequenceAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, pNewCutSeqs));
                    pCommand->Do();
                    CommandManager::GetInstance()->AddCommand(pCommand);

                    // ���¡��и�ڵ㡱���档
                    RefreshCutNodes();
                }
            }
		}
		else
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_SELECT_LOOPINST);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
	}

	return TRUE;
}

BOOL LoopSequenceAddAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_pApplyPartDlg->IsApplyPart())
		m_pActivePartInst = m_pPartLayoutData->GetPartInstances()->GetClosestPartInstance(pt, m_partRectMap);
	else
		m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInstEx(m_pLoopInstanceList, pt);

	return TRUE;
}

BOOL LoopSequenceAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL LoopSequenceAddAction::DrawBeforeFinish()
{
	// prepare some data.
	const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();

	// ���Ƶ�ǰ�ġ���·ʵ��/���ʵ������
	if (m_pApplyPartDlg->IsApplyPart())
	{
		if (m_pActivePartInst)
		{
			PartPlacementDrawer partPlacementDrawer(m_pViewPort);
			partPlacementDrawer.DrawActivePartInstance(m_pActivePartInst.get());
		}
	}
	else
	{
		if (m_pActiveLoopInstance)
		{
			LoopDrawer loopDrawer(m_pViewPort);
            COLORREF iColor;
            if (m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->GetIsToolDiabled())
            {
                iColor = *pDisplayParam->GetNoCutLoopColor();
            }
            else
            {
                iColor = m_pActiveLoopInstance->GetCacheData()->GetLoopTool()->GetToolColor();
            }
			loopDrawer.DrawActiveLoop(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), iColor, m_pActiveLoopInstance->GetTransformMatrix());
		}
	}

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void LoopSequenceAddAction::UpdateForUndoRedo()
{
    m_pLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(m_pPartLayoutData->GetPartInstances());
    m_partRectMap = m_pPartLayoutData->GetPartInstances()->GetLoopInstanceRect();

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

void LoopSequenceAddAction::AssignPartSeq(PartInstancePtr pPartInstance)
{
	/************************************************************************/
	// �����ʵ�����乤��

	const IMaterialSize* pMaterialSize = m_pConfigData->GetMatSize();
	const SequenceConfigItem* pSeqConfigItem = m_pConfigData->GetSeqConfigItem();

	// ��ǰ���и�λ�á�
	Point2D lastCutPt;
	if (m_pCutSequences->size() == 0)
		lastCutPt = pMaterialSize->GetOuterRect().GetCornerPt(pSeqConfigItem->GetStartCorner());
	else
		lastCutPt = m_pCutSequences->at(m_pCutSequences->size()-1)->GetLastCutPt();

	// ǰһ���ӹ��Ļ�·ʵ����
	LoopInstancePtr pPreLoopInst;
	LoopInstanceListPtr pSeqLoopInsts = m_pCutSequences->GetSequencedLoopInstances();
	if (pSeqLoopInsts->size() > 0)
		pPreLoopInst = pSeqLoopInsts->at(pSeqLoopInsts->size()-1);

	// ���ʵ������Ӿ��Ρ�
	Rect2D partInstRect = m_pActivePartInst->GetLoopInstanceRect().first;

	// ������ʵ����δ���乤��ġ���·ʵ������
	LoopInstanceListPtr pAllLoopInst = LoopInstanceManager::GetLoopInstanceList(m_pActivePartInst);
	LoopInstanceListPtr pUnseqLoopInsts = m_pCutSequences->GetUnSequencedLoopInstances(pAllLoopInst);

	// ��ǰ�����ʵ���������˲㡣
	map<LONGLONG, PartInstanceTopologyItemPtr> partTopMap;
	PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances();
	PartInstanceTopologyItemListPtr pAllPartTopItems = PartPlacementManager::CalcPartTopStruct(pPartInstances, m_partRectMap, partTopMap);
	PartInstanceTopologyItemPtr pPartTopItem = partTopMap[m_pActivePartInst->GetID()];
	PartInstanceTopologyItemListPtr pPartTopItems = PartPlacementManager::GetPartTopLayer(pAllPartTopItems, m_pActivePartInst->GetID());

	// �õ����С����ʵ��������Χ��·�Ļ���ͼ�Ρ�
	map<LONGLONG, LineArc2DListPtr> outerLoopGeom = PartPlacementManager::CalcOutLoopGeom(pPartInstances);

	// ���乤��
	CutSequenceListPtr pLoopSequenceList = LoopSequenceManager::GenLoopSeq(lastCutPt, pPreLoopInst, partInstRect, pPartTopItems.get(), pPartTopItem.get(), pUnseqLoopInsts, pSeqLoopInsts, outerLoopGeom, m_pConfigData.get());
	/************************************************************************/

	// ���ӹ���
	ICommandPtr pCommand(new LoopSequenceAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, pLoopSequenceList));
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
