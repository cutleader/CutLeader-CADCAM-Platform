#include "StdAfx.h"
#include "PartPlacementSelectAction.h"

#include "boost/bind.hpp"
#include "Point2D.h"
#include "MathEx.h"
#include "baseConst.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "clUILibResource.h"
#include "DataBaseManager.h"
#include "ValueInputManager.h"

#include "SnapPtDrawer.h"
#include "LoopToolData.h"
#include "LoopCutFeature.h"
#include "PartCamData.h"
#include "PartItem.h"

#include "PartPlacementList.h"
#include "PartPlacement.h"
#include "PartInstanceList.h"
#include "PartPlacementDrawer.h"
#include "clPartLayoutResource.h"
#include "PartPlacementPropertyBar.h"
#include "PartPlacementDeleteCommand.h"
#include "PartPlacementCopyCommand.h"
#include "PartPlacementTransformCommand.h"
#include "PartPlacementManager.h"
#include "PartPlacementMoveAction.h"
#include "PartPlacementRotateAction.h"
#include "PartGroup.h"
#include "PartGroupList.h"
#include "PartLayoutHelper.h"

#include "ConfigItemLoader.h"
#include "SequenceConfigItem.h"

#include "DisplayParam.h"
#include "SheetEditParam.h"
#include "CLOptionInfo.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementSelectAction::PartPlacementSelectAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
										 GlViewPortPtr pViewPort, DistanceDetectDlg* pDisDetectDlg, SelectPartOrLoopActionBar* pSelectPartOrLoopActionBar)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
    m_pActivePartPlacements.reset(new PartPlacementList);
    m_pSelectedPartPlacements.reset(new PartPlacementList);
	m_state = PARTPMT_SELECT_NONE;
	m_iMsgID = IDS_OUTPUT_SELECTPRTPMT_EDIT;
	m_pBasePartPlacements.reset(new PartPlacementList);
	m_bCopying = FALSE;
    m_pDisDetectDlg = pDisDetectDlg;
    m_pSelectPartOrLoopActionBar = pSelectPartOrLoopActionBar;

    UpdateOuterRects_ofAllPartPlacements();

    m_pWrongToolSizeLoopInstances.reset(new WrongToolSizeLoopInstances);
}

PartPlacementSelectAction::~PartPlacementSelectAction(void)
{
}

void PartPlacementSelectAction::UpdateOuterRects_ofAllPartPlacements()
{
    for (unsigned int i = 0; i < m_pPartPlacementList->size(); i++)
    {
        PartPlacementPtr pPartPlacement = m_pPartPlacementList->at(i);
        m_rects_of_partPmt[pPartPlacement->GetID()] = pPartPlacement->GetRectBox();
    }
}

BOOL PartPlacementSelectAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_bCopying)
	{
        // 克隆零件放置后，也克隆相关的零件组，不过只需克隆顶层零件组。
        PartGroupListPtr pPartGroupList_new = PartPlacementManager::ClonePartGroups_whenClonePartPlacements_2(m_pPartGroupList.get(),
            m_pSelectedPartPlacements.get(), m_pMovePartPlacements.get());

        // 所有要新建的零件分组
        PartGroupListPtr pPartGroups_willAdded(new PartGroupList);
        for (unsigned int i = 0; i < pPartGroupList_new->size(); i++)
        {
            PartGroupPtr pPartGroup = pPartGroupList_new->operator [](i);
            pPartGroups_willAdded->push_back(pPartGroup);
            PartGroupListPtr pPartGroups_tmp = pPartGroup->GetSubPartGroupsRecursively();
            pPartGroups_willAdded->insert(pPartGroups_willAdded->end(), pPartGroups_tmp->begin(), pPartGroups_tmp->end());
        }

		// 复制“零件放置”对象。
		PartPlacementListPtr pNewPartPlacements(new PartPlacementList);
		pNewPartPlacements->insert(pNewPartPlacements->end(), m_pMovePartPlacements->begin(), m_pMovePartPlacements->end());
		ICommandPtr pCommand(new PartPlacementCopyCommand(GetEditorData(), m_pPartPlacementList, m_pPartInstanceList, m_pPartGroupList, pNewPartPlacements, pPartGroups_willAdded));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// 选中新建的“零件放置”。
		m_pSelectedPartPlacements->clear();
		m_pSelectedPartPlacements->insert(m_pSelectedPartPlacements->end(), pNewPartPlacements->begin(), pNewPartPlacements->end());
		PartPlacementPropertyBar::GetInstance()->DisplayPartPmt(m_pSelectedPartPlacements, m_pPartInstanceList, m_pViewPort);

		// 重置变量。
		m_bCopying = FALSE;
		m_bDisplayValInput = FALSE;
		m_pMovePartPlacements->clear();
        UpdateOuterRects_ofAllPartPlacements();
	}
	else
	{
		if (m_state == PARTPMT_SELECT_NONE || m_state == PARTPMT_SELECT_SECOND_POINT)
		{
			m_state = PARTPMT_SELECT_FIRST_POINT;
			m_iMsgID = IDS_OUTPUT_SELECTPRTPMT_NEXTPT;

			m_data[0] = dPtX;
			m_data[1] = dPtY;
			m_data[2] = dPtX;
			m_data[3] = dPtY;
		}
	}

	return TRUE;
}

