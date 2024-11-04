#include "StdAfx.h"
#include "PartPlacementMoveBase.h"

#include <tuple>
#include "LineArc2DList.h"
#include "Polygon2D.h"
#include "Polygon2DList.h"
#include "Polygon2DBuilder.h"
#include "Polygon2DProcessor.h"
#include "LineArc2DProcessor.h"
#include "PolyViewDlg.h"
#include "LogMessage.h"
#include "LogMgr.h"
#include "DateHelper.h"

#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "PatternLoopCache.h"
#include "PartCadData.h"
#include "LoopToolData.h"
#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "PartCamData.h"
#include "PartItem.h"

#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartInstance.h"
#include "LoopInstance.h"
#include "LoopInstanceCache.h"
#include "LoopInstanceList.h"
#include "LoopInstanceManager.h"
#include "PartPlacementManager.h"
#include "RemnantRegion.h"
#include "RemnantRegionList.h"

#include "SheetEditParam.h"
#include "CLOptionInfo.h"

using namespace std;
using namespace std::tr1;

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementMoveBase::PartPlacementMoveBase(void)
{
}

PartPlacementMoveBase::~PartPlacementMoveBase(void)
{
}

void PartPlacementMoveBase::UpdateBeforeMov()
{
	// ���ƶ������ʵ��������Ӿ��εĳߴ硣
	Rect2D movRect;
	PartInstanceListPtr pMovPartInsts = PartPlacementManager::BuildPartInsts(m_pMovePartPlacements);
	LoopInstanceListPtr pMovLoopInsts = LoopInstanceManager::GetOuterLoopInsts(pMovPartInsts);
	for (unsigned int i = 0; i < pMovLoopInsts->size(); i++)
	{
		Rect2D rect = pMovLoopInsts->at(i)->GetCacheData()->GetOuterRect();
		if (i == 0)
			movRect = rect;
		else
			movRect.Unite(rect);
	}
	m_dMovPartWidth = movRect.GetWidth();
	m_dMovPartHeight = movRect.GetHeight();

	// ÿ�����ƶ������ʵ��������Χ����Ρ�
	m_pPartOuterPolys.reset(new Polygon2DList);
	m_pNewPartOuterPolys.reset(new Polygon2DList);
	double dExpandVal_of_movParts = GetExpandVal_of_movParts(m_partAttachConfig, m_pMovePartPlacements.get());
	for (unsigned int i = 0; i < pMovLoopInsts->size(); i++)
	{
		LoopInstancePtr pMovLoopInst = pMovLoopInsts->at(i);
		IPatternLoopPtr pPatternLoop = pMovLoopInst->GetCacheData()->GetPatternLoop();
		LineArc2DListPtr pGeom2DList = pPatternLoop->GetCacheData()->GetLoopBaseGeom()->Clone();
		pGeom2DList->Transform(pMovLoopInst->GetTransformMatrix());
		Polygon2DListPtr pPolyList = Polygon2DBuilder::BuildPolyListFromLineArc(pGeom2DList, CUTARC_FACTOR);
        if (pPolyList->size() == 1) // �˴���Ҫ���ϱ�����Ҫ��Ȼ���ܱ���
        {
            Polygon2DPtr pPoly = pPolyList->at(0);
            Polygon2DProcessor::SleekPolygon(pPoly, OPTPOLY_FACTOR);

            // ����
            Polygon2DPtr pNewPoly = pPoly->Clone();
            Polygon2DProcessor::EnlargePolygon(pNewPoly, dExpandVal_of_movParts, CUTARC_FACTOR, OPTPOLY_FACTOR);

            m_pPartOuterPolys->push_back(pPoly);
            m_pNewPartOuterPolys->push_back(pNewPoly);
        }
        else
        {
            CString strPartName = pMovLoopInst->GetPartInstance()->GetPartPlacement()->GetPartItem()->GetPartName();
            CString strMsg;
            strMsg.Format(_T("Debug warning 1 in PartPlacementMoveBase::UpdateBeforeMov function! Part name: %s ���ⲻ�����ش��󣬿ɼ�����"), strPartName);
            LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_WARNING, _T("���ɶ���μ���"), strMsg));
            LogMgr::WriteLog(pLogMsg);
        }
	}

	// ����ϳ�����������������á�����������������
	m_pRemnantRegionList = PartPlacementManager::CalcRemnantRegion(m_pMaterialSize, m_pFixPartInstances, m_dMovPartWidth, m_dMovPartHeight);

	m_innerStickPolys.clear();
	m_outerStickPolys.clear();
	m_pStickRemRegion.reset();
}

