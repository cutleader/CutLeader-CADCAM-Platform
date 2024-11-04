#include "StdAfx.h"
#include "DistanceMeasureAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "LineArc2DList.h"
#include "BasicGeometryDrawer.h"

#include "IPatternLoop.h"
#include "PatternLoopCache.h"
#include "LoopDrawer.h"
#include "LoopCutFeatureDrawer.h"

#include "clPartLayoutResource.h"
#include "LoopInstanceList.h"
#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "PartPlacementManager.h"
#include "LoopInstanceManager.h"

#include "IMaterialSize.h"
#include "MaterialProcessor.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

DistanceMeasureAction::DistanceMeasureAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_iState = NONE;
	m_iMsgID = IDS_MEASURE_DIS_FST_LOOP;

	// �õ�������Χ����·ʵ������
	m_pAllLoopInstances = LoopInstanceManager::GetLoopInstanceList(m_pPartInstanceList);

	// ������ϱ߿�Ļ���ͼ�Ρ�
	m_pSheetBorderGeoms = MaterialProcessor::GetBorderLineArcs(m_pMatSize.get());
}

DistanceMeasureAction::~DistanceMeasureAction(void)
{
}

BOOL DistanceMeasureAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	Point2D pt(dPtX, dPtY);
	if (m_iState == NONE || m_iState == SECOND_PT_SELECTED)
	{
		m_iState = FIRST_LOOP_SELECTED;
		m_iMsgID = IDS_MEASURE_DIS_FST_PT;
	}
	else if (m_iState == FIRST_LOOP_SELECTED)
	{
		m_iState = FIRST_PT_SELECTED;
		m_iMsgID = IDS_MEASURE_DIS_SND_LOOP;
	}
	else if (m_iState == FIRST_PT_SELECTED)
	{
		m_iState = SECOND_LOOP_SELECTED;
		m_iMsgID = IDS_MEASURE_DIS_SND_PT;
	}
	else if (m_iState == SECOND_LOOP_SELECTED)
	{
		// ��ʾ���������
		CString strDis1, strDis2, strDis3;
		strDis1.Format(_T("%.3f"), m_fstPt.DistanceTo(m_sndPt));
		strDis2.Format(_T("%.3f"), m_fstPt.XDistanceTo(m_sndPt));
		strDis3.Format(_T("%.3f"), m_fstPt.YDistanceTo(m_sndPt));
		CString strMsg, strMsg1, strMsg2, strMsg3;
		strMsg1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MEASURE_DIS_RESULT_1);
		strMsg1.Replace(_T("WILLBEREPLACED1"), strDis1);
		strMsg2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MEASURE_DIS_RESULT_2);
		strMsg2.Replace(_T("WILLBEREPLACED2"), strDis2);
		strMsg3 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MEASURE_DIS_RESULT_3);
		strMsg3.Replace(_T("WILLBEREPLACED3"), strDis3);
		strMsg = strMsg1 + _T("\n") + strMsg2 + _T("\n") + strMsg3;
		MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONINFORMATION);

		m_iState = SECOND_PT_SELECTED;
		m_iMsgID = IDS_MEASURE_DIS_FST_PT;
	}

	return TRUE;
}

BOOL DistanceMeasureAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_iState == NONE || m_iState == SECOND_PT_SELECTED)
	{
		// ���������Ļ�·��
		m_pCloseLoop = CalcClosestLoop(pt);
	}
	else if (m_iState == FIRST_LOOP_SELECTED)
	{
		// ��һ�������㡣
		m_fstPt = m_pCloseLoop->GetClosestPt(pt);
	}
	else if (m_iState == FIRST_PT_SELECTED)
	{
		// ���������Ļ�·��
		m_pCloseLoop = CalcClosestLoop(pt);
	}
	else if (m_iState == SECOND_LOOP_SELECTED)
	{
		// �ڶ��������㡣
		m_sndPt = m_pCloseLoop->GetClosestPt(pt);
	}

	return TRUE;
}

BOOL DistanceMeasureAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL DistanceMeasureAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();
	BasicGeometryDrawer geomBaseDrawer(m_pViewPort);
	LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	if (m_iState == NONE || m_iState == SECOND_PT_SELECTED)
	{
		// ��������Ļ�·��
		if (m_pCloseLoop)
		{
			m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());
			m_pViewPort->SetLineWidth(2);
			geomBaseDrawer.Draw2DLineArcs(m_pCloseLoop.get());
		}
	}
	else if (m_iState == FIRST_LOOP_SELECTED)
	{
		loopFeatureDrawer.DrawPosRing(m_fstPt);
	}
	else if (m_iState == FIRST_PT_SELECTED)
	{
		// ���Ƶ�һ�������㡣
		loopFeatureDrawer.DrawFeatureLoc(m_fstPt, RGB(255,0,0));

		// ��������Ļ�·��
		if (m_pCloseLoop)
		{
			m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());
			m_pViewPort->SetLineWidth(2);
			geomBaseDrawer.Draw2DLineArcs(m_pCloseLoop.get());
		}
	}
	else if (m_iState == SECOND_LOOP_SELECTED)
	{
		// ���Ƶ�һ�������㡣
		loopFeatureDrawer.DrawFeatureLoc(m_fstPt, RGB(255,0,0));

		// ���Ƶڶ��������㡣
		loopFeatureDrawer.DrawPosRing(m_sndPt);
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

LineArc2DListPtr DistanceMeasureAction::CalcClosestLoop(const Point2D& pt)
{
	LineArc2DListPtr pGeom2Ds;

	// ����ı߿����С���롣
	double dBorderDis = m_pSheetBorderGeoms->GetClosestDis(pt);

	// �����С���·ʵ��������̾��롣
	double dLoopInstDis = INFINITE_VAL;
	LoopInstancePtr pLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pAllLoopInstances, pt);
	if (pLoopInstance)
		dLoopInstDis = pLoopInstance->DistanceTo(pt);

	if (dBorderDis > dLoopInstDis) // ����·ʵ������ýϽ���
	{
		IPatternLoopPtr pPatternLoop = pLoopInstance->GetCacheData()->GetPatternLoop();
		pGeom2Ds = pPatternLoop->GetCacheData()->GetLoopBaseGeom()->Clone();
		pGeom2Ds->Transform(pLoopInstance->GetTransformMatrix());
	}
	else // ��������ýϽ���
		pGeom2Ds = m_pSheetBorderGeoms;

	return pGeom2Ds;
}

END_CUTLEADER_NAMESPACE()
