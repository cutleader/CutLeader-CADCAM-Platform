#include "StdAfx.h"
#include "RemnantCutlineDeleteAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "Point2DList.h"
#include "Line2D.h"
#include "LineArc2DList.h"
#include "ValueInputManager.h"
#include "clBaseDataResource.h"

#include "ICutPartSequence.h"
#include "CutSequenceList.h"
#include "CutSequenceSelectAction.h"
#include "CutNodeManager.h"
#include "clCutSequenceResource.h"
#include "TraceDataMgr.h"
#include "CutSequenceDrawer.h"
#include "ConfigData.h"
#include "CutSequenceManager.h"
#include "CutSequenceDrawer.h"

#include "IMaterialSize.h"
#include "SequenceConfigItem.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

RemnantCutlineDeleteAction::RemnantCutlineDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
								 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iMsgID = IDS_SEQ_DELETE_MOV_PT;
}

RemnantCutlineDeleteAction::~RemnantCutlineDeleteAction(void)
{
}

BOOL RemnantCutlineDeleteAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pActiveRemnantLineSeq)
	{
		CutSequenceManager::DeleteCutSeq(m_pCutSequences, m_pActiveRemnantLineSeq.get(), m_pConfigData->GetMatSize(), m_pConfigData->GetSeqConfigItem());
		m_pCutSequences->SetModified(TRUE);
		RefreshCutNodes(); // ���¡��и�ڵ㡱���档
	}

	return TRUE;
}

BOOL RemnantCutlineDeleteAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// �õ���������������и��߹���
	CutSequenceListPtr pRemnantLineSeqs = m_pCutSequences->GetCutSequencesByType(CutSequence_RemnantCutLine);
	double dDistance = INFINITE_VAL;
	for (unsigned int i = 0; i < pRemnantLineSeqs->size(); i++)
	{
		const Point2D& startPt = pRemnantLineSeqs->at(i)->GetFirstCutPt();
		if (startPt.DistanceTo(pt) < dDistance)
		{
			dDistance = startPt.DistanceTo(pt);
			m_pActiveRemnantLineSeq = pRemnantLineSeqs->at(i);
		}
	}

	return TRUE;
}

BOOL RemnantCutlineDeleteAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL RemnantCutlineDeleteAction::DrawBeforeFinish()
{
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// ���Ƶ�ǰ�Ĺ���
	if (m_pActiveRemnantLineSeq)
	{
		cutSequenceDrawer.DrawActiveCutSequence(m_pActiveRemnantLineSeq.get(), m_pCutNodeList.get());
	}

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

END_CUTLEADER_NAMESPACE()