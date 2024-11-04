#include "StdAfx.h"
#include "PartLayoutHelper.h"

#include "LogMgr.h"
#include "ICommand.h"
#include "CommandManager.h"

#include "PartPlacementList.h"
#include "PartPlacement.h"
#include "PartGroup.h"
#include "PartGroupList.h"
#include "CreatePartGroupCommand.h"
#include "clPartLayoutResource.h"
#include "DeletePartGroupCommand.h"
#include "LoopInstance.h"
#include "IPatternLoop.h"
#include "LoopProcessor.h"
#include "LoopInstanceCache.h"
#include "LoopStartCutProcessor.h"
#include "LoopCornerData.h"
#include "PatternList.h"
#include "PatternLoopNode.h"
#include "PatternLoopNodeList.h"


using namespace std;


BEGIN_CUTLEADER_NAMESPACE()


PartPlacementListPtr PartLayoutHelper::GetPartPlacements_byRectSelection(const Rect2D& rect, const PartPlacementList* pAllPartPlacements, const PartGroupList* pAllPartGroups,
                                                                         const std::map<LONGLONG, Rect2D>& m_rects_of_partPmt)
{
    PartPlacementListPtr pPartPlacements_return(new PartPlacementList);

    if (!pAllPartPlacements || !pAllPartGroups)
    {
        LogMgr::DebugWarn(_T("32012"));
        return pPartPlacements_return;
    }

    // �ڿ���е�����������á�
    PartPlacementListPtr pPartPlacements_inRect;
    if (m_rects_of_partPmt.size() > 0)
    {
        pPartPlacements_inRect = pAllPartPlacements->GetPartPlacementInRect(rect, m_rects_of_partPmt);
    }
    else
    {
        pPartPlacements_inRect = pAllPartPlacements->GetPartPlacementInRect(rect);
    }

    // �Ѽ�����Щ�ڷ����кͲ��ڷ����е�����������á�
    PartGroupListPtr pPartGroupList_tmp(new PartGroupList); // ��ʱ��¼ѡ�еķ��飬��ֹ�ظ�
    for (unsigned int i = 0; i < pPartPlacements_inRect->size(); i++)
    {
        PartPlacementPtr pPartPlacement_inRect = pPartPlacements_inRect->operator [](i);
        PartGroupPtr pPartGroup = pAllPartGroups->GetPartGroupIncludePartPlacement(pPartPlacement_inRect.get());
        if (pPartGroup)
        {
            PartGroupPtr pPartGroup_topLevel = pAllPartGroups->GetTopLevelPartGroup(pPartGroup);
            if (!pPartGroupList_tmp->GetPartGroupByID(pPartGroup_topLevel->GetID()))
            {
                pPartGroupList_tmp->push_back(pPartGroup_topLevel);
                PartPlacementListPtr pPartPlacements = PartGroup::GetAllPartPlacementsInThisGroup(pPartGroup_topLevel.get());
                pPartPlacements_return->insert(pPartPlacements_return->end(), pPartPlacements->begin(), pPartPlacements->end());
            }
        }
        else
        {
            pPartPlacements_return->push_back(pPartPlacement_inRect);
        }
    }

    return pPartPlacements_return;
}

PartPlacementListPtr PartLayoutHelper::GetPartPlacements_byClickSelection(const Point2D& pt, const PartPlacementList* pAllPartPlacements, const PartGroupList* pAllPartGroups,
                                                                          const std::map<LONGLONG, Rect2D>& m_rects_of_partPmt)
{
    PartPlacementListPtr pPartPlacements_return(new PartPlacementList);

    PartPlacementPtr pClosestPartPlacement;
    if (m_rects_of_partPmt.size() > 0)
    {
        pClosestPartPlacement = pAllPartPlacements->GetPartPlacementClosestTo(pt, m_rects_of_partPmt);
    }
    else
    {
        pClosestPartPlacement = pAllPartPlacements->GetPartPlacementClosestTo(pt);
    }

    if (pClosestPartPlacement)
    {
        PartGroupPtr pPartGroup = pAllPartGroups->GetPartGroupIncludePartPlacement(pClosestPartPlacement.get());
        if (pPartGroup) // �����������������������һ������顣
        {
            PartGroupPtr pPartGroup_topLevel = pAllPartGroups->GetTopLevelPartGroup(pPartGroup);
            PartPlacementListPtr pPartPlacementsInGroup = PartGroup::GetAllPartPlacementsInThisGroup(pPartGroup_topLevel.get());
            pPartPlacements_return->insert(pPartPlacements_return->end(), pPartPlacementsInGroup->begin(), pPartPlacementsInGroup->end());
        }
        else
        {
            pPartPlacements_return->push_back(pClosestPartPlacement);
        }
    }

    return pPartPlacements_return;
}

