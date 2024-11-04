#pragma once

#include "clPartLayoutCommon.h"
#include "Rect2D.h"


DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartGroupList)
DECLARE_CUTLEADER_CLASS(PartGroup)
DECLARE_CUTLEADER_CLASS(LoopInstance)


BEGIN_CUTLEADER_NAMESPACE()

class PartLayoutHelper
{
public:
    // �ÿ�ѡ�ķ�ʽѡ��������á�
    // ע��
    // 1�������ֻ�ܱ�����ѡ�У�����ѡ��������еĲ��������Ҫ�ǿ�ѡ�˷����е�һ���������������ᱻѡ�С�
    static PartPlacementListPtr GetPartPlacements_byRectSelection(const Rect2D& rect, const PartPlacementList* pAllPartPlacements, const PartGroupList* pAllPartGroups,
        const std::map<LONGLONG, Rect2D>& m_rects_of_partPmt);

    // �õ�ѡ�ķ�ʽѡ��������á�
    static PartPlacementListPtr GetPartPlacements_byClickSelection(const Point2D& pt, const PartPlacementList* pAllPartPlacements, const PartGroupList* pAllPartGroups,
        const std::map<LONGLONG, Rect2D>& m_rects_of_partPmt);

    // ִ�������������顣
    static void ExecuteCommand2CreatePartGroup(CWnd* pView, PartGroupListPtr pAllPartGroups, PartGroupListPtr pSubPartGroups, PartPlacementListPtr pPartPlacements);

    // ִ�������ɢ����顣
    static void ExecuteCommand2ExplodePartGroup(CWnd* pView, PartGroupListPtr pAllPartGroups, PartGroupPtr pPartGroup_willBeDeleted);

    // ��鵶�߳ߴ��ǲ����ʺ��������ʵ������������ȷ��ƫ�ƾ���Ϊ�ʺϡ�
    static bool CheckToolSize_4_LoopInstance(const LoopInstance* pLoopInstance, double dToolSize);
};

END_CUTLEADER_NAMESPACE()