BOOL PartPlacementSelectAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_bCopying)
		return TRUE;

	if (m_state == PARTPMT_SELECT_FIRST_POINT)
	{
		m_state = PARTPMT_SELECT_SECOND_POINT;
		m_iMsgID = IDS_OUTPUT_SELECTPRTPMT_EDIT;

		// 要是没有按住ctrl键，就需要清除现有选择集。
		if (!ValueInputManager::GetInstance()->IsInputCtrl())
        {
			m_pSelectedPartPlacements->clear();
        }

		if (m_pActivePartPlacements->size() > 0)
		{
			m_pSelectedPartPlacements->insert(m_pSelectedPartPlacements->end(), m_pActivePartPlacements->begin(), m_pActivePartPlacements->end());

            // 看看要不要选中相同零件。
            if (m_pSelectPartOrLoopActionBar->GetIsAutoSelectSamePartsOrLoops())
            {
                for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
                {
                    const PartPlacement* pPartPlacement = m_pSelectedPartPlacements->operator [](i).get();
                    PartPlacementListPtr pPartPlacements_samePart = m_pPartPlacementList->GetPartPlacementByPart(pPartPlacement->GetPartItem()->GetPartID());
                    for (unsigned int j = 0; j < pPartPlacements_samePart->size(); j++)
                    {
                        PartPlacementPtr pPartPlacement_samePart = pPartPlacements_samePart->operator [](j);
                        if (!m_pSelectedPartPlacements->GetPartPlacementByID(pPartPlacement_samePart->GetID()))
                        {
                            m_pSelectedPartPlacements->push_back(pPartPlacement_samePart);
                        }
                    }
                }
            }

			PartPlacementPropertyBar::GetInstance()->DisplayPartPmt(m_pSelectedPartPlacements, m_pPartInstanceList, m_pViewPort);
		}
	}

	return TRUE;
}

BOOL PartPlacementSelectAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_bCopying)
	{
		Point2D currentPt(dPtX, dPtY);

		// 移动的过程中更新一些数据。
		UpdateWhenMov(currentPt);
	}
	else
	{
		if (m_state == PARTPMT_SELECT_FIRST_POINT)
		{
			m_data[2] = dPtX;
			m_data[3] = dPtY;

			Rect2D rect;
			rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
			rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
			rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
			rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);
			m_pActivePartPlacements = PartLayoutHelper::GetPartPlacements_byRectSelection(rect, m_pPartPlacementList.get(), m_pPartGroupList.get(), m_rects_of_partPmt);
		}
		else
		{
			Point2D pt(dPtX, dPtY);
            m_pActivePartPlacements = PartLayoutHelper::GetPartPlacements_byClickSelection(pt, m_pPartPlacementList.get(), m_pPartGroupList.get(), m_rects_of_partPmt);
		}
	}

	return TRUE;
}

