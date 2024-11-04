#pragma once

#include "ClData.h"
#include "IPatternLoop.h"

BEGIN_CUTLEADER_NAMESPACE()

// “图形回路”的基本实现。
class ClGeometryFeature_Export PatternLoopBase : public ClData,
									   public IPatternLoop
{
public:
	PatternLoopBase();
	virtual ~PatternLoopBase();

public: // implement IData interface.
	virtual BOOL IsModified() const override;

public: // implement interface IPatternLoop.
	virtual PatternListPtr GetPatternList() const override;
	virtual IPatternPtr GetPatternByLoopNodeIndex(int iIndex, BOOL& bLoopDirection) const override;
	virtual IPatternPtr GetPatternByLoopNodeIndex(int iIndex) const override;
	virtual IPatternPtr GetPatternByID(LONGLONG iPatternID, BOOL& bLoopDirection) const override;
	virtual IPatternPtr GetPatternByID(LONGLONG iPatternID) const override;
	virtual int GetLoopNodeIndexByPatternID(LONGLONG iPatternID) const override;
	virtual PatternLoopNodeListPtr GetPatternLoopNodes() const override { return m_pLoopNodeList; }
	virtual void GetClosestCrossPt(const Point2D& basePt, double dAngle, const Matrix2D& mat, Point2D& crossPt, int& iIndex) const override;
	virtual void GetClosestPt(const Point2D& pt, Point2D& closePt, int& iIndex, const Matrix2D& mat) const override;
	virtual BOOL IsBoundaryLoop() const override { return m_bBoundary; }
	virtual void IsBoundaryLoop(BOOL bBoundary) override { m_bBoundary = bBoundary; }
	virtual BOOL FromPatList(const PatternList* pPatList) const override;
	virtual void SetCacheData(PatternLoopCachePtr pPatternLoopCache) override { m_pPatternLoopCache = pPatternLoopCache; }
	virtual PatternLoopCachePtr GetCacheData() const override { return m_pPatternLoopCache; }

protected:
	// the pattern nodes in the pattern loop.
	// notes:
	//   1) actually for PatternLoop_Std and PatternLoop_Array, there are only one
	//      pattern node in the loop, but still define a list from them, so
	//      we can define some common interfaces.
	PatternLoopNodeListPtr m_pLoopNodeList;

	// whether the loop is the boundary loop.
	// notes:
	//   1) this variable is not saved in db, when load loops from db, we will calculate the boundary loop.
	BOOL m_bBoundary;

	// the cache data of the pattern loop.
	PatternLoopCachePtr m_pPatternLoopCache;
};

END_CUTLEADER_NAMESPACE()
