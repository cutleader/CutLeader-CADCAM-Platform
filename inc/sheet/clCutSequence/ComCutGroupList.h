#pragma once

#include "clCutSequenceCommon.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(ComCutGroup)

BEGIN_CUTLEADER_NAMESPACE()

class ComCutGroupList : public std::vector<ComCutGroupPtr>
{
public:
	ComCutGroupList(void);
	~ComCutGroupList(void);

public:
	// 得到包含“iPartInstID”的“共边组”。
	ComCutGroupPtr GetComCutGroup(LONGLONG iPartInstID);
	int GetGroupIndex(LONGLONG iPartInstID);

	// 得到共边组。
	ComCutGroupPtr GetGroupByID(LONGLONG iGroupID);

	// 删除某个“共边组”。
	void RemoveGroup(LONGLONG iGroupID);
};

END_CUTLEADER_NAMESPACE()
