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
    // 用框选的方式选择零件放置。
    // 注：
    // 1）零件组只能被整体选中，不能选择零件组中的部分零件。要是框选了分组中的一个零件，整个分组会被选中。
    static PartPlacementListPtr GetPartPlacements_byRectSelection(const Rect2D& rect, const PartPlacementList* pAllPartPlacements, const PartGroupList* pAllPartGroups,
        const std::map<LONGLONG, Rect2D>& m_rects_of_partPmt);

    // 用点选的方式选择零件放置。
    static PartPlacementListPtr GetPartPlacements_byClickSelection(const Point2D& pt, const PartPlacementList* pAllPartPlacements, const PartGroupList* pAllPartGroups,
        const std::map<LONGLONG, Rect2D>& m_rects_of_partPmt);

    // 执行命令，创建零件组。
    static void ExecuteCommand2CreatePartGroup(CWnd* pView, PartGroupListPtr pAllPartGroups, PartGroupListPtr pSubPartGroups, PartPlacementListPtr pPartPlacements);

    // 执行命令，打散零件组。
    static void ExecuteCommand2ExplodePartGroup(CWnd* pView, PartGroupListPtr pAllPartGroups, PartGroupPtr pPartGroup_willBeDeleted);

    // 检查刀具尺寸是不是适合这个轮廓实例。轮廓能正确的偏移就认为适合。
    static bool CheckToolSize_4_LoopInstance(const LoopInstance* pLoopInstance, double dToolSize);
};

END_CUTLEADER_NAMESPACE()
