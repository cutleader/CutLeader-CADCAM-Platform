#include "StdAfx.h"
#include "LoopToolEditAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
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


BEGIN_CUTLEADER_NAMESPACE()

LoopToolEditAction::LoopToolEditAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort,
                                       ToolAssignConfigItemPtr pToolAssignConfigItem, LeadConfigItemPtr pLeadConfigItem, ConditionTypeListPtr pCondTypeList)
    : CutFeatureEditBase(pPatList, pPatternLoopList, pLoopFeatureList, pViewPort)
{
    m_iMsgID = IDS_MSG_EDIT_LOOP_TOOL;

    m_iState = NONE;
    m_pToolAssignConfigItem = pToolAssignConfigItem;
	m_pLeadConfigItem = pLeadConfigItem;
    m_pCondTypeList = pCondTypeList;
    m_pActivePatternLoops_inRect.reset(new PatternLoopList);
    m_pSelectedPatternLoops.reset(new PatternLoopList);
}

LoopToolEditAction::~LoopToolEditAction(void)
{
}

BOOL LoopToolEditAction::LButtonDown(double& dPtX, double& dPtY)
{
    if (m_iState == NONE || m_iState == PATLOOP_SELECT_SECOND_POINT)
    {
        m_iState = PATLOOP_SELECT_FIRST_POINT;

        m_boxData[0] = dPtX;
        m_boxData[1] = dPtY;
        m_boxData[2] = dPtX;
        m_boxData[3] = dPtY;
    }

	return TRUE;
}

BOOL LoopToolEditAction::LButtonUp(double& dPtX, double& dPtY)
{
    CString strProductName, strMsg;
    strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

    if (m_iState == PATLOOP_SELECT_FIRST_POINT)
    {
        m_iState = PATLOOP_SELECT_SECOND_POINT;

        // �õ�ѡ�е�������
        m_pSelectedPatternLoops->clear();
        if (m_pActivePatternLoops_inRect->size() > 0)
        {
            m_pSelectedPatternLoops->insert(m_pSelectedPatternLoops->end(), m_pActivePatternLoops_inRect->begin(), m_pActivePatternLoops_inRect->end());
        }
        else if (m_pActivePatternLoop)
        {
            m_pSelectedPatternLoops->push_back(m_pActivePatternLoop);
        }

        // ��ʾ��Щ�����ĵ�����Ϣ
        if (m_pSelectedPatternLoops->size() > 0)
        {
            LoopToolDataListPtr pLoopToolList_4_edit(new LoopToolDataList);
            LoopToolDataListPtr pLoopToolList = m_pLoopFeatureList->GetLoopToolList();
            for (unsigned int i = 0; i < m_pSelectedPatternLoops->size(); i++)
            {
                LoopToolDataPtr pLoopTool = pLoopToolList->GetLoopTool_by_loopID(m_pSelectedPatternLoops->at(i)->GetID());
                pLoopToolList_4_edit->push_back(pLoopTool);
            }

			LoopStartCutDataListPtr pLoopStaEndCutList_4_edit(new LoopStartCutDataList);
			LoopStartCutDataListPtr pLoopStaEndCutList = m_pLoopFeatureList->GetLoopStaEndCutList();
			for (unsigned int i = 0; i < m_pSelectedPatternLoops->size(); i++)
			{
				LoopStartCutDataPtr pLoopStartCutData = pLoopStaEndCutList->GetItemByLoopID(m_pSelectedPatternLoops->at(i)->GetID());
				pLoopStaEndCutList_4_edit->push_back(pLoopStartCutData);
			}

            LoopToolEditPanel::GetInstance()->DisplayLoopToolInfo(GetEditorData(), pLoopToolList_4_edit, pLoopStaEndCutList_4_edit, m_pLeadConfigItem, m_pToolAssignConfigItem, m_pCondTypeList);
        }
        else
        {
            LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
        }
    }

    return TRUE;
}

