#include "StdAfx.h"
#include "MicroJointAddAction.h"

#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "PatternList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "PatternDrawer.h"
#include "IArrayPattern.h"

#include "LoopCutFeatureList.h"
#include "LoopDrawer.h"
#include "MicroJoint.h"
#include "LoopCutFeature.h"
#include "MicroJointAddCommand.h"
#include "MicroJointList.h"
#include "LoopMicroJoint.h"
#include "MicroJointMgr.h"
#include "clCutFeatureResource.h"
#include "LoopCutFeatureDrawer.h"
#include "LoopToolDataList.h"
#include "LoopToolManager.h"

#include "ParamConfig.h"
#include "MicroJointConfigItem.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

MicroJointAddAction::MicroJointAddAction(PatternListPtr pPatList, PatternLoopListPtr pPatternLoopList, LoopCutFeatureListPtr pLoopFeatureList, GlViewPortPtr pViewPort, ParamConfigPtr pParamConfig)
    : CutFeatureEditBase(pPatList, pPatternLoopList, pLoopFeatureList, pViewPort)
{
    m_iMsgID = IDS_CUTFEATURE_ASSIGN_MicroJoint;

	m_pParamConfig = pParamConfig;
	m_iPatIndex = INVALID_IDX;
	m_state = ADD_MICROJOINT_STATE_NONE;
}

MicroJointAddAction::~MicroJointAddAction(void)
{
}

BOOL MicroJointAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_state == ADD_MICROJOINT_STATE_NONE)
	{
		// 得到选中的回路。
		m_pSelectedLoop = m_pActiveLoop;
		if (!m_pSelectedLoop)
			return FALSE;

        UpdateNewPosition(Point2D(dPtX, dPtY)); // 新的微连接点
		m_state = ADD_MICROJOINT_STATE_SELECTLOOP;
	}
	else if (m_state == ADD_MICROJOINT_STATE_SELECTLOOP)
	{
		ParamConfigItemPtr pParamConfigItem1 = m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_CUT_MJ_ASSIGN);
		MicroJointConfigItemPtr pMicroJointConfigItem = boost::dynamic_pointer_cast<MicroJointConfigItem>(pParamConfigItem1);
		double dDflWidth = pMicroJointConfigItem->GetManualWidth();

		// get loop micro joint.
		LoopCutFeaturePtr pLoopFeature = m_pLoopFeatureList->FindByPatternLoop(m_pSelectedLoop);
		LoopMicroJointPtr pLoopMicroJoint = pLoopFeature->GetLoopMicroJoint();

		// 检查选中的位置能不能放置微连接。
		BOOL bDir;
		IPatternPtr pPattern = m_pSelectedLoop->GetPatternByLoopNodeIndex(m_iPatIndex, bDir);
		if (MicroJointMgr::GeomCanHoldMJ(pPattern, *(m_pMicroJointPt.get()), dDflWidth))
		{
			// create new micro joint.
			MicroJointPtr pMicroJoint = MicroJointMgr::BuildMicroJoint(pLoopMicroJoint, m_iPatIndex, *m_pMicroJointPt, dDflWidth, pMicroJointConfigItem);

			// create the command.
			EditorData editorData = GetEditorData();
			ICommandPtr pCommand(new MicroJointAddCommand(editorData, pLoopMicroJoint, pMicroJoint));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);
		}
		else
		{
			CString str, strProductName;
			strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_CUTFEATURE_GEOM_NOT_HOLD_MicroJoint);
			MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
		}
	}

	return TRUE;
}

BOOL MicroJointAddAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_state == ADD_MICROJOINT_STATE_NONE)
	{
		// 找到距离光标最近的回路。
		IPatternPtr pPattern = m_pPatList->GetPatternClosestTo(pt);
		if (pPattern)
        {
			m_pActiveLoop = m_pPatternLoopList->FindLoopByPat(pPattern.get());
        }
	} 
	else if (m_state == ADD_MICROJOINT_STATE_SELECTLOOP)
	{
		// 添加的微连接在回路上的位置。
        UpdateNewPosition(pt);
	}

	return TRUE;
}

BOOL MicroJointAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CutFeatureEditBase::OnKeyDown(nChar, nRepCnt, nFlags);

    if (ValueInputManager::GetInstance()->IsInputCancel(nChar) && m_state == ADD_MICROJOINT_STATE_SELECTLOOP)
    {
        m_state = ADD_MICROJOINT_STATE_NONE;
        m_iMsgID = IDS_CUTFEATURE_ASSIGN_MicroJoint;
        m_pSelectedLoop.reset();
    }

	return TRUE;
}

BOOL MicroJointAddAction::DrawBeforeFinish()
{
	if (m_state == ADD_MICROJOINT_STATE_NONE)
	{
		// draw the active loop in highlight mode.
		if (m_pActiveLoop)
		{
            const LoopToolData* pLoopToolData = m_pLoopFeatureList->GetLoopToolList()->GetLoopTool_by_loopID(m_pActiveLoop->GetID()).get();
			LoopDrawer loopDrawer(m_pViewPort);
            loopDrawer.DrawActiveLoop(m_pActiveLoop, LoopToolManager::GetLoopDisplayColor(pLoopToolData));
		}
	} 
	else if (m_state == ADD_MICROJOINT_STATE_SELECTLOOP)
	{
		// draw the selected loop in highlight mode.
        const LoopToolData* pLoopToolData = m_pLoopFeatureList->GetLoopToolList()->GetLoopTool_by_loopID(m_pSelectedLoop->GetID()).get();
		LoopDrawer loopDrawer(m_pViewPort);
		loopDrawer.DrawActiveLoop(m_pSelectedLoop, LoopToolManager::GetLoopDisplayColor(pLoopToolData));

		// 绘制微连接点。
		if (m_pMicroJointPt)
		{
			LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);
			loopFeatureDrawer.DrawPosRing(Point2D(m_pMicroJointPt->X(), m_pMicroJointPt->Y()));
		}
	}

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void MicroJointAddAction::UpdateNewPosition(const Point2D& cursorPos)
{
    if (m_pSelectedLoop->GetPatternLoopType() == PatternLoop_Array)
    {
        m_iPatIndex = 0;

        // get the first inst of array pattern.
        BOOL bDir;
        IPatternPtr pPattern = m_pSelectedLoop->GetPatternByLoopNodeIndex(m_iPatIndex, bDir);
        IArrayPatternPtr pArrayPattern = boost::dynamic_pointer_cast<IArrayPattern>(pPattern);
        IPatternPtr pFirstPat = pArrayPattern->GetFirstInstance();

        // get the closest point on the first inst.
        Point2D tmpPt;
        Matrix2D mat;
        tmpPt = pFirstPat->GetClosestPt(cursorPos, mat);
        m_pMicroJointPt.reset(new Point2D(tmpPt));
    }
    else
    {
        Point2D tmpPt;
        m_pSelectedLoop->GetClosestPt(cursorPos, tmpPt, m_iPatIndex, Matrix2D());
        m_pMicroJointPt.reset(new Point2D(tmpPt));
    }
}

END_CUTLEADER_NAMESPACE()