void PartPlacementMoveBase::UpdateWhenMov(Point2D currentPt)
{
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	if (!pSheetEditParam->EnableDisDetect())
	{
		// �ƶ�ʱ�ı任����
		Matrix2D mat;
		Vector2D vect(m_movCenterPt, currentPt);
		mat.SetTransfer(vect);

		// ���¡��ƶ��е�������á���
		for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
			m_pMovePartPlacements->at(i)->Transform(mat);

		// ���¡��ƶ���������á������ĵ㡣
		m_movCenterPt = currentPt;
	}
	else
	{
		if (!m_pStickRemRegion) // ��ʱû�б�������
		{
			// �ƶ�ʱ�ı任����
			Matrix2D mat;
			Vector2D vect(m_movCenterPt, currentPt);
			mat.SetTransfer(vect);

			// ���¡��ƶ��е�������á���
			for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
				m_pMovePartPlacements->at(i)->Transform(mat);

			// ���¡��ƶ������ʵ��������Χ����Ρ�
			for (unsigned int i = 0; i < m_pPartOuterPolys->size(); i++)
				m_pPartOuterPolys->at(i)->Transform(mat);
			for (unsigned int i = 0; i < m_pNewPartOuterPolys->size(); i++)
				m_pNewPartOuterPolys->at(i)->Transform(mat);

			// ��鿿����
			Point2D stickPt = CheckStickA(currentPt);
			if (m_pStickRemRegion) // ��ס�ˡ�
			{
				// �ƶ�ʱ�ı任����
				Matrix2D mat;
				Vector2D vect(currentPt, stickPt);
				mat.SetTransfer(vect);

				// ���¡��ƶ��е�������á���
				for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
					m_pMovePartPlacements->at(i)->Transform(mat);

				// ���¡��ƶ������ʵ��������Χ����Ρ�
				for (unsigned int i = 0; i < m_pPartOuterPolys->size(); i++)
					m_pPartOuterPolys->at(i)->Transform(mat);
				for (unsigned int i = 0; i < m_pNewPartOuterPolys->size(); i++)
					m_pNewPartOuterPolys->at(i)->Transform(mat);

				// ���¡��ƶ���������á������ĵ㡣
				m_movCenterPt = stickPt;
			}
			else if (!m_pStickRemRegion) // û��ס��
			{
				m_movCenterPt = currentPt;
			}
		}
		else if (m_pStickRemRegion) // ��ʱ�б�������
		{
			// ��鿿����
			Point2D stickPt = CheckStickB(currentPt);
			if (m_pStickRemRegion) // ��Ȼ��ס�ˡ�
			{
				// �ƶ�ʱ�ı任����
				Matrix2D mat;
				Vector2D vect(m_movCenterPt, stickPt);
				mat.SetTransfer(vect);

				// ���¡��ƶ��е�������á���
				for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
					m_pMovePartPlacements->at(i)->Transform(mat);

				// ���¡��ƶ������ʵ��������Χ����Ρ�
				for (unsigned int i = 0; i < m_pPartOuterPolys->size(); i++)
					m_pPartOuterPolys->at(i)->Transform(mat);
				for (unsigned int i = 0; i < m_pNewPartOuterPolys->size(); i++)
					m_pNewPartOuterPolys->at(i)->Transform(mat);

				// ���¡��ƶ���������á������ĵ㡣
				m_movCenterPt = stickPt;
			}
			else
			{
				// �ƶ�ʱ�ı任����
				Matrix2D mat;
				Vector2D vect(m_movCenterPt, currentPt);
				mat.SetTransfer(vect);

				// ���¡��ƶ��е�������á���
				for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
					m_pMovePartPlacements->at(i)->Transform(mat);

				// ���¡��ƶ������ʵ��������Χ����Ρ�
				for (unsigned int i = 0; i < m_pPartOuterPolys->size(); i++)
					m_pPartOuterPolys->at(i)->Transform(mat);
				for (unsigned int i = 0; i < m_pNewPartOuterPolys->size(); i++)
					m_pNewPartOuterPolys->at(i)->Transform(mat);

				m_movCenterPt = currentPt;
			}
		}
	}
}

