#pragma once

#include "clPartLayoutUICommon.h"
#include <set>


DECLARE_CUTLEADER_CLASS(WrongToolSizeLoopInstances)
DECLARE_CUTLEADER_CLASS(LoopInstanceList)
DECLARE_CUTLEADER_CLASS(GlViewPort)


BEGIN_CUTLEADER_NAMESPACE()


// ����������Ų����ʵ��߳ߴ������ʵ����
class ClPartLayoutUI_Export WrongToolSizeLoopInstances
{
public:
    WrongToolSizeLoopInstances();
    ~WrongToolSizeLoopInstances();

public:
    LoopInstanceListPtr GetLoopInstances_wrongToolSize() const { return m_pLoopInstances_wrongToolSize; }

public:
    // ��ʼ���ö���
    // ע��
    // 1���ú������ڳ�ʼ���ö��󣬲����ú��������Ǳ�����õġ�
    // 2��pAllLoopInstances����������е�����ʵ����
    // 3��patternLoops_failedToOffset��û���ܳɹ�ƫ�Ƶļ���������
    void Init(const LoopInstanceList* pAllLoopInstances, const std::set<LONGLONG>& patternLoops_failedToOffset);

    // ���µ�ǰ����
    // ע��
    // 1���ú������ڶԸö�����и��¡�
    // 1��pLoopInstances_willBeChecked����Ҫ����Щ��������ƫ�Ʋ��ԡ�
    void Update(const LoopInstanceList* pLoopInstances_willBeChecked, double dToolSize);

    // ����ƫ�������������ʵ����
    void Draw(GlViewPortPtr pViewPort);

private:
    LoopInstanceListPtr m_pLoopInstances_wrongToolSize;
};

END_CUTLEADER_NAMESPACE()
