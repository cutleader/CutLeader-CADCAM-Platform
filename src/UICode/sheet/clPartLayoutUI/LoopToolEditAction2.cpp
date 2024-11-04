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

	// �õ����С���·ʵ������
	m_pAllLoopInstances = LoopInstanceManager::GetLoopInstanceList(m_pPartInstanceList);

    LoopToolEditPanel::GetInstance()->GetLoopToolEditDlg()->AddObserver(this);

    // ��������������ƫ�Ʋ���
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

    // ��ʱ��Ҫ�ж϶Ի����Ƿ���Ч���������ݿ���������Ȼ��༭�������ߣ����ֱ�ӹرճ���ʱ��LoopToolEditDlg���ڸ�action֮ǰ�����٣�RemoveObserver�����Ͳ��ܱ����á�
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

        // �õ�ѡ�е�����
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

        // ��ʾ��Щ�����ĵ�����Ϣ
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

	// �ϵ�����
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

    // ����ѡ�е�������
    for (unsigned int i = 0; i < m_pSelectedLoopInsts->size(); i++)
    {
		LoopInstancePtr pSelectedLoopInst = m_pSelectedLoopInsts->at(i);
        COLORREF iColor = LoopToolManager::GetLoopDisplayColor(pSelectedLoopInst->GetCacheData()->GetLoopTool().get());
        loopDrawer.DrawSelectedLoop(pSelectedLoopInst->GetCacheData()->GetPatternLoop(), iColor, pSelectedLoopInst->GetTransformMatrix());
    }

    if (m_iState == LOOPINST_SELECT_FIRST_POINT) // ��ѡ��
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
        // ���ƺ�ѡ����
        if (m_pActiveLoopInstance && !m_pSelectedLoopInsts->GetLoopInstance_ByPartInstanceAndPatternLoop(m_pActiveLoopInstance->GetPartInstance()->GetID(), m_pActiveLoopInstance->GetPatternLoopID()))
        {
            COLORREF iColor = LoopToolManager::GetLoopDisplayColor(m_pActiveLoopInstance->GetCacheData()->GetLoopTool().get());
            loopDrawer.DrawActiveLoop(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), iColor, m_pActiveLoopInstance->GetTransformMatrix());
        }
    }

	// �ָ�����
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

    // ����ƫ�������������ʵ����
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
    if (iEventID == ID_EVENT_ChangedToolSizeOfSelectedLoops) // �˴���Ҫ��鵶�߳ߴ��ǲ����ʺ���Щѡ�е�������
    {
        double dToolSize = varEventContent.dblVal;

        // �õ��������������ʵ����
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

        // ���µ��߳ߴ粻���ʵ�������
        m_pWrongToolSizeLoopInstances->Update(pLoopInstances_willBeChecked.get(), dToolSize);

        // ���ĳЩ�����������˲����ʵĵ��߳ߴ磬������ʾ�û���
        if (m_pWrongToolSizeLoopInstances->GetLoopInstances_wrongToolSize()->size() > 0)
        {
            CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LoopHaveWrongToolSize);
            MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
        }

        // ˢ����ͼ
        CWnd::FromHandle(m_pViewPort->GetWnd())->Invalidate();
    }
    else
    {
        // SpreadSamePartDlg�б仯��������Ҫ������ʾһ��
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
