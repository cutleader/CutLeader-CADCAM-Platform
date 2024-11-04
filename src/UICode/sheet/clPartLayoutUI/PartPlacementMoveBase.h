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
    double m_dPartDis; // �����������ǽ��������еģ�����������߿����Ļ����������������Ҫ��������������õ��߳ߴ����á�
    double m_dSheetMargin;
};

// ĳЩ��������Ҫ�ƶ���������á�����Щ������Ҫ�������̳С�
// ע��
//  1) ������ṩ�жϡ����ܿ������͡�һ���������Ĺ��ܡ���һ���������Ĺ����ɼ�����̬�����ṩ��
class PartPlacementMoveBase
{
public:
	PartPlacementMoveBase(void);
	~PartPlacementMoveBase(void);

public:
	// Ϊ���ܿ���׼�����ݡ�
	void UpdateBeforeMov();

	// ��ʼ�����롣
    void InitPartAttachConfig(const PartAttachConfig& partAttachConfig) { m_partAttachConfig = partAttachConfig; }

	// �õ��ƶ��ġ�������á���
	PartPlacementListPtr GetMovePartPmts() { return m_pMovePartPlacements; }

protected:
	// �ƶ��Ĺ����и���һЩ���ݡ�
	void UpdateWhenMov(Point2D currentPt);

	// ������ת�����������ݡ�
	void UpdateWhenRotate(BOOL bCCW);

	// �õ��������ڿס�Ҫ���͵Ĵ�С��dToolSize���γɸ��ڿ׵��������������ĵ��߳ߴ硣
	static double GetExpandVal_of_regionHole(const PartAttachConfig& partAttachConfig, double dToolSize);

	// �õ�������߿�Ҫѹ���Ĵ�С��
	// ע: 
	//  1) �����ù��߿���ʱ������ƶ��еļ�������ĵ��߳ߴ粻ͬ�����ʱ��̫�þ���������߿�Ӧ�������٣������⣬�ǾͲο���һ���ƶ�������ĵ��߳ߴ硣
	//  2) �������������ϼ��Сʱ�����ص�ֵ�����Ǹ��ġ�
	static double GetCompressVal_of_regionBoundary(const PartAttachConfig& partAttachConfig, BOOL bOuterRegion, const PartPlacementList* pMovePartPlacements);
    static double GetCompressVal_of_regionBoundary(const PartAttachConfig& partAttachConfig, BOOL bOuterRegion, const PartPlacement* pMovePartPmt);

	// �õ����ƶ��������Ҫ���͵Ĵ�С��
	// ע��
	//  1) ѡ�ж�����ʱ���������ֻ��һ������������ĵ��߳ߴ������͡�
	static double GetExpandVal_of_movParts(const PartAttachConfig& partAttachConfig, const PartPlacementList* pMovePartPlacements);

	// �������������һ�������������ƶ���
	static BOOL RapidMovePartPmt(IMaterialSizePtr pMaterialSize, const PartAttachConfig& partAttachConfig, PartPlacementListPtr pFixPartPmts, PartInstanceListPtr pFixPartInsts,
						         POSITION_INFO iDir, PartPlacementPtr pMovPartPmt, PartInstanceListPtr pMovPartInsts, Matrix2D& retMoveMat);

private:
	// ��ǰû�п���ʱ��顰�Ű����Ρ��Ŀ�����
	Point2D CheckStickA(Point2D currentPt);

	// ��ǰ�п���ʱ��顰�Ű����Ρ��Ŀ�����
	Point2D CheckStickB(Point2D currentPt);

    /************************************************************************/
    // һ��������ء�

	// ����������ƶ���������ƶ��������������ڡ�
	// ע��
	//  1) ������ܺ͡�����߿򡱺͡������ڿס��غϡ�
	//  2) �ɹ��Ļ������ƶ�����λ�õ㣬��������movPartRect��
	static BOOL RapidMovInRegion(const PartAttachConfig& partAttachConfig, RemnantRegionPtr pRemRegion, double dCompressVal_of_regionBoundary, Polygon2DListPtr pPartOuterPolys,
                                 const Rect2D& movPartsRect, POSITION_INFO iDir, Point2D& retPt);

