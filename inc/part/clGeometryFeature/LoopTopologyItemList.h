#pragma once

#include "clGeometryFeatureCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(LoopTopologyItem)

BEGIN_CUTLEADER_NAMESPACE()

// ��ͼ�λ�·��������ϡ�
class ClGeometryFeature_Export LoopTopologyItemList : public std::vector<LoopTopologyItemPtr>
{
public:
	LoopTopologyItemList(void);
	~LoopTopologyItemList(void);

public:
	// ɾ�������
	// ע��
	//   1) ���ǵݹ顣
	void RemoveTopItem(LoopTopologyItemPtr pLoopTopItem);

	// ĳ����������Ƿ���ڡ�
	BOOL TopItemExist(LoopTopologyItemPtr pLoopTopItem);
};

END_CUTLEADER_NAMESPACE()
