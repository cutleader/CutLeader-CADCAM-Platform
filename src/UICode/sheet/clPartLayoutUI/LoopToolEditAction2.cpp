#include "StdAfx.h"
#include "LoopToolEditAction2.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "DummyAction.h"
#include "ValueInputManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "PatternList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "LoopDrawer.h"
#include "clGeometryFeatureResource.h"
#include "LoopCutFeature.h"
#include "LoopCutFeatureList.h"
#include "LoopToolData.h"
#include "LoopToolDataList.h"
#include "LoopStartCutDataList.h"
#include "LoopToolEditPanel.h"
#include "clCutFeatureResource.h"
#include "LoopToolManager.h"
#include "PartInstance.h"
#include "LoopInstanceManager.h"
#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "LoopInstanceList.h"
#include "PartLayoutHelper.h"
#include "PartPlacementList.h"
#include "PartInstanceList.h"
#include "PartPlacementManager.h"


using namespace std;


BEGIN_CUTLEADER_NAMESPACE()

LoopToolEditAction2::LoopToolEditAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort,
										 ToolAssignConfigItemPtr pToolAssignConfigItem, LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList, SpreadSamePartDlg* pSpreadSamePartDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_iMsgID = IDS_MSG_EDIT_LOOP_TOOL;

    m_pSpreadSamePartDlg = pSpreadSamePartDlg;
    m_pSpreadSamePartDlg->AddObserver(this);

    m_iState = NONE;
    m_pToolAssignConfigItem = pToolAssignConfigItem;
	m_pLeadConfigItem = pLeadConfigItem;
    m_pCondTypeList = pCondTypeList;
    m_pActiveLoopInsts_inRect.reset(new LoopInstanceList);
    m_pSelectedLoopInsts.reset(new LoopInstanceList);

	// 得到所有“回路实例”。
	m_pAllLoopInstances = LoopInstanceManager::GetLoopInstanceList(m_pPartInstanceList);

    LoopToolEditPanel::GetInstance()->GetLoopToolEditDlg()->AddObserver(this);

    // 对所有轮廓进行偏移测试
    {
        m_pWrongToolSizeLoopInstances.reset(new WrongToolSizeLoopInstances);
        PartPlacementListPtr pPartPmts_offset(new PartPlacementList);
        PartInstanceListPtr pPartInsts_offset(new PartInstanceList);
        std::set<LONGLONG> patternLoops_failedToOffset = PartPlacementManager::OffsetPartPmts(pPartPlacements.get(), pPartInstances.get(), pPartPmts_offset, pPartInsts_offset);
        if (patternLoops_failedToOffset.size() > 0)
        {
            m_pWrongToolSizeLoopInstances->Init(m_pAllLoopInstances.get(), patternLoops_failedToOffset);
        }
    }
}

LoopToolEditAction2::~LoopToolEditAction2(void)
{
    m_pSpreadSamePartDlg->RemoveObserver(GetID());

    // 此时需要判断对话框是否有效，当从数据库加载零件，然后编辑轮廓刀具，最后直接关闭程序时，LoopToolEditDlg会在该action之前被销毁，RemoveObserver函数就不能被调用。
    if (IsWindow(LoopToolEditPanel::GetInstance()->GetLoopToolEditDlg()->m_hWnd))
    {
        LoopToolEditPanel::GetInstance()->GetLoopToolEditDlg()->RemoveObserver(GetID());
    }
}

BOOL LoopToolEditAction2::LButtonDown(double& dPtX, double& dPtY)
{
    if (m_iState == NONE || m_iState == LOOPINST_SELECT_SECOND_POINT)
    {
        m_iState = LOOPINST_SELECT_FIRST_POINT;

        m_boxData[0] = dPtX;
        m_boxData[1] = dPtY;
        m_boxData[2] = dPtX;
        m_boxData[3] = dPtY;
    }

	return TRUE;
}

BOOL LoopToolEditAction2::LButtonUp(double& dPtX, double& dPtY)
{
    CString strProductName, strMsg;
    strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

    if (m_iState == LOOPINST_SELECT_FIRST_POINT)
    {
        m_iState = LOOPINST_SELECT_SECOND_POINT;

        // 得到选中的轮廓
		if (!ValueInputManager::GetInstance()->IsInputCtrl())
		{
			m_pSelectedLoopInsts->clear();
		}
        if (m_pActiveLoopInsts_inRect->size() > 0)
        {
            m_pSelectedLoopInsts->insert(m_pSelectedLoopInsts->end(), m_pActiveLoopInsts_inRect->begin(), m_pActiveLoopInsts_inRect->end());
        }
        else if (m_pActiveLoopInstance)
        {
            m_pSelectedLoopInsts->push_back(m_pActiveLoopInstance);
        }

        // 显示这些轮廓的刀具信息
        DisplayLoopToolInfo();
    }

    return TRUE;
}

