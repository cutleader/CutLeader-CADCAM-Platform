#pragma once

#include "clPartCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(PartTopologyItem)
DECLARE_CUTLEADER_CLASS(PatternLoopList)

BEGIN_CUTLEADER_NAMESPACE()

// ���������ϡ�
class ClPart_Export PartTopologyItemList : public std::vector<PartTopologyItemPtr>
{
public:
	PartTopologyItemList(void);
	~PartTopologyItemList(void);

public:
	// �ݹ�õ����������
	// ע��
	//   1) һ��"PatternLoopListPtr"�������һ��������������Ҫ�����ĸ���·Ϊ��Χ��·��
	void GetAllPart(std::vector<PatternLoopListPtr>& partList);
};

END_CUTLEADER_NAMESPACE()
