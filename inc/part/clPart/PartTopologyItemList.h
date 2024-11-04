#pragma once

#include "clPartCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(PartTopologyItem)
DECLARE_CUTLEADER_CLASS(PatternLoopList)

BEGIN_CUTLEADER_NAMESPACE()

// 零件拓扑项集合。
class ClPart_Export PartTopologyItemList : public std::vector<PartTopologyItemPtr>
{
public:
	PartTopologyItemList(void);
	~PartTopologyItemList(void);

public:
	// 递归得到所有零件。
	// 注：
	//   1) 一个"PatternLoopListPtr"对象代表一个零件，这个函数要设置哪个回路为外围回路。
	void GetAllPart(std::vector<PatternLoopListPtr>& partList);
};

END_CUTLEADER_NAMESPACE()
