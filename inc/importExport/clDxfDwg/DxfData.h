#pragma once

#include "clDxfDwgCommon.h"

DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(PatternList)

BEGIN_CUTLEADER_NAMESPACE()

// this class defines all data which imported from dxf/dwg file.
class ClDxfDwg_Export DxfData
{
public:
	DxfData(void);
	~DxfData(void);

public: // get/set functions.
	void SetPatternList(PatternListPtr pPatList) { m_pPatList = pPatList; }
	PatternListPtr GetPatternList() const { return m_pPatList; }

    void SetPolygonPatList(PatternListPtr pPatList_polygon) { m_pPatList_polygon = pPatList_polygon; }
    PatternListPtr GetPolygonPatList() const { return m_pPatList_polygon; }

public:
	// update the cache data of PatternList object.
	void UpdateCache();

	// calc the base geoms.
	LineArc2DListPtr Calculate2DLineArcs() const;

private:
	// 除了多边形pattern以外的所有pattern
	PatternListPtr m_pPatList;

    // 多边形pattern，每个多段线\样条曲线都生成一个多边形pattern
    PatternListPtr m_pPatList_polygon;
};

END_CUTLEADER_NAMESPACE()