void LoopToolEditAction2::EditToolsOfSelectedLoopInstances(LoopInstanceListPtr pSelectedLoopInstances)
{
    m_pSelectedLoopInsts = pSelectedLoopInstances;
    DisplayLoopToolInfo();
}

BOOL LoopToolEditAction2::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

    if (m_iState == LOOPINST_SELECT_FIRST_POINT)
    {
        m_boxData[2] = dPtX;
        m_boxData[3] = dPtY;

        Rect2D rect;
        rect.SetXMin(m_boxData[0] < m_boxData[2] ? m_boxData[0] : m_boxData[2]);
        rect.SetXMax(m_boxData[0] < m_boxData[2] ? m_boxData[2] : m_boxData[0]);
        rect.SetYMin(m_boxData[1] < m_boxData[3] ? m_boxData[1] : m_boxData[3]);
        rect.SetYMax(m_boxData[1] < m_boxData[3] ? m_boxData[3] : m_boxData[1]);
		m_pActiveLoopInsts_inRect = LoopInstanceManager::GetLoopInstsInRect(m_pAllLoopInstances, rect);
        m_pActiveLoopInstance.reset();
    }
    else
    {
		m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pAllLoopInstances, pt);
        m_pActiveLoopInsts_inRect->clear();
    }

	return TRUE;
}

BOOL LoopToolEditAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (ValueInputManager::GetInstance()->IsInputCancel(nChar) && m_pSelectedLoopInsts->size() > 0)
    {
        m_pSelectedLoopInsts->clear();
        LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
    }

	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL LoopToolEditAction2::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();
    LoopDrawer loopDrawer(m_pViewPort);

	// 老的设置
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

    // 绘制选中的轮廓。
    for (unsigned int i = 0; i < m_pSelectedLoopInsts->size(); i++)
    {
		LoopInstancePtr pSelectedLoopInst = m_pSelectedLoopInsts->at(i);
        COLORREF iColor = LoopToolManager::GetLoopDisplayColor(pSelectedLoopInst->GetCacheData()->GetLoopTool().get());
        loopDrawer.DrawSelectedLoop(pSelectedLoopInst->GetCacheData()->GetPatternLoop(), iColor, pSelectedLoopInst->GetTransformMatrix());
    }

    if (m_iState == LOOPINST_SELECT_FIRST_POINT) // 框选。
    {
        // 绘制框框。
        {
            // backup the old draw config.
            bool bOldStipple = false;
            int iOldRepeat = 1;
            unsigned short iOldPattern = 0xffff;
            m_pViewPort->GetLineStipple(bOldStipple, iOldRepeat, iOldPattern);

            // draw the selection rectangle.
            m_pViewPort->SetLineStipple(true, 2, 0xcccc);
            m_pViewPort->DrawRect(m_boxData[0], m_boxData[1], m_boxData[2], m_boxData[3]);

            // restore the drawer
            m_pViewPort->SetLineStipple(bOldStipple, iOldRepeat, iOldPattern);
        }

        // 绘制候选轮廓。
        for (unsigned int i = 0; i < m_pActiveLoopInsts_inRect->size(); i++)
        {
            LoopInstancePtr pActiveLoopInst_inRect = m_pActiveLoopInsts_inRect->at(i);
            if (!m_pSelectedLoopInsts->GetLoopInstance_ByPartInstanceAndPatternLoop(pActiveLoopInst_inRect->GetPartInstance()->GetID(), pActiveLoopInst_inRect->GetPatternLoopID()))
            {
                COLORREF iColor = LoopToolManager::GetLoopDisplayColor(pActiveLoopInst_inRect->GetCacheData()->GetLoopTool().get());
                loopDrawer.DrawActiveLoop(pActiveLoopInst_inRect->GetCacheData()->GetPatternLoop(), iColor, pActiveLoopInst_inRect->GetTransformMatrix());
            }
        }
    }
    else
    {
        // 绘制候选轮廓
        if (m_pActiveLoopInstance && !m_pSelectedLoopInsts->GetLoopInstance_ByPartInstanceAndPatternLoop(m_pActiveLoopInstance->GetPartInstance()->GetID(), m_pActiveLoopInstance->GetPatternLoopID()))
        {
            COLORREF iColor = LoopToolManager::GetLoopDisplayColor(m_pActiveLoopInstance->GetCacheData()->GetLoopTool().get());
            loopDrawer.DrawActiveLoop(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), iColor, m_pActiveLoopInstance->GetTransformMatrix());
        }
    }

	// 恢复设置
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

    // 绘制偏移有问题的轮廓实例。
    if (pDisplayParam->GetShowIncorrectToolSize())
    {
        m_pWrongToolSizeLoopInstances->Draw(m_pViewPort);
    }

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void LoopToolEditAction2::UpdateForUndoRedo()
{
	m_pActiveLoopInstance.reset();
	m_pActiveLoopInsts_inRect->clear();
	m_pSelectedLoopInsts->clear();
	LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
}

