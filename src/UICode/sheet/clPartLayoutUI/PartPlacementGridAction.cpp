#include "StdAfx.h"
#include "PartPlacementGridAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "ICommand.h"
#include "MathEx.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "LogMgr.h"

#include "PatternList.h"
#include "LoopStartCutDataList.h"
#include "Part.h"

#include "PartPlacementDrawer.h"
#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartPlacementGridCommand.h"
#include "clPartLayoutResource.h"
#include "PartPlacementSelectAction.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementGridAction::PartPlacementGridAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances,
								     PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort, CreatePartGridDlg* pCreatePartGridDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_iMsgID = IDS_OUTPUT_GRID_SEL_PART;
	m_pCreatePartGridDlg = pCreatePartGridDlg;

	m_iState = GRID_PARTPMT_NOTSTART;
	m_pGridPartPmts.reset(new PartPlacementList);
}

PartPlacementGridAction::~PartPlacementGridAction(void)
{
}

BOOL PartPlacementGridAction::LButtonDown(double& dPtX, double& dPtY)
{
	CString str, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_iState == GRID_PARTPMT_NOTSTART)
	{
		if (!m_pActivePartPmt)
		{
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_NOPRTPMT_SELECT);
			MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
		}
		else
		{
			if (m_pActivePartPmt->IsGrid())
			{
				str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_GRID_PART_SELECTED);
				MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
			}
			else
			{
				m_pCreatePartGridDlg->ShowPartGridDlg(m_pViewPort, m_pPartPlacementList, m_pPartInstanceList, m_pActivePartPmt, m_pGridPartPmts);
				m_iState = GRID_PARTPMT_PART_SELECTED;
				m_iMsgID = IDS_OUTPUT_GRID_PARTPMT;
			}
		}
	}

	return TRUE;
}

void PartPlacementGridAction::CreateGridFromPartPlacement(PartPlacementPtr pPartPlacement)
{
    if (pPartPlacement->IsGrid())
    {
        LogMgr::DebugWarn(_T("546538"), CWnd::FromHandle(m_pViewPort->GetWnd()));
        return;
    }

    m_pActivePartPmt = pPartPlacement;
    m_pCreatePartGridDlg->ShowPartGridDlg(m_pViewPort, m_pPartPlacementList, m_pPartInstanceList, m_pActivePartPmt, m_pGridPartPmts);
    m_iState = GRID_PARTPMT_PART_SELECTED;
    m_iMsgID = IDS_OUTPUT_GRID_PARTPMT;
}

BOOL PartPlacementGridAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_iState == GRID_PARTPMT_NOTSTART)
	{
		Point2D pt(dPtX, dPtY);
		m_pActivePartPmt = m_pPartPlacementList->GetPartPlacementClosestTo(pt);
	}
	else if (m_iState == GRID_PARTPMT_PART_SELECTED)
	{
		// 是不是取消了阵列。
		if (m_pCreatePartGridDlg->IsCanceled())
        {
            SendMessage2EnterPartPlacementSelectAction();
        }

		// 是不是确认了创建阵列。
		if (m_pCreatePartGridDlg->IsCreateGrid() && m_pGridPartPmts->size() > 0)
		{
			PartPlacementPtr pGridPartPmt = m_pGridPartPmts->at(0);
			ICommandPtr pCommand(new PartPlacementGridCommand(GetEditorData(), m_pPartPlacementList, m_pPartInstanceList, m_pActivePartPmt, pGridPartPmt));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

            // 进入零件选择动作。
            SendMessage2EnterPartPlacementSelectAction(m_pActivePartPmt.get());
		}
	}

	return TRUE;
}

BOOL PartPlacementGridAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL PartPlacementGridAction::DrawBeforeFinish()
{
	PartPlacementDrawer partPlacementDrawer(m_pViewPort);

	if (m_iState == GRID_PARTPMT_NOTSTART)
	{
		// 高亮要做阵列的零件。
		if (m_pActivePartPmt)
        {
			partPlacementDrawer.DrawActivePartPlacement(m_pActivePartPmt.get());
        }
	}
	else if (m_iState == GRID_PARTPMT_PART_SELECTED)
	{
		if (m_pGridPartPmts->size() > 0) // 阵列已经生成了。
		{
			// 显示零件阵列。
			partPlacementDrawer.DrawActivePartPlacement(m_pGridPartPmts->at(0).get());
		}
		else
		{
			// 高亮要做阵列的零件。
			if (m_pActivePartPmt)
            {
				partPlacementDrawer.DrawSelectedPartPlacement(m_pActivePartPmt.get());
            }
		}
	}

	// 绘制光标。
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void PartPlacementGridAction::UpdateForUndoRedo()
{
	if (m_pActivePartPmt && !m_pPartPlacementList->GetPartPlacementByID(m_pActivePartPmt->GetID()))
	{
		m_pActivePartPmt.reset();
		m_iState = GRID_PARTPMT_NOTSTART;
		m_iMsgID = IDS_OUTPUT_GRID_SEL_PART;
	}
}

END_CUTLEADER_NAMESPACE()
