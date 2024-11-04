#pragma once

#include "clPartLayoutUICommon.h"
#include "Rect2D.h"
#include <vector>
#include <map>

DECLARE_CUTLEADER_CLASS(Polygon2DList)
DECLARE_CUTLEADER_CLASS(Polygon2D)
DECLARE_CUTLEADER_CLASS(IMaterialSize)
DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(RemnantRegion)
DECLARE_CUTLEADER_CLASS(RemnantRegionList)

BEGIN_CUTLEADER_NAMESPACE()

struct PartAttachConfig
{
    PartAttachConfig() {}
    PartAttachConfig(bool bEnableComcut, double dPartDis, double dSheetMargin)
    {
        m_bEnableComcut = bEnableComcut;
        m_dPartDis = dPartDis;
        m_dSheetMargin = dSheetMargin;
    }
    bool m_bEnableComcut;
    double m_dPartDis; // 这个零件距离是界面设置中的，如果开启共边靠碰的话，不能用这个，需要从零件外轮廓中拿刀具尺寸来用。
    double m_dSheetMargin;
};

// 某些动作中需要移动“零件放置”，这些动作需要从这个类继承。
// 注：
//  1) 这个类提供判断“智能靠碰”和“一键靠碰”的功能。“一键靠碰”的功能由几个静态函数提供。
class PartPlacementMoveBase
{
public:
	PartPlacementMoveBase(void);
	~PartPlacementMoveBase(void);

public:
	// 为智能靠碰准备数据。
	void UpdateBeforeMov();

	// 初始化距离。
    void InitPartAttachConfig(const PartAttachConfig& partAttachConfig) { m_partAttachConfig = partAttachConfig; }

	// 得到移动的“零件放置”。
	PartPlacementListPtr GetMovePartPmts() { return m_pMovePartPlacements; }

protected:
	// 移动的过程中更新一些数据。
	void UpdateWhenMov(Point2D currentPt);

	// 按键旋转零件后更新数据。
	void UpdateWhenRotate(BOOL bCCW);

	// 得到“区域内孔”要膨胀的大小。dToolSize是形成该内孔的零件外轮廓上面的刀具尺寸。
	static double GetExpandVal_of_regionHole(const PartAttachConfig& partAttachConfig, double dToolSize);

	// 得到“区域边框”要压缩的大小。
	// 注: 
	//  1) 当启用共边靠碰时，如果移动中的几个零件的刀具尺寸不同，则此时不太好决定“区域边框”应该缩多少，简化问题，那就参考第一个移动的零件的刀具尺寸。
	//  2) 当零件间距大而材料间距小时，返回的值可能是负的。
	static double GetCompressVal_of_regionBoundary(const PartAttachConfig& partAttachConfig, BOOL bOuterRegion, const PartPlacementList* pMovePartPlacements);
    static double GetCompressVal_of_regionBoundary(const PartAttachConfig& partAttachConfig, BOOL bOuterRegion, const PartPlacement* pMovePartPmt);

	// 得到“移动的零件”要膨胀的大小。
	// 注：
	//  1) 选中多个零件时，简单起见，只用一个零件外轮廓的刀具尺寸来膨胀。
	static double GetExpandVal_of_movParts(const PartAttachConfig& partAttachConfig, const PartPlacementList* pMovePartPlacements);

	// 把零件放置沿着一个方向做快速移动。
	static BOOL RapidMovePartPmt(IMaterialSizePtr pMaterialSize, const PartAttachConfig& partAttachConfig, PartPlacementListPtr pFixPartPmts, PartInstanceListPtr pFixPartInsts,
						         POSITION_INFO iDir, PartPlacementPtr pMovPartPmt, PartInstanceListPtr pMovPartInsts, Matrix2D& retMoveMat);

private:
	// 当前没有靠碰时检查“排版多边形”的靠碰。
	Point2D CheckStickA(Point2D currentPt);

	// 当前有靠碰时检查“排版多边形”的靠碰。
	Point2D CheckStickB(Point2D currentPt);