void PartPlacementMoveBase::UpdateWhenRotate(BOOL bCCW)
{
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	SheetEditParamPtr pSheetEditParam = pCLOptionInfo->GetSheetEditParam();

	// ��ת�ƶ��еġ�������á���
	Rect2D rect = m_pMovePartPlacements->GetRectBox();
	Matrix2D mat;
	if (bCCW)
		mat.RotateBy(rect.GetCenterPt(), pSheetEditParam->GetRotateStep()*D2R);
	else
		mat.RotateBy(rect.GetCenterPt(), -pSheetEditParam->GetRotateStep()*D2R);
	for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
		m_pMovePartPlacements->at(i)->Transform(mat);

	// ����ת����������ĵ�У�Ե����㡣
	Rect2D movRect;
	PartInstanceListPtr pMovPartInsts = PartPlacementManager::BuildPartInsts(m_pMovePartPlacements);
	LoopInstanceListPtr pMovLoopInsts = LoopInstanceManager::GetOuterLoopInsts(pMovPartInsts);
	for (unsigned int i = 0; i < pMovLoopInsts->size(); i++)
	{
		Rect2D rect = pMovLoopInsts->at(i)->GetCacheData()->GetOuterRect();
		if (i == 0)
			movRect = rect;
		else
			movRect.Unite(rect);
	}
	Matrix2D movMat;
	Vector2D vect(movRect.GetCenterPt(), m_movCenterPt);
	movMat.SetTransfer(vect);
	for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
		m_pMovePartPlacements->at(i)->Transform(movMat);

	// �Ƿ�ҪΪ���ܿ���׼�����ݡ�
	if (pSheetEditParam->EnableDisDetect())
	{
		// ���ƶ������ʵ��������Ӿ��εĳߴ硣
		m_dMovPartWidth = movRect.GetWidth();
		m_dMovPartHeight = movRect.GetHeight();

		// ÿ�����ƶ������ʵ��������Χ����Ρ�
		m_pPartOuterPolys.reset(new Polygon2DList);
		m_pNewPartOuterPolys.reset(new Polygon2DList);
		double dExpandVal_of_movParts = GetExpandVal_of_movParts(m_partAttachConfig, m_pMovePartPlacements.get());
		for (unsigned int i = 0; i < pMovLoopInsts->size(); i++)
		{
			LoopInstancePtr pMovLoopInst = pMovLoopInsts->at(i);
			IPatternLoopPtr pPatternLoop = pMovLoopInst->GetCacheData()->GetPatternLoop();
			LineArc2DListPtr pGeom2DList = pPatternLoop->GetCacheData()->GetLoopBaseGeom()->Clone();
			pGeom2DList->Transform(pMovLoopInst->GetTransformMatrix());
			Polygon2DListPtr pPolyList = Polygon2DBuilder::BuildPolyListFromLineArc(pGeom2DList, CUTARC_FACTOR);
			Polygon2DPtr pPoly = pPolyList->at(0);
			Polygon2DProcessor::SleekPolygon(pPoly, OPTPOLY_FACTOR);

			// ����
			Polygon2DPtr pNewPoly = pPoly->Clone();
			Polygon2DProcessor::EnlargePolygon(pNewPoly, dExpandVal_of_movParts, CUTARC_FACTOR, OPTPOLY_FACTOR);

			m_pPartOuterPolys->push_back(pPoly);
			m_pNewPartOuterPolys->push_back(pNewPoly);
		}

		m_innerStickPolys.clear();
		m_outerStickPolys.clear();
		m_pStickRemRegion.reset();
	}
}

double PartPlacementMoveBase::GetExpandVal_of_regionHole(const PartAttachConfig& partAttachConfig, double dToolSize)
{
	double dExpandVal_of_regionHole = partAttachConfig.m_dPartDis / 2.0;
	if (partAttachConfig.m_bEnableComcut)
	{
		dExpandVal_of_regionHole = dToolSize / 2.0;
	}

	return dExpandVal_of_regionHole;
}

double PartPlacementMoveBase::GetCompressVal_of_regionBoundary(const PartAttachConfig& partAttachConfig, BOOL bOuterRegion, const PartPlacementList* pMovePartPlacements)
{
	double dCompressVal = .0;

	if (partAttachConfig.m_bEnableComcut)
	{
		// ���ƶ����������һ�����õ����ϵ����������߳ߴ�
		if (pMovePartPlacements->size() == 0)
		{
			LogMgr::DebugWarn(_T("we found that no parts were selected in GetCompressVal_of_regionBoundary function!"));
		}
		PartItemPtr pPartItem = pMovePartPlacements->at(0)->GetPartItem();
		LoopCutFeaturePtr pLoopFeature_base;
		if (pPartItem->GetCadData()->GetPatternLoopList()->GetBoundaryLoopEx())
		{
			pLoopFeature_base = pPartItem->GetCamData()->GetBoundaryLoopFeature();
		}
		else
		{
			pLoopFeature_base = pPartItem->GetCamData()->GetLoopFeatureList()->at(0);
		}
		double dToolSize = pLoopFeature_base->GetLoopTool()->GetToolWidth();

		if (bOuterRegion)
		{
			dCompressVal = partAttachConfig.m_dSheetMargin - dToolSize / 2.0;
		}
		else
		{
			dCompressVal = partAttachConfig.m_dPartDis - dToolSize / 2.0;
		}
	}
	else
	{
		if (bOuterRegion)
		{
			dCompressVal = partAttachConfig.m_dSheetMargin - partAttachConfig.m_dPartDis / 2.0;
		}
		else
		{
			dCompressVal = partAttachConfig.m_dPartDis / 2.0;
		}
	}

	return dCompressVal;
}

double PartPlacementMoveBase::GetCompressVal_of_regionBoundary(const PartAttachConfig& partAttachConfig, BOOL bOuterRegion, const PartPlacement* pMovePartPmt)
{
    double dCompressVal = .0;

    if (partAttachConfig.m_bEnableComcut)
    {
        // �õ����ƶ���������ϵ����������߳ߴ�
        LoopCutFeaturePtr pLoopFeature_base;
		if (pMovePartPmt->GetPartItem()->GetCadData()->GetPatternLoopList()->GetBoundaryLoopEx())
		{
			pLoopFeature_base = pMovePartPmt->GetPartItem()->GetCamData()->GetBoundaryLoopFeature();
		}
		else
		{
			pLoopFeature_base = pMovePartPmt->GetPartItem()->GetCamData()->GetLoopFeatureList()->at(0);
		}
        double dToolSize = pLoopFeature_base->GetLoopTool()->GetToolWidth();

        if (bOuterRegion)
        {
            dCompressVal = partAttachConfig.m_dSheetMargin - dToolSize / 2.0;
        }
        else
        {
            dCompressVal = partAttachConfig.m_dPartDis - dToolSize / 2.0;
        }
    }
    else
    {
        if (bOuterRegion)
        {
            dCompressVal = partAttachConfig.m_dSheetMargin - partAttachConfig.m_dPartDis / 2.0;
        }
        else
        {
            dCompressVal = partAttachConfig.m_dPartDis / 2.0;
        }
    }

    return dCompressVal;
}

