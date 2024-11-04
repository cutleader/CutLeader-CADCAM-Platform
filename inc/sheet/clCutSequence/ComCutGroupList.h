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
	// �õ�������iPartInstID���ġ������顱��
	ComCutGroupPtr GetComCutGroup(LONGLONG iPartInstID);
	int GetGroupIndex(LONGLONG iPartInstID);

	// �õ������顣
	ComCutGroupPtr GetGroupByID(LONGLONG iGroupID);

	// ɾ��ĳ���������顱��
	void RemoveGroup(LONGLONG iGroupID);
};

END_CUTLEADER_NAMESPACE()
