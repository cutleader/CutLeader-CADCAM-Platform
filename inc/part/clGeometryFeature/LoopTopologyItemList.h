#pragma once

#include "clGeometryFeatureCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(LoopTopologyItem)

BEGIN_CUTLEADER_NAMESPACE()

// “图形回路”拓扑项集合。
class ClGeometryFeature_Export LoopTopologyItemList : public std::vector<LoopTopologyItemPtr>
{
public:
	LoopTopologyItemList(void);
	~LoopTopologyItemList(void);

public:
	// 删除拓扑项。
	// 注：
	//   1) 不是递归。
	void RemoveTopItem(LoopTopologyItemPtr pLoopTopItem);

	// 某个“拓扑项”是否存在。
	BOOL TopItemExist(LoopTopologyItemPtr pLoopTopItem);
};

END_CUTLEADER_NAMESPACE()
