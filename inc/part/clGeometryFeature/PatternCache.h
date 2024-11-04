#pragma once

#include "clGeometryFeatureCommon.h"
#include "Rect2D.h"

BEGIN_CUTLEADER_NAMESPACE()

// “图形”的缓存数据。
class ClGeometryFeature_Export PatternCache
{
public:
	PatternCache(void);
	~PatternCache(void);

public: // get/set functions.
	void SetRect(Rect2D patRect) { m_patRect = patRect; }
	const Rect2D& GetRect() const { return m_patRect; }

	void SetBaseGeom(LineArc2DListPtr pGeom2DList) { m_pGeom2DList = pGeom2DList; }
	LineArc2DListPtr GetBaseGeom() { return m_pGeom2DList; }

private:
	// the rect of the pattern.
	Rect2D m_patRect;

	// the base geom of pattern.
	LineArc2DListPtr m_pGeom2DList;
};

END_CUTLEADER_NAMESPACE()
