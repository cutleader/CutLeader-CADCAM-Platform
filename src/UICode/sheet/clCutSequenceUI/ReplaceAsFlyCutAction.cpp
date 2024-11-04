#include "StdAfx.h"
#include "ReplaceAsFlyCutAction.h"

#include "baseConst.h"
#include "MathEx.h"
#include "IntegerList.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"
#include "Point2DList.h"
#include "Line2D.h"
#include "Polygon2D.h"

#include "IArrayPattern.h"
#include "IPatternLoop.h"
#include "PatternLoopNode.h"
#include "PatternLoopNodeList.h"
#include "LoopDrawer.h"
#include "LoopCutFeatureDrawer.h"
#include "PartItem.h"

#include "PartPlacementList.h"
#include "PartInstance.h"
#include "PartInstanceList.h"
#include "LoopInstanceList.h"
#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "PartPlacementManager.h"
#include "PartPlacementDrawer.h"
#include "LoopInstanceManager.h"

#include "clCutSequenceResource.h"
#include "FlyCutSequence.h"
#include "ReplaceAsFlyCutCommand.h"
#include "CutNodeList.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "PartLayoutData.h"
#include "CutNodeManager.h"
#include "CutSequenceSelectAction.h"
#include "ConfigData.h"
#include "FlyCutSequenceManager.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "SheetEditParam.h"

BEGIN_CUTLEADER_NAMESPACE()

ReplaceAsFlyCutAction::ReplaceAsFlyCutAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
									 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iState = NONE;
	m_iMsgID = IDS_SEQ_REPLACE_AS_FLYCUT_SEQ_TIP;
	m_pRectSelectLoopInstances.reset(new LoopInstanceList);

	// 得到所有“回路实例”。
	m_pAllLoopInstances = LoopInstanceManager::GetLoopInstanceList(m_pPartLayoutData->GetPartInstances());
}

ReplaceAsFlyCutAction::~ReplaceAsFlyCutAction(void)
{
}

BOOL ReplaceAsFlyCutAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_iState == NONE || m_iState == FLYCUT_SEQ_SECOND_POINT)
	{
		m_iState = FLYCUT_SEQ_FIRST_POINT;

		m_data[0] = dPtX;
		m_data[1] = dPtY;
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL ReplaceAsFlyCutAction::LButtonUp(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_iState == FLYCUT_SEQ_FIRST_POINT)
	{
		m_iState = FLYCUT_SEQ_SECOND_POINT;

		// 得到选中的回路实例。
		LoopInstanceListPtr pSelectedLoopInsts(new LoopInstanceList);
		if (m_pRectSelectLoopInstances->size() > 0)
		{
			pSelectedLoopInsts->insert(pSelectedLoopInsts->end(), m_pRectSelectLoopInstances->begin(), m_pRectSelectLoopInstances->end());
		}
		else
		{
			if (m_pActiveLoopInstance)
			{
				pSelectedLoopInsts->push_back(m_pActiveLoopInstance);
			}
			else
			{
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_FLYCUT_SEQ_NO_HOLE_SEL_TIP);
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
				return TRUE;
			}
		}

		// 检查选中的零件实例是不是有的已经有工序了。
		CutSequenceListPtr pExistSeqs_on_holes = m_pCutSequences->GetCutSequencesByLoopInstances(pSelectedLoopInsts.get()); // 得到加在这些“零件实例”上的工序。
		if (pExistSeqs_on_holes->size() == 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_FLYCUT_SEQ_EXIST_TIP);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
			return TRUE;
		}

		if (FlyCutSequenceManager::CheckLoopInsts_4_flyCutSeq(m_pConfigData, m_pViewPort, pSelectedLoopInsts)) // 可以创建工序。
		{
			// 是否需要应用到其他相同零件。
			bool bApplyTo_other_sameParts = false;
			PartInstancePtr pPartInst_current = pSelectedLoopInsts->at(0)->GetPartInstance();
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_FLYCUT_APPLY_SAMEPARTS_TIP);
			if (m_pPartLayoutData->GetPartInstances()->GetPartInstances_brother(pPartInst_current)->size() > 0 &&
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				bApplyTo_other_sameParts = true;
			}

			// 替换工序。
			ICommandPtr pCommand(new ReplaceAsFlyCutCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartInstances(), pSelectedLoopInsts, bApplyTo_other_sameParts));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// 更新“切割节点”缓存。
			RefreshCutNodes();
		}
	}

	return TRUE;
}

BOOL ReplaceAsFlyCutAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_iState == FLYCUT_SEQ_FIRST_POINT)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;

		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);
		m_pRectSelectLoopInstances = LoopInstanceManager::GetLoopInstsInRect(m_pAllLoopInstances, rect);
		m_pActiveLoopInstance.reset();
	}
	else
	{
		m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInstEx(m_pAllLoopInstances, pt);
		m_pRectSelectLoopInstances->clear();
	}

	return TRUE;
}

BOOL ReplaceAsFlyCutAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL ReplaceAsFlyCutAction::DrawBeforeFinish()
{
	const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();
	LoopDrawer loopDrawer(m_pViewPort);

	// 老的设置。
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	// 绘制选中的回路。
	m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());
	if (m_iState == FLYCUT_SEQ_FIRST_POINT) // 框选。
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
			m_pViewPort->DrawRect(m_data[0], m_data[1], m_data[2], m_data[3]);

			// restore the drawer
			m_pViewPort->SetLineStipple(bOldStipple, iOldRepeat, iOldPattern);
		}

		// 绘制框选到的loop inst。
		for (unsigned int i = 0; i < m_pRectSelectLoopInstances->size(); i++)
		{
			LoopInstancePtr pLoopInstance = m_pRectSelectLoopInstances->at(i);
			loopDrawer.DrawLoopData(pLoopInstance->GetCacheData()->GetPatternLoop(), pLoopInstance->GetTransformMatrix());
		}
	}
	else
	{
		// 绘制当前的loop inst。
		if (m_pActiveLoopInstance)
		{
			loopDrawer.DrawLoopData(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), m_pActiveLoopInstance->GetTransformMatrix());
		}
	}

	// 恢复设置。
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void ReplaceAsFlyCutAction::UpdateForUndoRedo()
{
	// 更新“切割节点”缓存。
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