BOOL PartPlacementSelectAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	PartPlacementEditAction::OnKeyDown(nChar, nRepCnt, nFlags);

	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	SheetEditParamPtr pSheetEditParam = pCLOptionInfo->GetSheetEditParam();

	double d1, d2;
	if (ValueInputManager::GetInstance()->IsInputEnd(nChar))
	{
		if (!ValueInputManager::GetInstance()->GetInput(d1, d2))
			m_pViewPort->GetCursorPos(d1, d2);

		ConvertKBInput(d1, d2);
		MovePoint(d1, d2);
		LButtonDown(d1, d2);
	}
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
	{
		m_pSelectedPartPlacements->clear();
		m_pSelectedPartPlacements->insert(m_pSelectedPartPlacements->end(), m_pPartPlacementList->begin(), m_pPartPlacementList->end());
		PartPlacementPropertyBar::GetInstance()->DisplayPartPmt(m_pSelectedPartPlacements, m_pPartInstanceList, m_pViewPort);
	}
	else if (ValueInputManager::GetInstance()->IsInputDel(nChar))
	{
		DeleteSelectedPartPlacements();
	}
	// 步进移动选中的零件。
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputUpArrow(nChar))
		StepMove(POSITION_UP);
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputDownArrow(nChar))
		StepMove(POSITION_DOWN);
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputLeftArrow(nChar))
		StepMove(POSITION_LEFT);
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputRightArrow(nChar))
		StepMove(POSITION_RIGHT);
	// 快速移动选中的零件。
	else if (ValueInputManager::GetInstance()->IsInputUpArrow(nChar))
		RapidMove(POSITION_UP);
	else if (ValueInputManager::GetInstance()->IsInputDownArrow(nChar))
		RapidMove(POSITION_DOWN);
	else if (ValueInputManager::GetInstance()->IsInputLeftArrow(nChar))
		RapidMove(POSITION_LEFT);
	else if (ValueInputManager::GetInstance()->IsInputRightArrow(nChar))
		RapidMove(POSITION_RIGHT);
	//
	else if (ValueInputManager::GetInstance()->IsInputAdd(nChar))
	{
		if (!m_bCopying)
		{
			Rect2D rect = m_pSelectedPartPlacements->GetRectBox();
			Matrix2D mat;
			mat.RotateBy(rect.GetCenterPt(), -pSheetEditParam->GetRotateStep()*D2R);
			for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
				m_pSelectedPartPlacements->at(i)->Transform(mat);
			m_pPartPlacementList->SetModified(TRUE);

			if (m_pSelectedPartPlacements->size() == 1 && PartPlacementPropertyBar::GetInstance()->IsVisible())
				PartPlacementPropertyBar::GetInstance()->UpdatePmtProp();
		}
		else
		{
			UpdateWhenRotate(FALSE);
		}
	}
	else if (ValueInputManager::GetInstance()->IsInputDes(nChar))
	{
		if (!m_bCopying)
		{
			Rect2D rect = m_pSelectedPartPlacements->GetRectBox();
			Matrix2D mat;
			mat.RotateBy(rect.GetCenterPt(), pSheetEditParam->GetRotateStep()*D2R);
			for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
				m_pSelectedPartPlacements->at(i)->Transform(mat);
			m_pPartPlacementList->SetModified(TRUE);

			if (m_pSelectedPartPlacements->size() == 1 && PartPlacementPropertyBar::GetInstance()->IsVisible())
				PartPlacementPropertyBar::GetInstance()->UpdatePmtProp();
		}
		else
		{
			UpdateWhenRotate(TRUE);
		}
	}
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputC(nChar))
	{
		if (m_pSelectedPartPlacements->size() > 0)
		{
			m_pBasePartPlacements->clear();
			m_pBasePartPlacements->insert(m_pBasePartPlacements->end(), m_pSelectedPartPlacements->begin(), m_pSelectedPartPlacements->end());
		}
		else
		{
			CString str, strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_SELECT_PRTPMT_COPY);
			MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
		}
	}
	else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputV(nChar))
	{
		if (m_pBasePartPlacements->size() > 0) // 拷贝缓冲区中有没有“零件放置”对象。
		{
			// 克隆新的“零件放置”集合。
			PartPlacementListPtr pMovePartPlacements(new PartPlacementList);
			for (unsigned int i = 0; i < m_pBasePartPlacements->size(); i++)
				pMovePartPlacements->push_back(PartPlacementManager::ClonePartPmt(m_pBasePartPlacements->at(i).get()));
			m_pMovePartPlacements = pMovePartPlacements;

			m_bCopying = TRUE;
			m_movCenterPt = m_pBasePartPlacements->GetRectBox().GetCenterPt();

			// 是否要为智能靠碰准备数据。
			SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
			if (pSheetEditParam->EnableDisDetect())
			{
                m_pMaterialSize = m_pMatSize;
				m_pFixPartInstances = m_pPartInstanceList;
                m_partAttachConfig = m_pDisDetectDlg->GetPartAttachConfig();
				UpdateBeforeMov();
			}

			// 隐藏属性面板。
			PartPlacementPropertyBar::GetInstance()->ShowBar(FALSE);
		}
	}
    else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputQ(nChar)) // 试图移动
    {
        if (m_pSelectedPartPlacements->size() > 0)
        {
            PartPlacementMoveActionPtr pPartPlacementMoveAction(new PartPlacementMoveAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroupList, m_pSelectedPartPlacements, m_pViewPort, m_pDisDetectDlg));
            m_pNextAction = pPartPlacementMoveAction;
        }
        else
        {
            CString str, strProductName;
            strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_SELECT_PRTPMT_MOVE);
            MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
        }
    }
    else if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputW(nChar)) // 试图旋转
    {
        if (m_pSelectedPartPlacements->size() > 0)
        {
            PartPlacementRotateActionPtr pPartPlacementRotateAction(new PartPlacementRotateAction(m_pMatSize, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroupList, m_pSelectedPartPlacements, m_pViewPort, m_pDisDetectDlg));
            m_pNextAction = pPartPlacementRotateAction;
        }
        else
        {
            CString str, strProductName;
            strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_OUTPUT_SELECT_PRTPMT_ROTATE);
            MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
        }
    }

	ValueInputManager::GetInstance()->AcceptInputChar(nChar);

	return TRUE;
}