    /************************************************************************/
    // 一键靠碰相关。

	// 把零件按照移动方向快速移动到“余料区域”内。
	// 注：
	//  1) 零件不能和“区域边框”和“区域内孔”重合。
	//  2) 成功的话返回移动到的位置点，这个点相对movPartRect。
	static BOOL RapidMovInRegion(const PartAttachConfig& partAttachConfig, RemnantRegionPtr pRemRegion, double dCompressVal_of_regionBoundary, Polygon2DListPtr pPartOuterPolys,
                                 const Rect2D& movPartsRect, POSITION_INFO iDir, Point2D& retPt);

	// 过滤出一些“余料区域”。
	static RemnantRegionListPtr FilterRemRegions(RemnantRegionListPtr pRemRegions, const Rect2D& movPartRect, POSITION_INFO iDir);

	// 过滤出“余料区域”的一些“区域内孔”。
	static std::vector<std::pair<Polygon2DPtr, double>> FilterHoleRegions(RemnantRegionPtr pRemRegion, const Rect2D& movPartRect, POSITION_INFO iDir);

	// 结果点pt1是不是比pt2好。
	// 注：
	//  1) 如果向下快速移动，高的较好。
	static BOOL BetterThan(const Point2D& retPt1, const Point2D& retPt2, POSITION_INFO iDir);
    /************************************************************************/

protected:
	/************************************************************************/
	// 为智能靠碰准备数据。

	// 材料。
	IMaterialSizePtr m_pMaterialSize;

	// 不动的零件放置对应的零件实例。
	PartInstanceListPtr m_pFixPartInstances;

	// 用来移动的“零件放置”集合。
	// 注：
	//  1) 在选中零件准备移动前初始化这个变量，这是一份克隆出来的。
	PartPlacementListPtr m_pMovePartPlacements;

	// 这些数据来自配置信息，在浮动面板中可以改这几个数据。
    PartAttachConfig m_partAttachConfig;
	/************************************************************************/

	// “移动的零件实例”的外接矩形的尺寸。
	// 注：
	//  1) 在选中零件准备移动前更新这个变量。
	double m_dMovPartWidth;
	double m_dMovPartHeight;

	// “移动的零件放置”的中心点。
	// 注：
	//  1) 零件被吸附住时，这个点和当前鼠标位置不一定一样。
	Point2D m_movCenterPt;

	// 缓存选中零件与“区域内孔”的“外靠碰轨迹”。
	// 注：
	//  1) 在准备移动前需要重设这个变量，移动时可能要更新这个变量。
	//  2) 在轨迹上移动时就能保证零件都在对应的“区域内孔”外，且附着着边框运动。
	std::map<LONGLONG, Polygon2DPtr> m_outerStickPolys;

	// 一些“余料区域”，它们的外接矩形尺寸大于“移动的零件实例”的外接矩形尺寸。
	// 注：
	//  1) 在准备移动前初始化这个变量，移动时无需更新。
	RemnantRegionListPtr m_pRemnantRegionList;

	// 靠碰到这个“余料区域”中的多边形。
	RemnantRegionPtr m_pStickRemRegion;

private:
	/************************************************************************/
	// “移动中的零件”的一些信息。

	// 每个“移动的零件实例”的外围多边形。
	// 注：
	//  1) 鼠标移动时需要更新这个变量。
	Polygon2DListPtr m_pPartOuterPolys;

	// 每个“移动的零件实例”的外围多边形，被膨胀了半个零件距离。
	// 注：
	//  1) 鼠标移动时需要更新这个变量。
	Polygon2DListPtr m_pNewPartOuterPolys;
	/************************************************************************/

	// 缓存选中零件与“区域边框”的“内靠碰轨迹”。
	// 注：
	//  1) 在准备移动前需要重设这个变量，移动时可能要更新这个变量。
	//  2) 在轨迹上移动时就能保证零件都在对应的“区域边框”内，且附着着边框运动。
	std::map<LONGLONG, Polygon2DListPtr> m_innerStickPolys;
};

END_CUTLEADER_NAMESPACE()
