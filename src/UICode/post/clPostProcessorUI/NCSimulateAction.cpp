#include "StdAfx.h"
#include "NCSimulateAction.h"

#include "GlViewPort.h"
#include "Line2D.h"
#include "Rect2D.h"
#include "DummyAction.h"
#include "CursorDrawer.h"

#include "CutNodeList.h"
#include "CutNodeManager.h"
#include "CutSequenceDrawer.h"

#include "clPostProcessorResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

NCSimulateAction::NCSimulateAction(GlViewPortPtr pViewPort)
{
	m_pViewPort = pViewPort;
	m_iMsgID = IDS_MSG_SIM_NC_ROW;
	m_bDisplayValInput = FALSE;
    m_pWrongToolSizeLoopInstances.reset(new WrongToolSizeLoopInstances);
}

NCSimulateAction::~NCSimulateAction(void)
{
}

BOOL NCSimulateAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ActionBase::OnKeyDown(nChar, nRepCnt, nFlags);
	return TRUE;
}

BOOL NCSimulateAction::DrawBeforeFinish()
{
	const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();
    CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// �ϵ����á�
	DWORD dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	// ��ѡ�е�NC�����ж�Ӧ�ġ��и�ڵ㡱��������
	m_pViewPort->SetDrawColor(*pDisplayParam->GetSeqSimColor());
	m_pViewPort->SetLineWidth(2);
	for (unsigned int i = 0; i < m_pCutNodeList->size(); i++)
	{
		const ICutNode* pCutNode = m_pCutNodeList->at(i).get();
		SequenceSimulateShapeListPtr pSimShapeList = CutNodeManager::CutNode2SimShape(pCutNode);
		cutSequenceDrawer.DrawSequenceSimulateShapes(pSimShapeList.get(), false);
	}

	// �ָ���ͼ���á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

    // ����ƫ�������������ʵ����
    if (pDisplayParam->GetShowIncorrectToolSize())
    {
        m_pWrongToolSizeLoopInstances->Draw(m_pViewPort);
    }

	// draw cursor.
	CursorDrawer::DrawDefaultCursor(m_pViewPort);

	return TRUE;
}

void NCSimulateAction::ViewCutNode(SequenceSimulateShapeListPtr pSeqSimShapeList, CutNodeListPtr pCutNodeList)
{
	m_pSequenceSimulateShapeList = pSeqSimShapeList;
	m_pCutNodeList = pCutNodeList;
}

END_CUTLEADER_NAMESPACE()