double PartPlacementMoveBase::GetExpandVal_of_movParts(const PartAttachConfig& partAttachConfig, const PartPlacementList* pMovePartPlacements)
{
	// ���ƶ����������һ�����õ����ϵ����������߳ߴ�
	if (pMovePartPlacements->size() == 0)
	{
		LogMgr::DebugWarn(_T("we found that no parts were selected in GetExpandVal_of_movParts function!"));
	}
	PartItemPtr pPartItem = pMovePartPlacements->at(0)->GetPartItem();
	LoopCutFeaturePtr pLoopFeature_base;
	if (pPartItem->GetCadData()->GetPatternLoopList()->GetBoundaryLoopEx())
	{
		pLoopFeature_base = pPartItem->GetCamData()->GetBoundaryLoopFeature();
	}
	else
	{
		pLoopFeature_base = pPartItem->GetCamData()->GetLoopFeatureList()->at(0);
	}
	double dToolSize = pLoopFeature_base->GetLoopTool()->GetToolWidth();

	double dEnlargeValue = partAttachConfig.m_dPartDis / 2.0;
	if (partAttachConfig.m_bEnableComcut)
	{
		dEnlargeValue = dToolSize / 2.0;
	}

	return dEnlargeValue;
}

BOOL PartPlacementMoveBase::RapidMovePartPmt(IMaterialSizePtr pMaterialSize, const PartAttachConfig& partAttachConfig, PartPlacementListPtr pFixPartPmts, PartInstanceListPtr pFixPartInsts,
									   POSITION_INFO iDir, PartPlacementPtr pMovPartPmt, PartInstanceListPtr pMovPartInsts, Matrix2D& retMoveMat)
{
	BOOL bRet = FALSE;

	// ����������еġ��������򡱣����ǵ���Ӿ��ξ����ڡ��ƶ������������Ӿ��Ρ�
	Rect2D movPartRect = pMovPartPmt->GetRectBox();
	RemnantRegionListPtr pRemRegions1 = PartPlacementManager::CalcRemnantRegion(pMaterialSize, pFixPartInsts, movPartRect.GetWidth(), movPartRect.GetHeight());
	if (pRemRegions1->size() == 0)
		return FALSE;

	// ���˳�һЩ���������򡱡�
	RemnantRegionListPtr pRemRegions2 = FilterRemRegions(pRemRegions1, movPartRect, iDir);
	if (pRemRegions2->size() == 0)
		return FALSE;

	// ÿ�����ƶ������ʵ��������Χ����Σ���Ҫ���͡�
	Polygon2DListPtr pPartOuterPolys(new Polygon2DList);
	LoopInstanceListPtr pMovLoopInsts = LoopInstanceManager::GetOuterLoopInsts(pMovPartInsts);
	for (unsigned int i = 0; i < pMovLoopInsts->size(); i++)
	{
		LoopInstancePtr pMovLoopInst = pMovLoopInsts->at(i);
		Polygon2DPtr pPoly = LoopInstanceManager::LoopInst2Poly(pMovLoopInst);
        double dExpandVal_of_movPart = partAttachConfig.m_dPartDis / 2.0;
        if (partAttachConfig.m_bEnableComcut)
        {
            dExpandVal_of_movPart = pMovLoopInst->GetCacheData()->GetLoopTool()->GetToolWidth() / 2.0;
        }
		Polygon2DProcessor::EnlargePolygon(pPoly, dExpandVal_of_movPart, CUTARC_FACTOR, OPTPOLY_FACTOR);
		pPartOuterPolys->push_back(pPoly);
	}
	if (pPartOuterPolys->size() == 0)
	{
		return FALSE;
	}

	// ����������ƶ���������ƶ������������������ڡ�
	Point2D retPt; // �����ƶ���Ŀ�ĵ㡣
	for (unsigned int i = 0; i < pRemRegions2->size(); i++)
	{
		RemnantRegionPtr pRemRegion = pRemRegions2->at(i);
		Point2D movPt;
        double dCompressVal_of_regionBoundary = GetCompressVal_of_regionBoundary(partAttachConfig, pRemRegion->IsOutRegion(), pMovPartPmt.get());
		if (RapidMovInRegion(partAttachConfig, pRemRegion, dCompressVal_of_regionBoundary, pPartOuterPolys, movPartRect, iDir, movPt))
		{
			if (!bRet)
			{
				retPt = movPt;
				bRet = TRUE;
			}
			else
			{
				if (BetterThan(movPt, retPt, iDir))
					retPt = movPt;
			}
		}
	}

	// �õ��ƶ�����
	if (bRet)
	{
		Vector2D vect(movPartRect.GetCenterPt(), retPt);
		retMoveMat.SetTransfer(vect);
	}

	return bRet;
}

