#pragma once

#include "clPartCommon.h"
#include "ClData.h"
#include "Rect2D.h"

DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PatternLoopList)

BEGIN_CUTLEADER_NAMESPACE()

// this class define the cad data of the part.
// notes:
//   1) do not save this object to database.
class ClPart_Export PartCadData : public ClData
{
public:
	PartCadData(void);
	PartCadData(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList);
	virtual ~PartCadData(void);

public: // implement IData interface.
	virtual BOOL IsModified() const override;
    virtual LONGLONG GetLatestModifyTimeStamp() const override;

public:
	PatternListPtr GetPatternList() const { return m_pPatList; }
	void SetPatternList(PatternListPtr pPatList) { m_pPatList = pPatList; }

	PatternLoopListPtr GetPatternLoopList() const { return m_pPatternLoopList; }
	void SetPatternLoopList(PatternLoopListPtr pPatternLoopList) { m_pPatternLoopList = pPatternLoopList; }

public:
	// 得到从原点到图形左下点的向量。
	virtual Vector2D GetVectToOrigin() const;

private:
	// the patterns in the part
	PatternListPtr m_pPatList;

	// the pattern loop list.
	PatternLoopListPtr m_pPatternLoopList;
};

END_CUTLEADER_NAMESPACE()
