#pragma once

#include "clGeometryFeatureCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(LoopTopologyItemList)
DECLARE_CUTLEADER_CLASS(IPatternLoop)

BEGIN_CUTLEADER_NAMESPACE()

// ��ͼ�λ�·�������
class ClGeometryFeature_Export LoopTopologyItem : public ClData
{
public:
	LoopTopologyItem(IPatternLoopPtr pPatternLoop);
	~LoopTopologyItem(void);

public: // get/set functions.
	void SetPatternLoop(IPatternLoopPtr pPatternLoop) { m_pPatternLoop = pPatternLoop;}
	IPatternLoopPtr GetPatternLoop() const { return m_pPatternLoop; }

	void SetSubItemList(LoopTopologyItemListPtr pSubItemList) { m_pSubItemList = pSubItemList; }
	LoopTopologyItemListPtr GetSubItemList() const { return m_pSubItemList; }

private:
	// ��ǰͼ�λ�·��
	IPatternLoopPtr m_pPatternLoop;

	// �û�·�����ġ��������
	LoopTopologyItemListPtr m_pSubItemList;
};

END_CUTLEADER_NAMESPACE()