Point2D PartPlacementMoveBase::CheckStickA(Point2D currentPt)
{
	Point2D stickPt;

	// ׼�����ݡ�
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	double dStickDis = pSheetEditParam->GetStickDis();

	// ����������μ��ϡ�����С���������򡱡�
	Rect2D movPartsRect;
	movPartsRect.SetXMin(currentPt.X() - m_dMovPartWidth/2.0);
	movPartsRect.SetXMax(currentPt.X() + m_dMovPartWidth/2.0);
	movPartsRect.SetYMin(currentPt.Y() - m_dMovPartHeight/2.0);
	movPartsRect.SetYMax(currentPt.Y() + m_dMovPartHeight/2.0);
	RemnantRegionPtr pRemRegion = m_pRemnantRegionList->GetContainer(m_pPartOuterPolys, movPartsRect);
	if (!pRemRegion)
		return stickPt;

	// ѡ�е�����͡�����߿򡱵ġ��ڿ����켣����
	Polygon2DPtr pOuterPoly = pRemRegion->GetOuterPoly();
	Polygon2DListPtr pInnerStickPolys = m_innerStickPolys[pOuterPoly->GetPolygonID()];
	if (!pInnerStickPolys)
	{
		double dCompressVal_of_regionBoundary = GetCompressVal_of_regionBoundary(m_partAttachConfig, pRemRegion->IsOutRegion(), m_pMovePartPlacements.get());
		pInnerStickPolys = PartPlacementManager::CalcInnerStickPolys(pRemRegion, m_pNewPartOuterPolys, movPartsRect, dCompressVal_of_regionBoundary);
		if (pInnerStickPolys)
			m_innerStickPolys[pOuterPoly->GetPolygonID()] = pInnerStickPolys;
		else
			return stickPt; // ѡ�е�����޷��ŵ�������߿��ڲ���
	}

	// ���㡰�ƶ������������ĽϽ��ġ������ڿס��ġ��⿿���켣����
	Polygon2DListPtr pOuterStickPolys(new Polygon2DList);
	{
		vector<tuple<Polygon2DPtr, Rect2D, double>> holeRegions = pRemRegion->GetHoleRegions();
		for (unsigned int i = 0; i < holeRegions.size(); i++)
		{
			Polygon2DPtr pPolygon_of_regionHole = get<0>(holeRegions.at(i));
			Rect2D holeRect = get<1>(holeRegions.at(i));
			double dExpandVal_of_regionHole = GetExpandVal_of_regionHole(m_partAttachConfig, get<2>(holeRegions.at(i)));
			if (movPartsRect.DistanceTo(holeRect) < (dExpandVal_of_regionHole * 2.0 + dStickDis)) // ѡ���롰�Ű����Ρ��Ͻ��ġ������ڿס���
			{
				Polygon2DPtr pOuterStickPoly = m_outerStickPolys[pPolygon_of_regionHole->GetPolygonID()];
				if (!pOuterStickPoly)
				{
					pOuterStickPoly = PartPlacementManager::CalcOuterStickPolys(pPolygon_of_regionHole, m_pNewPartOuterPolys, movPartsRect, dExpandVal_of_regionHole);
					m_outerStickPolys[pPolygon_of_regionHole->GetPolygonID()] = pOuterStickPoly;
				}
				pOuterStickPolys->push_back(pOuterStickPoly);
			}
		}
	}

	// �ƶ���������ܺ͡����ϱ߿򡱺͸����ġ������ڿס��ཻ��
	for (unsigned int i = 0; i < pInnerStickPolys->size(); i++)
	{
		if (pInnerStickPolys->at(i)->GetPointPos(currentPt) == OUTSIDE_POLY)
			return stickPt;
	}
	for (unsigned int i = 0; i < pOuterStickPolys->size(); i++)
	{
		if (pOuterStickPolys->at(i)->GetPointPos(currentPt) == INSIDE_POLY)
			return stickPt;
	}

	// ����ǲ��ǿ����ˡ�
	Polygon2DListPtr pStickPolys(new Polygon2DList);
	pStickPolys->insert(pStickPolys->end(), pInnerStickPolys->begin(), pInnerStickPolys->end());
	pStickPolys->insert(pStickPolys->end(), pOuterStickPolys->begin(), pOuterStickPolys->end());
	Point2D closestPt = pStickPolys->GetClosestPt(currentPt);
	double dDis = closestPt.DistanceTo(currentPt);
	if (fabs(dDis) < dStickDis)
	{
		m_pStickRemRegion = pRemRegion;
		stickPt = closestPt;
	}

	return stickPt;
}