BOOL PartPlacementSelectAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();
	PartPlacementDrawer partPlacementDrawer(m_pViewPort);

	if (m_bCopying) // 正在快捷拷贝。
	{
		// 绘制选中的“零件放置”。
		partPlacementDrawer.DrawSelectedPartPlacements(m_pSelectedPartPlacements.get());

		// 绘制移动中的“零件放置”。
		if (m_pStickRemRegion)
		{
			for (unsigned int i = 0; i < m_pMovePartPlacements->size(); i++)
            {
				partPlacementDrawer.DrawActivePartPlacement(m_pMovePartPlacements->at(i).get());
            }
		}
		else
		{
			partPlacementDrawer.DrawPartPlacements(m_pMovePartPlacements.get());
		}

		// draw cursor.
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);
	}
	else
	{
		// 老的设置
		COLORREF dwOldColor;
		m_pViewPort->GetDrawColor(dwOldColor);
		int iOldLineWid = m_pViewPort->GetLineWidth();

		// 绘制选中的“零件放置”。
		partPlacementDrawer.DrawSelectedPartPlacements(m_pSelectedPartPlacements.get());

		if (m_state == PARTPMT_SELECT_FIRST_POINT) // 框选。
		{
			// 绘制框框
			{
				// backup the old draw config.
				bool bOldStipple = false;
				int iOldRepeat = 1;
				unsigned short iOldPattern = 0xffff;
				m_pViewPort->GetLineStipple(bOldStipple, iOldRepeat, iOldPattern);

				// draw the selection rectangle.
				m_pViewPort->SetLineStipple(true, 2, 0xcccc);
				m_pViewPort->DrawRect(m_data[0], m_data[1], m_data[2], m_data[3]);

				// restore the drawer
				m_pViewPort->SetLineStipple(bOldStipple, iOldRepeat, iOldPattern);
			}
		}

        // 高亮零件放置
        for (unsigned int i = 0; i < m_pActivePartPlacements->size(); i++)
        {
            const PartPlacement* pActivePartPlacement = m_pActivePartPlacements->at(i).get();
            if (!m_pSelectedPartPlacements->GetPartPlacementByID(pActivePartPlacement->GetID())) // 要是它已在选择集中，就不用高亮方式画之。
            {
                partPlacementDrawer.DrawActivePartPlacement(pActivePartPlacement);
            }
        }

		// draw cursor.
		m_pViewPort->SetDrawColor(*pDisplayParam->GetPatColor());
		CursorDrawer::DrawAimSelCursor(m_pViewPort);

		// 恢复设置
		m_pViewPort->SetDrawColor(dwOldColor);
		m_pViewPort->SetLineWidth(iOldLineWid);
	}

    // 绘制偏移有问题的轮廓实例。
    if (pDisplayParam->GetShowIncorrectToolSize())
    {
        m_pWrongToolSizeLoopInstances->Draw(m_pViewPort);
    }

	return TRUE;
}