	// ���˳�һЩ���������򡱡�
	static RemnantRegionListPtr FilterRemRegions(RemnantRegionListPtr pRemRegions, const Rect2D& movPartRect, POSITION_INFO iDir);

	// ���˳����������򡱵�һЩ�������ڿס���
	static std::vector<std::pair<Polygon2DPtr, double>> FilterHoleRegions(RemnantRegionPtr pRemRegion, const Rect2D& movPartRect, POSITION_INFO iDir);

	// �����pt1�ǲ��Ǳ�pt2�á�
	// ע��
	//  1) ������¿����ƶ����ߵĽϺá�
	static BOOL BetterThan(const Point2D& retPt1, const Point2D& retPt2, POSITION_INFO iDir);
    /************************************************************************/

protected:
	/************************************************************************/
	// Ϊ���ܿ���׼�����ݡ�

	// ���ϡ�
	IMaterialSizePtr m_pMaterialSize;

	// ������������ö�Ӧ�����ʵ����
	PartInstanceListPtr m_pFixPartInstances;

	// �����ƶ��ġ�������á����ϡ�
	// ע��
	//  1) ��ѡ�����׼���ƶ�ǰ��ʼ���������������һ�ݿ�¡�����ġ�
	PartPlacementListPtr m_pMovePartPlacements;

	// ��Щ��������������Ϣ���ڸ�������п��Ը��⼸�����ݡ�
    PartAttachConfig m_partAttachConfig;
	/************************************************************************/

	// ���ƶ������ʵ��������Ӿ��εĳߴ硣
	// ע��
	//  1) ��ѡ�����׼���ƶ�ǰ�������������
	double m_dMovPartWidth;
	double m_dMovPartHeight;

	// ���ƶ���������á������ĵ㡣
	// ע��
	//  1) ���������סʱ�������͵�ǰ���λ�ò�һ��һ����
	Point2D m_movCenterPt;

	// ����ѡ������롰�����ڿס��ġ��⿿���켣����
	// ע��
	//  1) ��׼���ƶ�ǰ��Ҫ��������������ƶ�ʱ����Ҫ�������������
	//  2) �ڹ켣���ƶ�ʱ���ܱ�֤������ڶ�Ӧ�ġ������ڿס��⣬�Ҹ����ű߿��˶���
	std::map<LONGLONG, Polygon2DPtr> m_outerStickPolys;

	// һЩ���������򡱣����ǵ���Ӿ��γߴ���ڡ��ƶ������ʵ��������Ӿ��γߴ硣
	// ע��
	//  1) ��׼���ƶ�ǰ��ʼ������������ƶ�ʱ������¡�
	RemnantRegionListPtr m_pRemnantRegionList;

	// ��������������������еĶ���Ρ�
	RemnantRegionPtr m_pStickRemRegion;

private:
	/************************************************************************/
	// ���ƶ��е��������һЩ��Ϣ��

	// ÿ�����ƶ������ʵ��������Χ����Ρ�
	// ע��
	//  1) ����ƶ�ʱ��Ҫ�������������
	Polygon2DListPtr m_pPartOuterPolys;

	// ÿ�����ƶ������ʵ��������Χ����Σ��������˰��������롣
	// ע��
	//  1) ����ƶ�ʱ��Ҫ�������������
	Polygon2DListPtr m_pNewPartOuterPolys;
	/************************************************************************/

	// ����ѡ������롰����߿򡱵ġ��ڿ����켣����
	// ע��
	//  1) ��׼���ƶ�ǰ��Ҫ��������������ƶ�ʱ����Ҫ�������������
	//  2) �ڹ켣���ƶ�ʱ���ܱ�֤������ڶ�Ӧ�ġ�����߿��ڣ��Ҹ����ű߿��˶���
	std::map<LONGLONG, Polygon2DListPtr> m_innerStickPolys;
};

END_CUTLEADER_NAMESPACE()
