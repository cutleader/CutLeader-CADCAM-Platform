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
	// “移动的零件实例”的外接矩形的尺寸。
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

	// 每个“移动的零件实例”的外围多边形。
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
        if (pPolyList->size() == 1) // 此处需要加上保护，要不然可能崩溃
        {
            Polygon2DPtr pPoly = pPolyList->at(0);
            Polygon2DProcessor::SleekPolygon(pPoly, OPTPOLY_FACTOR);

            // 膨胀
            Polygon2DPtr pNewPoly = pPoly->Clone();
            Polygon2DProcessor::EnlargePolygon(pNewPoly, dExpandVal_of_movParts, CUTARC_FACTOR, OPTPOLY_FACTOR);

            m_pPartOuterPolys->push_back(pPoly);
            m_pNewPartOuterPolys->push_back(pNewPoly);
        }
        else
        {
            CString strPartName = pMovLoopInst->GetPartInstance()->GetPartPlacement()->GetPartItem()->GetPartName();
            CString strMsg;
            strMsg.Format(_T("Debug warning 1 in PartPlacementMoveBase::UpdateBeforeMov function! Part name: %s 。这不是严重错误，可继续！"), strPartName);
            LogMessagePtr pLogMsg(new LogMessage(DateHelper::GetCurrentTime(), LOG_MSG_WARNING, _T("生成多边形集合"), strMsg));
            LogMgr::WriteLog(pLogMsg);
        }
	}

	// 板材上除掉“不动的零件放置”留出来的余料区域。
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
		// 移动时的变换矩阵。
		Matrix2D mat;
		Vector2D vect(m_movCenterPt, currentPt);
		mat.SetTransfer(vect);

		// 更新“移动中的零件放置”。
		for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
			m_pMovePartPlacements->at(i)->Transform(mat);

		// 更新“移动的零件放置”的中心点。
		m_movCenterPt = currentPt;
	}
	else
	{
		if (!m_pStickRemRegion) // 此时没有被吸附。
		{
			// 移动时的变换矩阵。
			Matrix2D mat;
			Vector2D vect(m_movCenterPt, currentPt);
			mat.SetTransfer(vect);

			// 更新“移动中的零件放置”。
			for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
				m_pMovePartPlacements->at(i)->Transform(mat);

			// 更新“移动的零件实例”的外围多边形。
			for (unsigned int i = 0; i < m_pPartOuterPolys->size(); i++)
				m_pPartOuterPolys->at(i)->Transform(mat);
			for (unsigned int i = 0; i < m_pNewPartOuterPolys->size(); i++)
				m_pNewPartOuterPolys->at(i)->Transform(mat);

			// 检查靠碰。
			Point2D stickPt = CheckStickA(currentPt);
			if (m_pStickRemRegion) // 碰住了。
			{
				// 移动时的变换矩阵。
				Matrix2D mat;
				Vector2D vect(currentPt, stickPt);
				mat.SetTransfer(vect);

				// 更新“移动中的零件放置”。
				for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
					m_pMovePartPlacements->at(i)->Transform(mat);

				// 更新“移动的零件实例”的外围多边形。
				for (unsigned int i = 0; i < m_pPartOuterPolys->size(); i++)
					m_pPartOuterPolys->at(i)->Transform(mat);
				for (unsigned int i = 0; i < m_pNewPartOuterPolys->size(); i++)
					m_pNewPartOuterPolys->at(i)->Transform(mat);

				// 更新“移动的零件放置”的中心点。
				m_movCenterPt = stickPt;
			}
			else if (!m_pStickRemRegion) // 没碰住。
			{
				m_movCenterPt = currentPt;
			}
		}
		else if (m_pStickRemRegion) // 此时有被吸附。
		{
			// 检查靠碰。
			Point2D stickPt = CheckStickB(currentPt);
			if (m_pStickRemRegion) // 仍然碰住了。
			{
				// 移动时的变换矩阵。
				Matrix2D mat;
				Vector2D vect(m_movCenterPt, stickPt);
				mat.SetTransfer(vect);

				// 更新“移动中的零件放置”。
				for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
					m_pMovePartPlacements->at(i)->Transform(mat);

				// 更新“移动的零件实例”的外围多边形。
				for (unsigned int i = 0; i < m_pPartOuterPolys->size(); i++)
					m_pPartOuterPolys->at(i)->Transform(mat);
				for (unsigned int i = 0; i < m_pNewPartOuterPolys->size(); i++)
					m_pNewPartOuterPolys->at(i)->Transform(mat);

				// 更新“移动的零件放置”的中心点。
				m_movCenterPt = stickPt;
			}
			else
			{
				// 移动时的变换矩阵。
				Matrix2D mat;
				Vector2D vect(m_movCenterPt, currentPt);
				mat.SetTransfer(vect);

				// 更新“移动中的零件放置”。
				for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
					m_pMovePartPlacements->at(i)->Transform(mat);

				// 更新“移动的零件实例”的外围多边形。
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

	// 旋转移动中的“零件放置”。
	Rect2D rect = m_pMovePartPlacements->GetRectBox();
	Matrix2D mat;
	if (bCCW)
		mat.RotateBy(rect.GetCenterPt(), pSheetEditParam->GetRotateStep()*D2R);
	else
		mat.RotateBy(rect.GetCenterPt(), -pSheetEditParam->GetRotateStep()*D2R);
	for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
		m_pMovePartPlacements->at(i)->Transform(mat);

	// 把旋转后零件的中心点校对到鼠标点。
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

	// 是否要为智能靠碰准备数据。
	if (pSheetEditParam->EnableDisDetect())
	{
		// “移动的零件实例”的外接矩形的尺寸。
		m_dMovPartWidth = movRect.GetWidth();
		m_dMovPartHeight = movRect.GetHeight();

		// 每个“移动的零件实例”的外围多边形。
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

			// 膨胀
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
		// 从移动的零件中挑一个，得到其上的外轮廓刀具尺寸
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
        // 得到“移动的零件”上的外轮廓刀具尺寸
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
	// 从移动的零件中挑一个，得到其上的外轮廓刀具尺寸
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

	// 板材上面所有的“余料区域”，它们的外接矩形均大于“移动的零件”的外接矩形。
	Rect2D movPartRect = pMovPartPmt->GetRectBox();
	RemnantRegionListPtr pRemRegions1 = PartPlacementManager::CalcRemnantRegion(pMaterialSize, pFixPartInsts, movPartRect.GetWidth(), movPartRect.GetHeight());
	if (pRemRegions1->size() == 0)
		return FALSE;

	// 过滤出一些“余料区域”。
	RemnantRegionListPtr pRemRegions2 = FilterRemRegions(pRemRegions1, movPartRect, iDir);
	if (pRemRegions2->size() == 0)
		return FALSE;

	// 每个“移动的零件实例”的外围多边形，需要膨胀。
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

	// 把零件按照移动方向快速移动到各个“余料区域”内。
	Point2D retPt; // 快速移动的目的点。
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

	// 得到移动矩阵。
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

	// 准备数据。
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	double dStickDis = pSheetEditParam->GetStickDis();

	// 包含“多边形集合”的最小“余料区域”。
	Rect2D movPartsRect;
	movPartsRect.SetXMin(currentPt.X() - m_dMovPartWidth/2.0);
	movPartsRect.SetXMax(currentPt.X() + m_dMovPartWidth/2.0);
	movPartsRect.SetYMin(currentPt.Y() - m_dMovPartHeight/2.0);
	movPartsRect.SetYMax(currentPt.Y() + m_dMovPartHeight/2.0);
	RemnantRegionPtr pRemRegion = m_pRemnantRegionList->GetContainer(m_pPartOuterPolys, movPartsRect);
	if (!pRemRegion)
		return stickPt;

	// 选中的零件和“区域边框”的“内靠碰轨迹”。
	Polygon2DPtr pOuterPoly = pRemRegion->GetOuterPoly();
	Polygon2DListPtr pInnerStickPolys = m_innerStickPolys[pOuterPoly->GetPolygonID()];
	if (!pInnerStickPolys)
	{
		double dCompressVal_of_regionBoundary = GetCompressVal_of_regionBoundary(m_partAttachConfig, pRemRegion->IsOutRegion(), m_pMovePartPlacements.get());
		pInnerStickPolys = PartPlacementManager::CalcInnerStickPolys(pRemRegion, m_pNewPartOuterPolys, movPartsRect, dCompressVal_of_regionBoundary);
		if (pInnerStickPolys)
			m_innerStickPolys[pOuterPoly->GetPolygonID()] = pInnerStickPolys;
		else
			return stickPt; // 选中的零件无法放到“区域边框”内部。
	}

	// 计算“移动的零件”和离的较近的“区域内孔”的“外靠碰轨迹”。
	Polygon2DListPtr pOuterStickPolys(new Polygon2DList);
	{
		vector<tuple<Polygon2DPtr, Rect2D, double>> holeRegions = pRemRegion->GetHoleRegions();
		for (unsigned int i = 0; i < holeRegions.size(); i++)
		{
			Polygon2DPtr pPolygon_of_regionHole = get<0>(holeRegions.at(i));
			Rect2D holeRect = get<1>(holeRegions.at(i));
			double dExpandVal_of_regionHole = GetExpandVal_of_regionHole(m_partAttachConfig, get<2>(holeRegions.at(i)));
			if (movPartsRect.DistanceTo(holeRect) < (dExpandVal_of_regionHole * 2.0 + dStickDis)) // 选出离“排版多边形”较近的“区域内孔”。
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

	// 移动的零件不能和“余料边框”和附近的“余料内孔”相交。
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

	// 检查是不是靠碰了。
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

	// 准备数据。
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	double dStickDis = pSheetEditParam->GetStickDis();
	double dEscapeDis = pSheetEditParam->GetEscapeDis();

	// 选中的零件和“区域边框”的“内靠碰轨迹”。
	// 注：
	//  1) 这个轨迹已经计算过了，直接从缓存获取即可。
	Polygon2DPtr pOuterPoly = m_pStickRemRegion->GetOuterPoly();
	Polygon2DListPtr pInnerStickPolys = m_innerStickPolys[pOuterPoly->GetPolygonID()];

	// 计算“当前停靠的零件(选中的)”和离的较近的“区域内孔”的“外靠碰轨迹”。
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

	// 当前吸附点靠近的“内靠碰轨迹”。
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

	// 当前吸附点靠近的“外靠碰轨迹”，把这些“外靠碰轨迹”合并成一个。
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

	// 判断吸附状态。
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
						m_pStickRemRegion.reset(); // 从吸附状态逃逸掉了。
					else
						stickPt = pt;
				}
				else // 切割多边形失败。
				{
					Point2D pt = pCloseInnerStickPoly->GetClosestPt(currentPt);
					if (pt.DistanceTo(currentPt) > dEscapeDis)
						m_pStickRemRegion.reset(); // 从吸附状态逃逸掉了。
					else
						stickPt = pt;
				}
			}
			else // 两个多边形不相交（还没碰到这种情况）。
			{
				Point2D pt = pCloseInnerStickPoly->GetClosestPt(currentPt);
				if (pt.DistanceTo(currentPt) > dEscapeDis)
					m_pStickRemRegion.reset(); // 从吸附状态逃逸掉了。
				else
					stickPt = pt;
			}
		}
		else
		{
			Point2D pt = pCloseInnerStickPoly->GetClosestPt(currentPt);
			if (pt.DistanceTo(currentPt) > dEscapeDis)
				m_pStickRemRegion.reset(); // 从吸附状态逃逸掉了。
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
				m_pStickRemRegion.reset(); // 从吸附状态逃逸掉了。
			else
				stickPt = pt;
		}
		else
			m_pStickRemRegion.reset(); // 从吸附状态逃逸掉了。
	}

	return stickPt;
}

BOOL PartPlacementMoveBase::RapidMovInRegion(const PartAttachConfig& partAttachConfig, RemnantRegionPtr pRemRegion, double dCompressVal_of_regionBoundary,
                                       Polygon2DListPtr pPartOuterPolys, const Rect2D& movPartsRect, POSITION_INFO iDir, Point2D& retPt)
{
	// 零件实例和“区域边框”的“内靠碰轨迹”。
	Polygon2DPtr pOuterPoly = pRemRegion->GetOuterPoly();
	Polygon2DListPtr pInnerStickPolys = PartPlacementManager::CalcInnerStickPolys(pRemRegion, pPartOuterPolys, movPartsRect, dCompressVal_of_regionBoundary);
	if (!pInnerStickPolys)
		return FALSE;
	Polygon2DPtr pInnerNFP_with_outerLoop = pInnerStickPolys->at(0); // 做简化，只拿一个“内靠碰轨迹”。

	// 计算零件实例和“余料区域”内孔的“外靠碰轨迹”，并把相交的合并。
	Polygon2DListPtr pNFPs_with_hole(new Polygon2DList);
	{
		vector<pair<Polygon2DPtr, double>> holePolys = FilterHoleRegions(pRemRegion, movPartsRect, iDir); // 过滤出一些“区域内孔”。
		if (holePolys.size() > 0)
		{
			// 零件和“区域内孔”的“外靠碰轨迹”。
			Polygon2DListPtr pOuterStickPolys(new Polygon2DList);
			for (unsigned int i = 0; i < holePolys.size(); i++)
			{
				Polygon2DPtr pHolePoly = holePolys.at(i).first;
				double dExpandVal_of_regionHole = GetExpandVal_of_regionHole(partAttachConfig, holePolys.at(i).second);
				Polygon2DPtr pOuterStickPoly = PartPlacementManager::CalcOuterStickPolys(pHolePoly, pPartOuterPolys, movPartsRect, dExpandVal_of_regionHole);
				pOuterStickPolys->push_back(pOuterStickPoly);
			}

			// 整理这些“外靠碰轨迹”，把相交的放在一起。
			map<LONGLONG, Rect2D> polygonRects;
			vector<Polygon2DListPtr> outerStickPolys = Polygon2DProcessor::MergePolyEx(pOuterStickPolys, polygonRects);
			for (unsigned int i = 0; i < outerStickPolys.size(); i++)
			{
				Polygon2DPtr pPoly = Polygon2DProcessor::MergePolygon(outerStickPolys.at(i), OPTPOLY_FACTOR);
				pNFPs_with_hole->push_back(pPoly);
			}
		}
	}

	// 计算在移动方向上碰到“余料边框”的“快速移动点”，不过移动到这个点可能会和“余料内孔”干涉，这里需要判断。
	bool bCanStickToOuterLoop = false; // 是否可以停靠到余料边框上。
	Point2D stickPtToOuterLoop; // 余料边框上的停靠点。
	Point2D rectCenterPt = movPartsRect.GetCenterPt();
	Point2DListPtr pPts = Polygon2DProcessor::GetCrossPt(pInnerNFP_with_outerLoop, rectCenterPt, iDir);
	for (unsigned int i = 0; i < pPts->size(); i++) // 这些停靠点是按照从近到远排的，这个循环中只要找到一个“合格”的停靠点，就停止循环。
	{
		Point2D tmpStickPt = pPts->at(i);

		// 和原来的点不能重合。
		if (tmpStickPt.IsEqual(rectCenterPt))
		{
			continue;
		}

		// 不能和内孔干涉。
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

		// 选这个停靠点，同时停止循环。
		bCanStickToOuterLoop = true;
		stickPtToOuterLoop = tmpStickPt;
		break;
	}

	// 计算在移动方向上碰到“余料内孔”的快速移动点。
	bool bCanStickToInnerHole = false; // 是否可以停靠到余料内孔上。
	Point2D stickPtToInnerHole; // 余料内孔上的停靠点。
	for (unsigned int m = 0; m < pNFPs_with_hole->size(); m++)
	{
		Polygon2DPtr pNFP_with_hole = pNFPs_with_hole->at(m);
		Point2DListPtr pPts = Polygon2DProcessor::GetCrossPt(pNFP_with_hole, movPartsRect.GetCenterPt(), iDir);
		for (unsigned int i = 0; i < pPts->size(); i++) // 这些停靠点是按照从近到远排的，这个循环中只要找到一个“合格”的停靠点，就停止循环。
		{
			Point2D tmpStickPt = pPts->at(i);

			// 和原来的点不能重合。
			if (tmpStickPt.IsEqual(rectCenterPt))
			{
				continue;
			}

			// 不能和外轮廓干涉。
			if (pInnerNFP_with_outerLoop->GetPointPos(tmpStickPt) == OUTSIDE_POLY)
			{
				continue;
			}

			// 选这个停靠点，同时停止循环。如果之前已有停靠点合格，需比较。
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

	// 决定最后的结果。
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
