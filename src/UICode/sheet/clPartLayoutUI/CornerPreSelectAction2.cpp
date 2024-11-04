#include "StdAfx.h"
#include "CornerPreSelectAction2.h"

#include "MathEx.h"
#include "Rect2D.h"
#include "Point2DList.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "ValueInputManager.h"

#include "clGeometryFeatureResource.h"

#include "clCutFeatureResource.h"
#include "LoopCutFeatureDrawer.h"
#include "CornerList.h"
#include "LoopCornerDataList.h"
#include "LoopCutFeatureList.h"

#include "LoopInstanceList.h"

#include "DisplayParam.h"
#include "CLOptionInfo.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

CornerPreSelectAction2::CornerPreSelectAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort)
    : PartPlacementEditAction(pMaterialSize, pPartPlacementList, pPartInstanceList, pPartGroups, pViewPort)
{
	m_preSelectState = PRE_SELECT_NO_INPUT;
	m_iMsgID = IDS_CUTFEATURE_SELECT_CORNER;
}

CornerPreSelectAction2::~CornerPreSelectAction2(void)
{
}

BOOL CornerPreSelectAction2::LButtonDown(double& dPtX, double& dPtY)
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

BOOL CornerPreSelectAction2::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;

		// ȷ��ѡ�еĽ�������
		vector< pair<ICornerPtr, Point2D> > selCornerPtPairs = CalcSelectCornerPt();
		if (selCornerPtPairs.size() > 0)
		{
			// Ҫ��û�а���ctrl����Ҫ���֮ǰ��ѡ��
			if (!ValueInputManager::GetInstance()->IsInputCtrl())
				m_selCornerPtPairs.clear();

			// ����ѡ�еġ�����������Ϣ��
			m_selCornerPtPairs.insert(m_selCornerPtPairs.end(), selCornerPtPairs.begin(), selCornerPtPairs.end());
		}

		// ȷ����ʾ��Ϣ��
		if (m_selCornerPtPairs.size() == 0)
			m_iMsgID = IDS_CUTFEATURE_NO_CORNER_SELECT;
		else
			m_iMsgID = IDS_CUTFEATURE_CORNER_SELECTED;

		// ��ǰ�����״̬��
		m_preSelectState = PRE_SELECT_LEFT_BTN_UP;
	}

	return TRUE;
}

BOOL CornerPreSelectAction2::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// �õ���������Ľ������㡣
	Point2DListPtr pCornerPtList = m_pAllLoopInstanceList->GetAllCornerPts();
	if (pCornerPtList->size() > 0)
		m_pActiveCornerPt.reset(new Point2D(pCornerPtList->GetPtClosestTo(pt)));
	else
		m_pActiveCornerPt.reset();

	// if left button down, we should draw a rectangle in stipple mode.
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL CornerPreSelectAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PartPlacementEditAction::OnKeyDown(nChar, nRepCnt, nFlags);

	// ѡ��ȫ��������������
	if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		m_selCornerPtPairs.clear();
		vector< pair<ICornerPtr, Point2D> > cornerPtPairs = m_pAllLoopInstanceList->GetAllCornerPtPairs();
		m_selCornerPtPairs.insert(m_selCornerPtPairs.end(), cornerPtPairs.begin(), cornerPtPairs.end());
	}

	return TRUE;
}

BOOL CornerPreSelectAction2::DrawBeforeFinish()
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
	if (m_pActiveCornerPt)
		loopFeatureDrawer.DrawFeatureLoc(*m_pActiveCornerPt, *pColorInfo->GetActiveObjColor());

	// ����ѡ�е�΢���ӵ㡣
	for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
		loopFeatureDrawer.DrawFeatureLoc(m_selCornerPtPairs.at(i).second, *pColorInfo->GetSelectObjColor());

	// ���ƹ�ꡣ
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

CornerListPtr CornerPreSelectAction2::GetSelectCornerList()
{
	CornerListPtr pCornerList(new CornerList);

	for (unsigned int i = 0; i < m_selCornerPtPairs.size(); i++)
	{
		pair<ICornerPtr, Point2D> cornerPtPair = m_selCornerPtPairs.at(i);
		pCornerList->push_back(cornerPtPair.first);
	}

	return pCornerList;
}

vector< pair<ICornerPtr, Point2D> > CornerPreSelectAction2::CalcSelectCornerPt()
{
	vector< pair<ICornerPtr, Point2D> > selCornerPtPairs;

	// �õ����С��������㡱����Ӧ�ġ�������������
	vector< pair<ICornerPtr, Point2D> > cornerPtPairs = m_pAllLoopInstanceList->GetAllCornerPtPairs();

	if (MathEx::Equal(m_data[0], m_data[2]) && MathEx::Equal(m_data[1], m_data[3]))
	{
		Point2D pt(m_data[0], m_data[1]);

		// �õ������������Ľ�������
		double dDis = INFINITE_VAL;
		int iIndex = INVALID_IDX;
		for (unsigned int i = 0; i < cornerPtPairs.size(); i++)
		{
			pair<ICornerPtr, Point2D> cornerPtPair = cornerPtPairs.at(i);
			double dTmpDis = pt.DistanceTo(cornerPtPair.second);
			if (dTmpDis < dDis)
			{
				dDis = dTmpDis;
				iIndex = i;
			}
		}
		if (iIndex != INVALID_IDX)
			selCornerPtPairs.push_back(cornerPtPairs[iIndex]);
	}
	else
	{
		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);

		// �õ���ѡ����еĽ�������
		for (unsigned int i = 0; i < cornerPtPairs.size(); i++)
		{
			pair<ICornerPtr, Point2D> cornerPtPair = cornerPtPairs.at(i);
			if (rect.ContainPt(cornerPtPair.second))
				selCornerPtPairs.push_back(cornerPtPairs[i]);
		}
	}

	return selCornerPtPairs;
}

END_CUTLEADER_NAMESPACE()
