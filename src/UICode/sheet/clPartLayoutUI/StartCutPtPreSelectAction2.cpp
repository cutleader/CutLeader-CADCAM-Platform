#include "StdAfx.h"
#include "StartCutPtPreSelectAction2.h"

#include "MathEx.h"
#include "Rect2D.h"
#include "Point2DList.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "ValueInputManager.h"

#include "clGeometryFeatureResource.h"

#include "clCutFeatureResource.h"
#include "LoopStartCutDataList.h"
#include "LoopCutFeatureList.h"
#include "LoopCutFeatureDrawer.h"

#include "LoopInstanceList.h"
#include "PartInstanceList.h"

#include "DisplayParam.h"
#include "CLOptionInfo.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

StartCutPtPreSelectAction2::StartCutPtPreSelectAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort)
    : PartPlacementEditAction(pMaterialSize, pPartPlacementList, pPartInstanceList, pPartGroups, pViewPort)
{
	m_preSelectState = PRE_SELECT_NO_INPUT;
	m_iMsgID = IDS_CUTFEATURE_SELECT_STARTCUT;
}

StartCutPtPreSelectAction2::~StartCutPtPreSelectAction2(void)
{
}

BOOL StartCutPtPreSelectAction2::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_NO_INPUT || m_preSelectState == PRE_SELECT_LEFT_BTN_UP)
	{
		m_preSelectState = PRE_SELECT_LEFT_BTN_DOWN;
		m_iMsgID = IDS_OUTPUT_SELECTPAT_NEXTPT;

		m_data[0] = dPtX;
		m_data[1] = dPtY;
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL StartCutPtPreSelectAction2::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;

		// ȷ��ѡ�еġ���·������������
		vector< pair<LoopStartCutDataPtr, Point2D> > selStaEndPtPairs = CalcSelLoopStaEndCut();
		if (selStaEndPtPairs.size() > 0)
		{
			// Ҫ��û�а���ctrl����Ҫ���֮ǰ��ѡ��
			if (!ValueInputManager::GetInstance()->IsInputCtrl())
				m_selStaEndPtPairs.clear();

			// ����ѡ�еġ���·������������
			m_selStaEndPtPairs.insert(m_selStaEndPtPairs.end(), selStaEndPtPairs.begin(), selStaEndPtPairs.end());
		}

		// ȷ����ʾ��Ϣ��
		if (m_selStaEndPtPairs.size() == 0)
			m_iMsgID = IDS_CUTFEATURE_NO_STARTPT_SELECT;
		else
			m_iMsgID = IDS_CUTFEATURE_STARTPT_SELECTED;

		// ��ǰ�����״̬��
		m_preSelectState = PRE_SELECT_LEFT_BTN_UP;
	}

	return TRUE;
}

BOOL StartCutPtPreSelectAction2::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// �õ�������������е㡣
	Point2DListPtr pStartPtList = m_pAllLoopInstanceList->GetAllStartCutPts();
	if (pStartPtList->size() > 0)
		m_pActiveStartPt.reset(new Point2D(pStartPtList->GetPtClosestTo(pt)));
	else
		m_pActiveStartPt.reset();

	// if left button down, we should draw a rectangle in stipple mode.
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL StartCutPtPreSelectAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// ѡ��ȫ������·������������
	if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		m_selStaEndPtPairs.clear();
		vector< pair<LoopStartCutDataPtr, Point2D> > staEndPtPairs = m_pAllLoopInstanceList->GetAllStartCutPtPairs();
		m_selStaEndPtPairs.insert(m_selStaEndPtPairs.end(), staEndPtPairs.begin(), staEndPtPairs.end());
	}

	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL StartCutPtPreSelectAction2::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pColorInfo = pCLOptionInfo->GetDisplayParam();
	LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);

	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	// ����ѡ���
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		// set the stipple mode.
		bool oldStipple = false;
		int oldRepeat = 1;
		unsigned short oldPattern = 0xffff;
		m_pViewPort->GetLineStipple(oldStipple, oldRepeat, oldPattern);
		m_pViewPort->SetLineStipple(true, 2, 0xcccc);

		// draw the selection rectangle.
		m_pViewPort->DrawRect(m_data[0], m_data[1], m_data[2], m_data[3]);

		// restore the drawer
		m_pViewPort->SetLineStipple(oldStipple, oldRepeat, oldPattern);
	}

	// ���Ƶ�ǰ΢���ӵ㡣
	if (m_pActiveStartPt)
		loopFeatureDrawer.DrawFeatureLoc(*m_pActiveStartPt, *pColorInfo->GetActiveObjColor());

	// ����ѡ�е�΢���ӵ㡣
	for (unsigned int i = 0; i < m_selStaEndPtPairs.size(); i++)
		loopFeatureDrawer.DrawFeatureLoc(m_selStaEndPtPairs.at(i).second, *pColorInfo->GetSelectObjColor());

	// ���ƹ�ꡣ
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

vector< pair<LoopStartCutDataPtr, Point2D> > StartCutPtPreSelectAction2::CalcSelLoopStaEndCut()
{
	vector< pair<LoopStartCutDataPtr, Point2D> > selStaEndPtPairs;

	// �õ����С���·�����������͡����е㡱��ԡ�
	vector< pair<LoopStartCutDataPtr, Point2D> > staEndPtPairs = m_pAllLoopInstanceList->GetAllStartCutPtPairs();

	if (MathEx::Equal(m_data[0], m_data[2]) && MathEx::Equal(m_data[1], m_data[3]))
	{
		Point2D pt(m_data[0], m_data[1]);

		// �õ�������������΢����������
		double dDis = INFINITE_VAL;
		int iIndex = INVALID_IDX;
		for (unsigned int i = 0; i < staEndPtPairs.size(); i++)
		{
			pair<LoopStartCutDataPtr, Point2D> staEndPtPair = staEndPtPairs.at(i);
			double dTmpDis = pt.DistanceTo(staEndPtPair.second);
			if (dTmpDis < dDis)
			{
				dDis = dTmpDis;
				iIndex = i;
			}
		}
		if (iIndex != INVALID_IDX)
			selStaEndPtPairs.push_back(staEndPtPairs[iIndex]);
	}
	else
	{
		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);

		// �õ���ѡ����еġ���·���С�������
		for (unsigned int i = 0; i < staEndPtPairs.size(); i++)
		{
			pair<LoopStartCutDataPtr, Point2D> staEndPtPair = staEndPtPairs.at(i);
			if (rect.ContainPt(staEndPtPair.second))
				selStaEndPtPairs.push_back(staEndPtPairs[i]);
		}
	}

	return selStaEndPtPairs;
}

END_CUTLEADER_NAMESPACE()