BOOL LoopToolEditAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

    if (m_iState == PATLOOP_SELECT_FIRST_POINT)
    {
        m_boxData[2] = dPtX;
        m_boxData[3] = dPtY;

        Rect2D rect;
        rect.SetXMin(m_boxData[0] < m_boxData[2] ? m_boxData[0] : m_boxData[2]);
        rect.SetXMax(m_boxData[0] < m_boxData[2] ? m_boxData[2] : m_boxData[0]);
        rect.SetYMin(m_boxData[1] < m_boxData[3] ? m_boxData[1] : m_boxData[3]);
        rect.SetYMax(m_boxData[1] < m_boxData[3] ? m_boxData[3] : m_boxData[1]);
        m_pActivePatternLoops_inRect = m_pPatternLoopList->GetPatternLoopsInRect(rect);
        m_pActivePatternLoop.reset();
    }
    else
    {
        IPatternPtr pPattern = m_pPatList->GetPatternClosestTo(pt);
        if (pPattern)
        {
            m_pActivePatternLoop = m_pPatternLoopList->FindLoopByPat(pPattern.get());
        }
        m_pActivePatternLoops_inRect->clear();
    }

	return TRUE;
}

BOOL LoopToolEditAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CutFeatureEditBase::OnKeyDown(nChar, nRepCnt, nFlags);

    if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
    {
        if (m_pSelectedPatternLoops->size() > 0)
        {
            m_pSelectedPatternLoops->clear();
            LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
        }
    }

	return TRUE;
}

BOOL LoopToolEditAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();
    LoopDrawer loopDrawer(m_pViewPort);

	// �ϵ�����
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

    // ����ѡ�е�������
    for (unsigned int i = 0; i < m_pSelectedPatternLoops->size(); i++)
    {
        IPatternLoopPtr pPatternLoop = m_pSelectedPatternLoops->operator [](i);
        const LoopToolData* pLoopToolData = m_pLoopFeatureList->GetLoopToolList()->GetLoopTool_by_loopID(pPatternLoop->GetID()).get();
        loopDrawer.DrawSelectedLoop(pPatternLoop, LoopToolManager::GetLoopDisplayColor(pLoopToolData));
    }

    if (m_iState == PATLOOP_SELECT_FIRST_POINT) // ��ѡ��
    {
        // ���ƿ��
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

        // ���ƺ�ѡ������
        for (unsigned int i = 0; i < m_pActivePatternLoops_inRect->size(); i++)
        {
            IPatternLoopPtr pActivePatternLoop_inRect = m_pActivePatternLoops_inRect->at(i);
            if (!m_pSelectedPatternLoops->GetLoopByID(pActivePatternLoop_inRect->GetID()))
            {
                const LoopToolData* pLoopToolData = m_pLoopFeatureList->GetLoopToolList()->GetLoopTool_by_loopID(pActivePatternLoop_inRect->GetID()).get();
                loopDrawer.DrawActiveLoop(pActivePatternLoop_inRect, LoopToolManager::GetLoopDisplayColor(pLoopToolData));
            }
        }
    }
    else
    {
        // ���ƺ�ѡ����
        if (m_pActivePatternLoop && !m_pSelectedPatternLoops->GetLoopByID(m_pActivePatternLoop->GetID()))
        {
            const LoopToolData* pLoopToolData = m_pLoopFeatureList->GetLoopToolList()->GetLoopTool_by_loopID(m_pActivePatternLoop->GetID()).get();
            loopDrawer.DrawActiveLoop(m_pActivePatternLoop, LoopToolManager::GetLoopDisplayColor(pLoopToolData));
        }
    }

	// �ָ�����
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void LoopToolEditAction::UpdateForUndoRedo()
{
	m_pActivePatternLoop.reset();
	m_pActivePatternLoops_inRect->clear();
	m_pSelectedPatternLoops->clear();
	LoopToolEditPanel::GetInstance()->ShowBar(FALSE);
}

END_CUTLEADER_NAMESPACE()