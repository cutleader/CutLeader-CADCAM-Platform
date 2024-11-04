#include "StdAfx.h"
#include "MovePtAddAction.h"

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

#include "IMaterialSize.h"
#include "SequenceConfigItem.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

MovePtAddAction::MovePtAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
								 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iMsgID = IDS_SEQ_ADD_MOVPT_BASE;

	// �������й���֮��Ŀ����ƶ��㡣
	Point2D startSeqPt = pConfigData->GetMatSize()->GetOuterRect().GetCornerPt(pConfigData->GetSeqConfigItem()->GetStartCorner());
	m_movePtData = TraceDataMgr::GetTracePtData(startSeqPt, pCutSequences);

	m_iCurrentSeq = INVALID_ID;
	m_iBaseSeq = INVALID_ID;
}

MovePtAddAction::~MovePtAddAction(void)
{
}

BOOL MovePtAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_iCurrentSeq != INVALID_ID)
	{
		if (m_iBaseSeq == INVALID_ID)
		{
			m_iBaseSeq = m_iCurrentSeq;
			m_baseMovPt = m_curMovPt;
			m_iMsgID = IDS_SEQ_ADD_MOVPT_PT;
		}
		else
		{
			// ��������ƶ��㡣
			Point2DListPtr pSelMovPts = m_movePtData[m_iBaseSeq];
			int iIndex = pSelMovPts->GetPtIndex(m_baseMovPt);
			pSelMovPts->insert(pSelMovPts->begin()+iIndex+1, pt);

			// �޸Ĺ���Ŀ����ƶ��㡣
			ICutSequencePtr pCutSequence = m_pCutSequences->GetCutSequenceByID(m_iBaseSeq);
			Point2DListPtr pPoint2DList = pCutSequence->GetTracePtList();
			pPoint2DList->insert(pPoint2DList->begin()+iIndex, pt);
			pCutSequence->SetModified(TRUE);

			m_baseMovPt = pt;

			// ���¡��и�ڵ㡱���档
			RefreshCutNodes();
		}
	}
	else
	{
		CString strProductName, strMsg;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_MOVPT_NO_BASE_SEL);
		MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
	}

	return TRUE;
}

BOOL MovePtAddAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// �ҵ�����Ŀ����ƶ��ߡ�
	if (m_iBaseSeq == INVALID_ID)
		FindClosestMovPt(pt);

	return TRUE;
}

BOOL MovePtAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL MovePtAddAction::DrawBeforeFinish()
{
	const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	if (m_iBaseSeq != INVALID_ID) // ��ʱ���ڲ����ƶ��㡣
	{
		// ����׼�㡣
		m_pViewPort->SetDrawColor(*pDisplayParam->GetSelectObjColor());
		m_pViewPort->SetLineWidth(1);
		m_pViewPort->DrawCrossPoint2DEx(m_baseMovPt.X(), m_baseMovPt.Y(), 12, 0);
	}
	else
	{
		if (m_iCurrentSeq != INVALID_ID)
		{
			// ��ǰ�����ƶ��㡣
			m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());
			m_pViewPort->SetLineWidth(2);		
			m_pViewPort->DrawCrossPoint2DEx(m_curMovPt.X(), m_curMovPt.Y(), 12, 0);
		}
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	if (m_iBaseSeq != INVALID_ID)
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);
	else
		CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void MovePtAddAction::FindClosestMovPt(const Point2D& pt)
{
	double dDis = INVALID_VALUE;
	for (map<LONGLONG, Point2DListPtr>::iterator iter = m_movePtData.begin(); iter != m_movePtData.end(); iter++)
	{
		Point2DListPtr pPtList = iter->second;
		for (unsigned int i = 0; i < pPtList->size()-1; i++)
		{
			if (dDis == INVALID_VALUE)
			{
				dDis = pPtList->at(i).DistanceTo(pt);
				m_iCurrentSeq = iter->first;
				m_curMovPt = pPtList->at(i);
			}
			else
			{
				Point2D tmpPt = pPtList->at(i);
				double dTmpDis = tmpPt.DistanceTo(pt);
				if (dTmpDis < dDis)
				{
					dDis = dTmpDis;
					m_iCurrentSeq = iter->first;
					m_curMovPt = tmpPt;
				}
			}
		}
	}
}

END_CUTLEADER_NAMESPACE()