Point2D PartPlacementMoveBase::CheckStickB(Point2D currentPt)
{
	Point2D stickPt;

	// ׼�����ݡ�
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	double dStickDis = pSheetEditParam->GetStickDis();
	double dEscapeDis = pSheetEditParam->GetEscapeDis();

	// ѡ�е�����͡�����߿򡱵ġ��ڿ����켣����
	// ע��
	//  1) ����켣�Ѿ�������ˣ�ֱ�Ӵӻ����ȡ���ɡ�
	Polygon2DPtr pOuterPoly = m_pStickRemRegion->GetOuterPoly();
	Polygon2DListPtr pInnerStickPolys = m_innerStickPolys[pOuterPoly->GetPolygonID()];

	// ���㡰��ǰͣ�������(ѡ�е�)������ĽϽ��ġ������ڿס��ġ��⿿���켣����
	Polygon2DListPtr pOuterStickPolys(new Polygon2DList);
	{
		Rect2D stickPartsRect;
		stickPartsRect.SetXMin(m_movCenterPt.X() - m_dMovPartWidth / 2.0);
		stickPartsRect.SetXMax(m_movCenterPt.X() + m_dMovPartWidth / 2.0);
		stickPartsRect.SetYMin(m_movCenterPt.Y() - m_dMovPartHeight / 2.0);
		stickPartsRect.SetYMax(m_movCenterPt.Y() + m_dMovPartHeight / 2.0);
		vector<tuple<Polygon2DPtr, Rect2D, double>> holeRegions = m_pStickRemRegion->GetHoleRegions();
		for (unsigned int i = 0; i < holeRegions.size(); i++)
		{
			Polygon2DPtr pPolygon_of_regionHole = get<0>(holeRegions.at(i));
			Rect2D holeRect = get<1>(holeRegions.at(i));
			double dExpandVal_of_regionHole = GetExpandVal_of_regionHole(m_partAttachConfig, get<2>(holeRegions.at(i)));
			if (stickPartsRect.DistanceTo(holeRect) < (dExpandVal_of_regionHole * 2.0 + dStickDis))
			{
				Polygon2DPtr pOuterStickPoly = m_outerStickPolys[pPolygon_of_regionHole->GetPolygonID()];
				if (!pOuterStickPoly)
				{
					pOuterStickPoly = PartPlacementManager::CalcOuterStickPolys(pPolygon_of_regionHole, m_pNewPartOuterPolys, stickPartsRect, dExpandVal_of_regionHole);
					m_outerStickPolys[pPolygon_of_regionHole->GetPolygonID()] = pOuterStickPoly;
				}
				pOuterStickPolys->push_back(pOuterStickPoly);
			}
		}
	}

	// ��ǰ�����㿿���ġ��ڿ����켣����
	Polygon2DPtr pCloseInnerStickPoly;
	for (unsigned int i = 0; i < pInnerStickPolys->size(); i++)
	{
		Polygon2DPtr pInnerStickPoly = pInnerStickPolys->at(i);
		Point2D pt = pInnerStickPoly->GetClosestPt(m_movCenterPt);
		if (pt.DistanceTo(m_movCenterPt) < dStickDis)
		{
			pCloseInnerStickPoly = pInnerStickPoly;
			break;
		}
	}

	// ��ǰ�����㿿���ġ��⿿���켣��������Щ���⿿���켣���ϲ���һ����
	Polygon2DPtr pCloseOuterStickPoly;
	Polygon2DListPtr pCloseOuterStickPolys(new Polygon2DList);
	for (unsigned int i = 0; i < pOuterStickPolys->size(); i++)
	{
		Polygon2DPtr pOuterStickPoly = pOuterStickPolys->at(i);
		Point2D pt = pOuterStickPoly->GetClosestPt(m_movCenterPt);
		if (pt.DistanceTo(m_movCenterPt) < dStickDis)
			pCloseOuterStickPolys->push_back(pOuterStickPoly);
	}
	if (pCloseOuterStickPolys->size() > 0)
		pCloseOuterStickPoly = Polygon2DProcessor::MergePolygon(pCloseOuterStickPolys, OPTPOLY_FACTOR);

	// �ж�����״̬��
	if (pCloseInnerStickPoly)
	{
		if (pCloseOuterStickPoly)
		{
			if (pCloseInnerStickPoly->IntersectWith(*pCloseOuterStickPoly))
			{
				Line2DListPtr pLineList1 = pCloseInnerStickPoly->GetLineList();
				Line2DListPtr pLineList2 = pCloseOuterStickPoly->GetLineList();
				Polygon2DListPtr pPolys = Polygon2DProcessor::CutOffPolygon(pCloseInnerStickPoly.get(), pCloseOuterStickPoly.get(), pLineList1.get(), pLineList2.get());
				if (pPolys->size() > 0)
				{
					Point2D pt = pPolys->GetClosestPt(currentPt);
					if (pt.DistanceTo(currentPt) > dEscapeDis)
						m_pStickRemRegion.reset(); // ������״̬���ݵ��ˡ�
					else
						stickPt = pt;
				}
				else // �и�����ʧ�ܡ�
				{
					Point2D pt = pCloseInnerStickPoly->GetClosestPt(currentPt);
					if (pt.DistanceTo(currentPt) > dEscapeDis)
						m_pStickRemRegion.reset(); // ������״̬���ݵ��ˡ�
					else
						stickPt = pt;
				}
			}
			else // ��������β��ཻ����û���������������
			{
				Point2D pt = pCloseInnerStickPoly->GetClosestPt(currentPt);
				if (pt.DistanceTo(currentPt) > dEscapeDis)
					m_pStickRemRegion.reset(); // ������״̬���ݵ��ˡ�
				else
					stickPt = pt;
			}
		}
		else
		{
			Point2D pt = pCloseInnerStickPoly->GetClosestPt(currentPt);
			if (pt.DistanceTo(currentPt) > dEscapeDis)
				m_pStickRemRegion.reset(); // ������״̬���ݵ��ˡ�
			else
				stickPt = pt;
		}
	}
	else
	{
		if (pCloseOuterStickPoly)
		{
			Point2D pt = pCloseOuterStickPoly->GetClosestPt(currentPt);
			if (pt.DistanceTo(currentPt) > dEscapeDis)
				m_pStickRemRegion.reset(); // ������״̬���ݵ��ˡ�
			else
				stickPt = pt;
		}
		else
			m_pStickRemRegion.reset(); // ������״̬���ݵ��ˡ�
	}

	return stickPt;
}

