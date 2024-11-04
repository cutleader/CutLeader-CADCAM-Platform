#include "StdAfx.h"
#include "MicroJointPreSelectAction2.h"

#include "MathEx.h"
#include "Rect2D.h"
#include "Point2DList.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "ValueInputManager.h"

#include "clGeometryFeatureResource.h"

#include "clCutFeatureResource.h"
#include "LoopCutFeatureDrawer.h"
#include "MicroJointList.h"
#include "LoopMicroJointList.h"
#include "LoopCutFeatureList.h"

#include "LoopInstanceList.h"

#include "DisplayParam.h"
#include "CLOptionInfo.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

MicroJointPreSelectAction2::MicroJointPreSelectAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort)
    : PartPlacementEditAction(pMaterialSize, pPartPlacementList, pPartInstanceList, pPartGroups, pViewPort)
{
	m_preSelectState = PRE_SELECT_NO_INPUT;
	m_iMsgID = IDS_CUTFEATURE_SELECT_MicroJoint;
}

MicroJointPreSelectAction2::~MicroJointPreSelectAction2(void)
{
}

BOOL MicroJointPreSelectAction2::LButtonDown(double& dPtX, double& dPtY)
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

BOOL MicroJointPreSelectAction2::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;

		// ȷ��ѡ�е�΢����������
		vector< pair<MicroJointPtr, Point2D> > selMJPtPairs = CalcSelectMJPt();
		if (selMJPtPairs.size() > 0)
		{
			// Ҫ��û�а���ctrl����Ҫ���֮ǰ��ѡ��
			if (!ValueInputManager::GetInstance()->IsInputCtrl())
				m_selMjPtPairs.clear();

			// ����ѡ�е�΢������Ϣ��
			m_selMjPtPairs.insert(m_selMjPtPairs.end(), selMJPtPairs.begin(), selMJPtPairs.end());
		}

		// ȷ����ʾ��Ϣ��
		if (m_selMjPtPairs.size() == 0)
			m_iMsgID = IDS_CUTFEATURE_NO_MJ_SELECT;
		else
			m_iMsgID = IDS_CUTFEATURE_MJ_SELECTED;

		// ��ǰ�����״̬��
		m_preSelectState = PRE_SELECT_LEFT_BTN_UP;
	}

	return TRUE;
}

BOOL MicroJointPreSelectAction2::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	// �õ����������΢���ӵ㡣
	Point2DListPtr pMJPtList = m_pAllLoopInstanceList->GetAllMicroJointPts();
	if (pMJPtList->size() > 0)
		m_pActiveMJPt.reset(new Point2D(pMJPtList->GetPtClosestTo(pt)));
	else
		m_pActiveMJPt.reset();

	// if left button down, we should draw a rectangle in stipple mode.
	if (m_preSelectState == PRE_SELECT_LEFT_BTN_DOWN)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL MicroJointPreSelectAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PartPlacementEditAction::OnKeyDown(nChar, nRepCnt, nFlags);

	// ѡ��ȫ����΢������������
	if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		m_selMjPtPairs.clear();
		vector< pair<MicroJointPtr, Point2D> > mjPtPairs = m_pAllLoopInstanceList->GetAllMicroJointPtPairs();
		m_selMjPtPairs.insert(m_selMjPtPairs.end(), mjPtPairs.begin(), mjPtPairs.end());
	}

	return TRUE;
}

BOOL MicroJointPreSelectAction2::DrawBeforeFinish()
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
	if (m_pActiveMJPt)
		loopFeatureDrawer.DrawFeatureLoc(*m_pActiveMJPt, *pColorInfo->GetActiveObjColor());

	// ����ѡ�е�΢���ӵ㡣
	for (unsigned int i = 0; i < m_selMjPtPairs.size(); i++)
		loopFeatureDrawer.DrawFeatureLoc(m_selMjPtPairs.at(i).second, *pColorInfo->GetSelectObjColor());

	// ���ƹ�ꡣ
	m_pViewPort->SetDrawColor(*pColorInfo->GetPatColor());
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	// restore the draw color
	m_pViewPort->SetDrawColor(dwOldColor);

	return TRUE;
}

MicroJointListPtr MicroJointPreSelectAction2::GetSelectMJList()
{
	MicroJointListPtr pMicroJointList(new MicroJointList);

	for (unsigned int i = 0; i < m_selMjPtPairs.size(); i++)
		pMicroJointList->push_back(m_selMjPtPairs.at(i).first);

	return pMicroJointList;
}

vector< pair<MicroJointPtr, Point2D> > MicroJointPreSelectAction2::CalcSelectMJPt()
{
	vector< pair<MicroJointPtr, Point2D> > selMjPtPairs;

	// �õ����С�΢���ӵ㡱��Ӧ�ġ�΢������������
	vector< pair<MicroJointPtr, Point2D> > mjPtPairs = m_pAllLoopInstanceList->GetAllMicroJointPtPairs();

	if (MathEx::Equal(m_data[0], m_data[2]) && MathEx::Equal(m_data[1], m_data[3]))
	{
		Point2D pt(m_data[0], m_data[1]);

		// �õ�������������΢����������
		double dDis = INFINITE_VAL;
		int iIndex = INVALID_IDX;
		for (unsigned int i = 0; i < mjPtPairs.size(); i++)
		{
			pair<MicroJointPtr, Point2D> mjPtPair = mjPtPairs.at(i);
			double dTmpDis = pt.DistanceTo(mjPtPair.second);
			if (dTmpDis < dDis)
			{
				dDis = dTmpDis;
				iIndex = i;
			}
		}
		if (iIndex != INVALID_IDX)
			selMjPtPairs.push_back(mjPtPairs[iIndex]);
	}
	else
	{
		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);

		// �õ���ѡ����е�΢����������
		for (unsigned int i = 0; i < mjPtPairs.size(); i++)
		{
			pair<MicroJointPtr, Point2D> mjPtPair = mjPtPairs.at(i);
			if (rect.ContainPt(mjPtPair.second))
				selMjPtPairs.push_back(mjPtPairs[i]);
		}
	}

	return selMjPtPairs;
}

END_CUTLEADER_NAMESPACE()