void PartPlacementSelectAction::UpdateForUndoRedo()
{
	m_pSelectedPartPlacements->clear();
	m_pActivePartPlacements->clear();
    UpdateOuterRects_ofAllPartPlacements();
	PartPlacementPropertyBar::GetInstance()->ShowBar(FALSE);
}

void PartPlacementSelectAction::SelectPartPlacements(PartPlacementListPtr pPartPlacements, BOOL bShowPropBar)
{
	m_pSelectedPartPlacements = pPartPlacements;
	m_state = PARTPMT_SELECT_SECOND_POINT;

	// whether need to show prop bar.
	PartPlacementPropertyBar* pPartPmtPropBar = PartPlacementPropertyBar::GetInstance();
	if (bShowPropBar && !pPartPmtPropBar->IsVisible())
		pPartPmtPropBar->ShowPane(TRUE, FALSE, TRUE);

	// if prop bar is visible, we should update it.
	if (pPartPmtPropBar->IsVisible())
		pPartPmtPropBar->DisplayPartPmt(m_pSelectedPartPlacements, m_pPartInstanceList, m_pViewPort);
}

void PartPlacementSelectAction::SelectPartPlacement(PartPlacementPtr pPartPlacement, BOOL bShowPropBar)
{
    m_pSelectedPartPlacements->clear();
    m_pSelectedPartPlacements->push_back(pPartPlacement);
    m_state = PARTPMT_SELECT_SECOND_POINT;

    // whether need to show prop bar.
    PartPlacementPropertyBar* pPartPmtPropBar = PartPlacementPropertyBar::GetInstance();
    if (bShowPropBar && !pPartPmtPropBar->IsVisible())
        pPartPmtPropBar->ShowPane(TRUE, FALSE, TRUE);

    // if prop bar is visible, we should update it.
    if (pPartPmtPropBar->IsVisible())
        pPartPmtPropBar->DisplayPartPmt(m_pSelectedPartPlacements, m_pPartInstanceList, m_pViewPort);
}

void PartPlacementSelectAction::DeleteSelectedPartPlacements()
{
	if (m_pSelectedPartPlacements->size() > 0)
	{
		// create "Delete Pattern" command
		EditorData editorData = GetEditorData();
		ICommandPtr pCommand(new PartPlacementDeleteCommand(editorData, m_pPartPlacementList, m_pPartInstanceList, m_pPartGroupList, m_pSelectedPartPlacements));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// clear selected part pmts.
		m_pSelectedPartPlacements->clear();

		// 隐藏零件属性界面，这个隐藏动作会激发mousemove事件，这样就会更新m_pActivePartPlacement变量。
		PartPlacementPropertyBar::GetInstance()->ShowBar(FALSE);
	}
}

void PartPlacementSelectAction::StepMove(POSITION_INFO iDir)
{
	if (m_pSelectedPartPlacements->size() == 0)
		return;

	// 确定矩阵。
	Matrix2D mat;
	SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
	if (iDir == POSITION_UP)
	{
		Vector2D vect(0, pSheetEditParam->GetMoveStep());
		mat.SetTransfer(vect);
	}
	else if (iDir == POSITION_DOWN)
	{
		Vector2D vect(0, -pSheetEditParam->GetMoveStep());
		mat.SetTransfer(vect);
	}
	else if (iDir == POSITION_LEFT)
	{
		Vector2D vect(-pSheetEditParam->GetMoveStep(), 0);
		mat.SetTransfer(vect);
	}
	else if (iDir == POSITION_RIGHT)
	{
		Vector2D vect(pSheetEditParam->GetMoveStep(), 0);
		mat.SetTransfer(vect);
	}

	// 移动零件。
	for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
		m_pSelectedPartPlacements->at(i)->Transform(mat);
	m_pPartPlacementList->SetModified(TRUE);

	// 更新界面。
	if (m_pSelectedPartPlacements->size() == 1 && PartPlacementPropertyBar::GetInstance()->IsVisible())
		PartPlacementPropertyBar::GetInstance()->UpdatePmtProp();
}

