#include "StdAfx.h"
#include "ResetInstructionDeleteAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "Point2DList.h"
#include "Line2D.h"
#include "LineArc2DList.h"
#include "ValueInputManager.h"
#include "clBaseDataResource.h"
#include "ColorSelector.h"

#include "IMaterialSize.h"
#include "SequenceConfigItem.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "LoopCutFeatureDrawer.h"
#include "PartLayoutData.h"

#include "ICutPartSequence.h"
#include "CutSequenceList.h"
#include "CutSequenceSelectAction.h"
#include "CutNodeManager.h"
#include "clCutSequenceResource.h"
#include "TraceDataMgr.h"
#include "CutSequenceDrawer.h"
#include "CutNodeList.h"
#include "CutLineNode.h"
#include "CutArcNode.h"
#include "ConfigData.h"
#include "ChainCutSequence.h"
#include "ChainCutSequenceManager.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

ResetInstructionDeleteAction::ResetInstructionDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
								 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
    // ���е������
    m_pAllChainSeqs.reset(new CutSequenceList);
    for (unsigned int i = 0; i < m_pCutSequences->size(); i++)
    {
        ICutSequencePtr pCutSequence = m_pCutSequences->at(i);
        if (pCutSequence->GetCutSequenceType() == CutSequence_ChainCut)
        {
            m_pAllChainSeqs->push_back(pCutSequence);
        }
    }

    // �����������и�·���͸�λ�㡣
    Update_cutPath_and_resetPt();
}

ResetInstructionDeleteAction::~ResetInstructionDeleteAction(void)
{
}

BOOL ResetInstructionDeleteAction::LButtonDown(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (!m_pClosestResetPt_onCutPath)
	{
		return FALSE;
	}

    // ��λ�����ڵĹ���
    ChainCutSequencePtr pChainSeq_ret;
    for (unsigned int i = 0; i < m_pAllChainSeqs->size(); i++)
    {
        ICutSequencePtr pCutSequence = m_pAllChainSeqs->at(i);
        ChainCutSequencePtr pChainSeq = boost::dynamic_pointer_cast<ChainCutSequence>(pCutSequence);
        if (pChainSeq->GetResetPtList()->IsExist(*m_pClosestResetPt_onCutPath))
        {
            pChainSeq_ret = pChainSeq;
            break;
        }
    }

    // ɾ����λ��
    pChainSeq_ret->GetResetPtList()->DeletePt(*m_pClosestResetPt_onCutPath);
    pChainSeq_ret->SetModified(TRUE);
    Update_cutPath_and_resetPt(); // �����������и�·���͸�λ�㡣
    Calc_closestResetPt_onCutPath(pt); // �����и�·���Ͼ����������ĸ�λ��

	return TRUE;
}

BOOL ResetInstructionDeleteAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

    // �����и�·���Ͼ����������ĸ�λ��
    Calc_closestResetPt_onCutPath(pt);

	return TRUE;
}

BOOL ResetInstructionDeleteAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL ResetInstructionDeleteAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	// �������е��������и�·������ʱ��Ҫ����ƫ�ơ�
	m_pViewPort->SetDrawColor(*pDisplayParam->GetSequenceColor());
	SequenceSimulateShapeListPtr pSeqSimShapeList = CutNodeManager::CutNode2SimShape(m_pCutNode_4_chainSeq.get());
	cutSequenceDrawer.DrawSequenceSimulateShapes(pSeqSimShapeList.get());

    // �������и�λ��
    LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);
    for (unsigned int i = 0; i < m_pResetPtList->size(); i++)
    {
        loopFeatureDrawer.DrawFeatureLoc(m_pResetPtList->at(i), RGB(255, 0, 0));
    }

    // �����и�·���ϵ��������ĸ�λ��
	if (m_pClosestResetPt_onCutPath)
	{
		loopFeatureDrawer.DrawFeatureLoc(*m_pClosestResetPt_onCutPath, ColorSelector::GetColor(COLOR_TYPE::COLOR_GREEN));
	}

    // draw cursor.
    m_pViewPort->SetDrawColor(*pDisplayParam->GetPatColor());
    CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	return TRUE;
}

void ResetInstructionDeleteAction::Update_cutPath_and_resetPt()
{
    // �����и�·������ʱ���ܿ�����ƫ�ơ�
    const SequenceConfigItem* pSeqConfigItem = m_pConfigData->GetSeqConfigItem();
    const LeadConfigItem* pLeadConfigItem = m_pConfigData->GetLeadConfigItem();
    const IMaterialSize* pMaterialSize = m_pConfigData->GetMatSize();
    Point2D startSeqPt = pMaterialSize->GetOuterRect().GetCornerPt(pSeqConfigItem->GetStartCorner());
    PartPlacementListPtr pPartPlacements = m_pPartLayoutData->GetPartPlacements();
    PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances();
    std::set<LONGLONG> patternLoops_failedToOffset;
    CutNodeListPtr pCutNodeList_4_simulate = CutNodeManager::CutSequences2CutNodes_4_simulate(startSeqPt, pPartPlacements.get(), pPartInstances.get(),
        m_pCutSequences.get(), pLeadConfigItem, pSeqConfigItem, patternLoops_failedToOffset);

    // ɾ��ƫ�����и�·���ĸ�λ�㡣
    ChainCutSequenceManager::DeleteResetPts_whichDeviateFromCutPath(m_pAllChainSeqs, pCutNodeList_4_simulate);

    // ��Щ������Ӧ���и�·��
    m_pCutNode_4_chainSeq.reset(new CutNodeList);
    for (unsigned int i = 0; i < pCutNodeList_4_simulate->size(); i++)
    {
        ICutNodePtr pCutNode = pCutNodeList_4_simulate->at(i);
        if (m_pAllChainSeqs->GetCutSequenceByID(pCutNode->GetParentID()))
        {
            m_pCutNode_4_chainSeq->push_back(pCutNode);
        }
    }

    // ��Щ������Ӧ�ĸ�λ��
    m_pResetPtList.reset(new Point2DList);
    for (unsigned int i = 0; i < m_pAllChainSeqs->size(); i++)
    {
        ChainCutSequencePtr pChainSeq = boost::dynamic_pointer_cast<ChainCutSequence>(m_pAllChainSeqs->at(i));
        Point2DListPtr pResetPtList = pChainSeq->GetResetPtList();
        m_pResetPtList->insert(m_pResetPtList->end(), pResetPtList->begin(), pResetPtList->end());
    }
}

void ResetInstructionDeleteAction::Calc_closestResetPt_onCutPath(const Point2D& pt)
{
	m_pClosestResetPt_onCutPath.reset();

    double dDistance = INFINITE_VAL;
    for (unsigned int i = 0; i < m_pResetPtList->size(); i++)
    {
        Point2D resetPt = m_pResetPtList->at(i);
        double dDis = resetPt.DistanceTo(pt);
        if (dDis < dDistance)
        {
            dDistance = dDis;
            m_pClosestResetPt_onCutPath.reset(new Point2D(resetPt));
        }
    }
}

END_CUTLEADER_NAMESPACE()