BOOL PartPlacementMoveBase::RapidMovInRegion(const PartAttachConfig& partAttachConfig, RemnantRegionPtr pRemRegion, double dCompressVal_of_regionBoundary,
                                       Polygon2DListPtr pPartOuterPolys, const Rect2D& movPartsRect, POSITION_INFO iDir, Point2D& retPt)
{
	// ���ʵ���͡�����߿򡱵ġ��ڿ����켣����
	Polygon2DPtr pOuterPoly = pRemRegion->GetOuterPoly();
	Polygon2DListPtr pInnerStickPolys = PartPlacementManager::CalcInnerStickPolys(pRemRegion, pPartOuterPolys, movPartsRect, dCompressVal_of_regionBoundary);
	if (!pInnerStickPolys)
		return FALSE;
	Polygon2DPtr pInnerNFP_with_outerLoop = pInnerStickPolys->at(0); // ���򻯣�ֻ��һ�����ڿ����켣����

	// �������ʵ���͡����������ڿ׵ġ��⿿���켣���������ཻ�ĺϲ���
	Polygon2DListPtr pNFPs_with_hole(new Polygon2DList);
	{
		vector<pair<Polygon2DPtr, double>> holePolys = FilterHoleRegions(pRemRegion, movPartsRect, iDir); // ���˳�һЩ�������ڿס���
		if (holePolys.size() > 0)
		{
			// ����͡������ڿס��ġ��⿿���켣����
			Polygon2DListPtr pOuterStickPolys(new Polygon2DList);
			for (unsigned int i = 0; i < holePolys.size(); i++)
			{
				Polygon2DPtr pHolePoly = holePolys.at(i).first;
				double dExpandVal_of_regionHole = GetExpandVal_of_regionHole(partAttachConfig, holePolys.at(i).second);
				Polygon2DPtr pOuterStickPoly = PartPlacementManager::CalcOuterStickPolys(pHolePoly, pPartOuterPolys, movPartsRect, dExpandVal_of_regionHole);
				pOuterStickPolys->push_back(pOuterStickPoly);
			}

			// ������Щ���⿿���켣�������ཻ�ķ���һ��
			map<LONGLONG, Rect2D> polygonRects;
			vector<Polygon2DListPtr> outerStickPolys = Polygon2DProcessor::MergePolyEx(pOuterStickPolys, polygonRects);
			for (unsigned int i = 0; i < outerStickPolys.size(); i++)
			{
				Polygon2DPtr pPoly = Polygon2DProcessor::MergePolygon(outerStickPolys.at(i), OPTPOLY_FACTOR);
				pNFPs_with_hole->push_back(pPoly);
			}
		}
	}

	// �������ƶ����������������ϱ߿򡱵ġ������ƶ��㡱�������ƶ����������ܻ�͡������ڿס����棬������Ҫ�жϡ�
	bool bCanStickToOuterLoop = false; // �Ƿ����ͣ�������ϱ߿��ϡ�
	Point2D stickPtToOuterLoop; // ���ϱ߿��ϵ�ͣ���㡣
	Point2D rectCenterPt = movPartsRect.GetCenterPt();
	Point2DListPtr pPts = Polygon2DProcessor::GetCrossPt(pInnerNFP_with_outerLoop, rectCenterPt, iDir);
	for (unsigned int i = 0; i < pPts->size(); i++) // ��Щͣ�����ǰ��մӽ���Զ�ŵģ����ѭ����ֻҪ�ҵ�һ�����ϸ񡱵�ͣ���㣬��ֹͣѭ����
	{
		Point2D tmpStickPt = pPts->at(i);

		// ��ԭ���ĵ㲻���غϡ�
		if (tmpStickPt.IsEqual(rectCenterPt))
		{
			continue;
		}

		// ���ܺ��ڿ׸��档
		bool bIntersect_with_hole = false;
		for (unsigned int j = 0; j < pNFPs_with_hole->size(); j++)
		{
			Polygon2DPtr pNFP_with_holes = pNFPs_with_hole->at(j);
			if (pNFP_with_holes->GetPointPos(tmpStickPt) != OUTSIDE_POLY)
			{
				bIntersect_with_hole = true;
				break;
			}
		}
		if (bIntersect_with_hole)
		{
			continue;
		}

		// ѡ���ͣ���㣬ͬʱֹͣѭ����
		bCanStickToOuterLoop = true;
		stickPtToOuterLoop = tmpStickPt;
		break;
	}

	// �������ƶ������������������ڿס��Ŀ����ƶ��㡣
	bool bCanStickToInnerHole = false; // �Ƿ����ͣ���������ڿ��ϡ�
	Point2D stickPtToInnerHole; // �����ڿ��ϵ�ͣ���㡣
	for (unsigned int m = 0; m < pNFPs_with_hole->size(); m++)
	{
		Polygon2DPtr pNFP_with_hole = pNFPs_with_hole->at(m);
		Point2DListPtr pPts = Polygon2DProcessor::GetCrossPt(pNFP_with_hole, movPartsRect.GetCenterPt(), iDir);
		for (unsigned int i = 0; i < pPts->size(); i++) // ��Щͣ�����ǰ��մӽ���Զ�ŵģ����ѭ����ֻҪ�ҵ�һ�����ϸ񡱵�ͣ���㣬��ֹͣѭ����
		{
			Point2D tmpStickPt = pPts->at(i);

			// ��ԭ���ĵ㲻���غϡ�
			if (tmpStickPt.IsEqual(rectCenterPt))
			{
				continue;
			}

			// ���ܺ����������档
			if (pInnerNFP_with_outerLoop->GetPointPos(tmpStickPt) == OUTSIDE_POLY)
			{
				continue;
			}

			// ѡ���ͣ���㣬ͬʱֹͣѭ�������֮ǰ����ͣ����ϸ���Ƚϡ�
			if (!bCanStickToInnerHole)
			{
				bCanStickToInnerHole = true;
				stickPtToInnerHole = tmpStickPt;
			}
			else
			{
				if (BetterThan(tmpStickPt, stickPtToInnerHole, iDir))
				{
					stickPtToInnerHole = tmpStickPt;
				}
			}
			break;
		}
	}

	// �������Ľ����
	if (!bCanStickToOuterLoop && !bCanStickToInnerHole)
	{
		return FALSE;
	}
	else if (bCanStickToOuterLoop && bCanStickToInnerHole)
	{
		if (BetterThan(stickPtToOuterLoop, stickPtToInnerHole, iDir))
		{
			retPt = stickPtToOuterLoop;
		}
		else
		{
			retPt = stickPtToInnerHole;
		}
	}
	else if (bCanStickToOuterLoop)
	{
		retPt = stickPtToOuterLoop;
	}
	else if (bCanStickToInnerHole)
	{
		retPt = stickPtToInnerHole;
	}

	return TRUE;
}