void PartPlacementSelectAction::RapidMove(POSITION_INFO iDir)
{
	if (m_pSelectedPartPlacements->size() == 0)
		return;

	// 按照快速移动的先后顺序对选中的零件放置进行排序。
	sort(m_pSelectedPartPlacements->begin(), m_pSelectedPartPlacements->end(), boost::bind(&PartPlacement::BeforePartPmt, _1, _2, iDir));

	// 得到不移动的零件放置和实例。
	PartPlacementListPtr pFixPartPmts(new PartPlacementList);
	PartInstanceListPtr pFixPartInsts(new PartInstanceList);
	for (unsigned int i = 0; i < m_pPartPlacementList->size(); i++)
	{
		PartPlacementPtr pPartPlacement = m_pPartPlacementList->at(i);
		if (!m_pSelectedPartPlacements->GetPartPlacementByID(pPartPlacement->GetID()))
		{
			pFixPartPmts->push_back(pPartPlacement);
			PartInstanceListPtr pPartInstances = m_pPartInstanceList->GetPartInstanceByPartPlacement(pPartPlacement.get());
			pFixPartInsts->insert(pFixPartInsts->end(), pPartInstances->begin(), pPartInstances->end());
		}
	}

    // 零件靠碰参数
    SheetEditParamPtr pSheetEditParam = CLOptionInfo::GetInstance()->GetSheetEditParam();
    PartAttachConfig partAttachConfig(pSheetEditParam->EnableComDetect(), pSheetEditParam->GetPartDis(), pSheetEditParam->GetSheetMargin());

	// 按照顺序一个一个的快速移动“零件放置”。
	PartPlacementListPtr pOldPartPmts(new PartPlacementList);
	PartPlacementListPtr pNewPartPlacements(new PartPlacementList);
	for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
	{
		PartPlacementPtr pMovPartPmt = m_pSelectedPartPlacements->at(i);
		PartInstanceListPtr pMovPartInsts = m_pPartInstanceList->GetPartInstanceByPartPlacement(pMovPartPmt.get());

		// 把零件放置沿着一个方向做快速移动。
		Matrix2D retMoveMat;
		BOOL bRet = RapidMovePartPmt(m_pMatSize, partAttachConfig, pFixPartPmts, pFixPartInsts, iDir, pMovPartPmt, pMovPartInsts, retMoveMat);
		if (bRet) // 如果快速移动成功。
		{
			// 老的零件。
			pOldPartPmts->push_back(pMovPartPmt);

			// 新的零件。
			PartPlacementPtr pNewPartPlacement = boost::dynamic_pointer_cast<PartPlacement>(pMovPartPmt->Clone());
			pNewPartPlacement->Transform(retMoveMat);
			pNewPartPlacements->push_back(pNewPartPlacement);

			// 更新不移动的零件放置和实例。
			pFixPartPmts->push_back(pNewPartPlacement);
			pFixPartInsts->AddPartInstanceByPartPlacement(pNewPartPlacement);
		}
		else // 不成功的话需要把老的零件放回去。
		{
			pFixPartPmts->push_back(pMovPartPmt);
			pFixPartInsts->insert(pFixPartInsts->end(), pMovPartInsts->begin(), pMovPartInsts->end());
		}
	}

	// 提交修改。
	if (pNewPartPlacements->size() > 0)
	{
		// 执行命令。
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMAND_MOVE_PARTPMT);
		ICommandPtr pCommand(new PartPlacementTransformCommand(str, GetEditorData(), m_pPartPlacementList, pOldPartPmts, pNewPartPlacements));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// 更新界面。
		if (m_pSelectedPartPlacements->size() == 1 && PartPlacementPropertyBar::GetInstance()->IsVisible())
			PartPlacementPropertyBar::GetInstance()->UpdatePmtProp();
	}
}

END_CUTLEADER_NAMESPACE()