void PartLayoutHelper::ExecuteCommand2CreatePartGroup(CWnd* pView, PartGroupListPtr pAllPartGroups, PartGroupListPtr pSubPartGroups, PartPlacementListPtr pPartPlacements)
{
    CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT);
    PartGroupPtr pNewPartGroup(new PartGroup(pSubPartGroups, pPartPlacements));
    ICommandPtr pCommand(new CreatePartGroupCommand(EditorData(pView, str), pAllPartGroups, pNewPartGroup));
    pCommand->Do();
    CommandManager::GetInstance()->AddCommand(pCommand);
}

void PartLayoutHelper::ExecuteCommand2ExplodePartGroup(CWnd* pView, PartGroupListPtr pAllPartGroups, PartGroupPtr pPartGroup_willBeDeleted)
{
    CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT);
    ICommandPtr pCommand(new DeletePartGroupCommand(EditorData(pView, str), pAllPartGroups, pPartGroup_willBeDeleted));
    pCommand->Do();
    CommandManager::GetInstance()->AddCommand(pCommand);
}

bool PartLayoutHelper::CheckToolSize_4_LoopInstance(const LoopInstance* pLoopInstance, double dToolSize)
{
    if (fabs(dToolSize) < GEOM_TOLERANCE)
    {
        return true;
    }

    if (pLoopInstance->GetCacheData()->GetPatternLoop()->IsOpenPath())
    {
        return true;
    }

    // ��¡һ����ʱ�ļ����������ڲ���ƫ�ơ�
    IPatternLoopPtr pPatternLoop_copy;
    {
        // ��¡����ͼ��
        const IPatternLoop* pPatternLoop = pLoopInstance->GetCacheData()->GetPatternLoop().get();
        PatternListPtr pPatternList = pPatternLoop->GetPatternList();
        PatternListPtr pPatternList_copy = boost::dynamic_pointer_cast<PatternList>(pPatternList->Clone());

        // ��¡���λ�·��
        pPatternLoop_copy = boost::dynamic_pointer_cast<IPatternLoop>(pPatternLoop->Clone());

        // ���¼��������ڵ������õļ���ͼ�Ρ�
        PatternLoopNodeListPtr pNewLoopNodeList = pPatternLoop_copy->GetPatternLoopNodes();
        for (unsigned int j = 0; j < pNewLoopNodeList->size(); j++)
        {
            PatternLoopNodePtr pNewLoopNode = pNewLoopNodeList->operator [](j);
            const IPattern* pOldPat = pNewLoopNode->GetPattern().get();
            int iPatIdx = pPatternList->GetPatternIndex(pOldPat);
            pNewLoopNode->SetPattern(pPatternList_copy->operator [](iPatIdx));
        }

        // ���»��档
        LoopProcessor::UpdateCacheEx(pPatternLoop_copy);
    }

    // ƫ�Ʋ���
    {
        PatternLoopCutOffsetType emLoopCutOffset = LoopStartCutProcessor::CalculateLoopCutOffset(pLoopInstance->GetCacheData()->GetLoopStartCutData().get());
        if (emLoopCutOffset == PatternLoopCutOffset_On)
        {
            return true;
        }
        else
        {
            BOOL bCutInside = TRUE;
            if (emLoopCutOffset == PatternLoopCutOffset_Outside)
            {
                bCutInside = FALSE;
            }
            vector<pair<LONGLONG, LONGLONG>> cornerPats = pLoopInstance->GetCacheData()->GetLoopCorner()->GetAllCornerPats();
            return LoopProcessor::OffsetPolyLoop(pPatternLoop_copy, bCutInside, dToolSize / 2.0, cornerPats);
        }
    }

    return true;
}

END_CUTLEADER_NAMESPACE()