void LoopToolEditAction2::Update(int iEventID, VARIANT varEventContent)
{
    if (iEventID == ID_EVENT_ChangedToolSizeOfSelectedLoops) // 此处需要检查刀具尺寸是不是适合这些选中的轮廓。
    {
        double dToolSize = varEventContent.dblVal;

        // 得到所有需检查的轮廓实例。
        LoopInstanceListPtr pLoopInstances_willBeChecked;
        if (m_pSelectedLoopInsts->size() > 0)
        {
            pLoopInstances_willBeChecked.reset(new LoopInstanceList);
            if (m_pSpreadSamePartDlg->IsSpread())
            {
                for (unsigned int i = 0; i < m_pSelectedLoopInsts->size(); i++)
                {
                    LoopInstancePtr pSelectedLoopInst = m_pSelectedLoopInsts->operator [](i);
                    for (unsigned int j = 0; j < m_pAllLoopInstances->size(); j++)
                    {
                        LoopInstancePtr pLoopInstance = m_pAllLoopInstances->operator [](j);
                        if (pLoopInstance->GetPatternLoopID() == pSelectedLoopInst->GetPatternLoopID())
                        {
                            pLoopInstances_willBeChecked->push_back(pLoopInstance);
                        }
                    }
                }
            }
            else
            {
                pLoopInstances_willBeChecked = m_pSelectedLoopInsts;
            }
        }

        // 更新刀具尺寸不合适的轮廓。
        m_pWrongToolSizeLoopInstances->Update(pLoopInstances_willBeChecked.get(), dToolSize);

        // 如果某些轮廓被分配了不合适的刀具尺寸，这里提示用户。
        if (m_pWrongToolSizeLoopInstances->GetLoopInstances_wrongToolSize()->size() > 0)
        {
            CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LoopHaveWrongToolSize);
            MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
        }

        // 刷新视图
        CWnd::FromHandle(m_pViewPort->GetWnd())->Invalidate();
    }
    else
    {
        // SpreadSamePartDlg有变化，这里需要重新显示一遍
        DisplayLoopToolInfo();
    }
}

void LoopToolEditAction2::DisplayLoopToolInfo()
{
    if (m_pSelectedLoopInsts->size() > 0)
    {
        LoopToolDataListPtr pLoopToolList_4_edit(new LoopToolDataList);
		LoopStartCutDataListPtr pLoopStaEndCutList_4_edit(new LoopStartCutDataList);
        if (m_pSpreadSamePartDlg->IsSpread())
        {
            for (unsigned int i = 0; i < m_pSelectedLoopInsts->size(); i++)
            {
                LoopInstancePtr pSelectedLoopInst = m_pSelectedLoopInsts->at(i);
                for (unsigned int j = 0; j < m_pAllLoopInstances->size(); j++)
                {
                    LoopInstancePtr pLoopInstance = m_pAllLoopInstances->at(j);
                    if (pLoopInstance->GetPatternLoopID() == pSelectedLoopInst->GetPatternLoopID())
                    {
                        pLoopToolList_4_edit->push_back(pLoopInstance->GetCacheData()->GetLoopTool());
						pLoopStaEndCutList_4_edit->push_back(pLoopInstance->GetCacheData()->GetLoopStartCutData());
                    }
                }
            }
        }
        else
        {
            for (unsigned int i = 0; i < m_pSelectedLoopInsts->size(); i++)
            {
                LoopToolDataPtr pLoopTool = m_pSelectedLoopInsts->at(i)->GetCacheData()->GetLoopTool();
                pLoopToolList_4_edit->push_back(pLoopTool);
				LoopStartCutDataPtr pLoopStartCutData = m_pSelectedLoopInsts->at(i)->GetCacheData()->GetLoopStartCutData();
				pLoopStaEndCutList_4_edit->push_back(pLoopStartCutData);
            }
        }
        LoopToolEditPanel::GetInstance()->DisplayLoopToolInfo(GetEditorData(), pLoopToolList_4_edit, pLoopStaEndCutList_4_edit, m_pLeadConfigItem, m_pToolAssignConfigItem, m_pCondTypeList);
    }
    else
    {
        LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
    }
}

END_CUTLEADER_NAMESPACE()
