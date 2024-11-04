#pragma once

#include "clPartCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(IPatternLoop)
DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(PartTopologyItemList)

BEGIN_CUTLEADER_NAMESPACE()

// ��������
class ClPart_Export PartTopologyItem : public ClData
{
public:
	PartTopologyItem(void);
	PartTopologyItem(IPatternLoopPtr pOuterLoop);
	~PartTopologyItem(void);

public: // get/set functions.
	void SetOuterLoop(IPatternLoopPtr pOuterLoop) { m_pOuterLoop = pOuterLoop; }
	IPatternLoopPtr GetOuterLoop() const { return m_pOuterLoop; }

	void SetInnerLoopList(PatternLoopListPtr pInnerLoopList) { m_pInnerLoopList = pInnerLoopList; }
	PatternLoopListPtr GetInnerLoopList() const { return m_pInnerLoopList; }

	void SetSubItemList(PartTopologyItemListPtr pSubItemList) { m_pSubItemList = pSubItemList; }
	PartTopologyItemListPtr GetSubItemList() const { return m_pSubItemList; }

private:
	// �������Χ��·��
	// ע��
	//   1) ����Ϊ�պϵġ�
	IPatternLoopPtr m_pOuterLoop;

	// ������ڲ���·��
	// ע��
	//   1) ����Ϊ���ŵġ�
	PatternLoopListPtr m_pInnerLoopList;

	// ����������ġ��������
	PartTopologyItemListPtr m_pSubItemList;
};

END_CUTLEADER_NAMESPACE()