RemnantRegionListPtr PartPlacementMoveBase::FilterRemRegions(RemnantRegionListPtr pRemRegions, const Rect2D& movPartRect, POSITION_INFO iDir)
{
	RemnantRegionListPtr pRetRemRegions(new RemnantRegionList);

	for (unsigned int i = 0; i < pRemRegions->size(); i++)
	{
		RemnantRegionPtr pRemRegion = pRemRegions->at(i);
		if (movPartRect.CommonAtDir(pRemRegion->GetOuterRect(), iDir))
			pRetRemRegions->push_back(pRemRegion);
	}

	return pRetRemRegions;
}

vector<pair<Polygon2DPtr, double>> PartPlacementMoveBase::FilterHoleRegions(RemnantRegionPtr pRemRegion, const Rect2D& movPartRect, POSITION_INFO iDir)
{
	vector<pair<Polygon2DPtr, double>> holePolys;

	vector<tuple<Polygon2DPtr, Rect2D, double>> holeRegions = pRemRegion->GetHoleRegions();
	for (unsigned int i = 0; i < holeRegions.size(); i++)
	{
		Rect2D holeRect = get<1>(holeRegions.at(i));
		if (movPartRect.CommonAtDir(holeRect, iDir))
		{
			holePolys.push_back(make_pair(get<0>(holeRegions.at(i)), get<2>(holeRegions.at(i))));
		}
	}

	return holePolys;
}

BOOL PartPlacementMoveBase::BetterThan(const Point2D& retPt1, const Point2D& retPt2, POSITION_INFO iDir)
{
	if (iDir == POSITION_DOWN)
	{
		if (retPt1.Y() > retPt2.Y())
			return TRUE;
	}
	else if (iDir == POSITION_UP)
	{
		if (retPt1.Y() < retPt2.Y())
			return TRUE;
	}
	else if (iDir == POSITION_LEFT)
	{
		if (retPt1.X() > retPt2.X())
			return TRUE;
	}
	else if (iDir == POSITION_RIGHT)
	{
		if (retPt1.X() < retPt2.X())
			return TRUE;
	}

	return FALSE;
}

END_CUTLEADER_NAMESPACE()
