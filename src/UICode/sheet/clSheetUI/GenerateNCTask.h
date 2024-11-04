#pragma once

#include "clSheetUICommon.h"
#include "TaskEx.h"

DECLARE_CUTLEADER_CLASS(CutNodeList)
DECLARE_CUTLEADER_CLASS(NCCodeData)
DECLARE_CUTLEADER_CLASS(Sheet)
DECLARE_CUTLEADER_CLASS(GenerateNCTask)

BEGIN_CUTLEADER_NAMESPACE()

class ClSheetUI_Export GenerateNCTask : public TaskEx
{
public:
	GenerateNCTask(SheetPtr pSheet);
	~GenerateNCTask(void);

public: // inherit from class TaskEx.
	virtual void Run();

public:
	NCCodeDataPtr GetNCCodeData() const { return m_pNCCodeData; }
	CutNodeListPtr GetCutNodeList() const { return m_pCutNodeList; }
    const std::set<LONGLONG>& GetPatternLoops_failedToOffset() const { return m_patternLoops_failedToOffset; }

private:
	SheetPtr m_pSheet;

	// ���С��и�ڵ㡱���ϡ�
	CutNodeListPtr m_pCutNodeList;

	// the NC data.
	NCCodeDataPtr m_pNCCodeData;

    // �˴���¼ƫ��ʧ�ܵļ���������
    std::set<LONGLONG> m_patternLoops_failedToOffset;
};

END_CUTLEADER_NAMESPACE()
