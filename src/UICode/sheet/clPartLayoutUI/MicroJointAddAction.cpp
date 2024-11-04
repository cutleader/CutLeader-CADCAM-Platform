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

#include "ParamConfig.h"
#include "MicroJointConfigItem.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

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
#include "LoopToolManager.h"

#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "PartInstanceList.h"
#include "LoopInstanceManager.h"
#include "LoopInstanceList.h"
#include "MicroJointsAddCommand.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

MicroJointAddAction::MicroJointAddAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
										 GlViewPortPtr pViewPort, ParamConfigPtr pParamConfig, SpreadSamePartDlg* pSpreadSamePartDlg)
    : PartPlacementEditAction(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
    m_iMsgID = IDS_CUTFEATURE_ASSIGN_MicroJoint;
	m_pParamConfig = pParamConfig;
	m_pSpreadSamePartDlg = pSpreadSamePartDlg;
	m_iPatIndex = INVALID_IDX;
	m_state = ADD_MICROJOINT_STATE_NONE;
	m_pAllLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
}

MicroJointAddAction::~MicroJointAddAction(void)
{
}

BOOL MicroJointAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_state == ADD_MICROJOINT_STATE_NONE)
	{
		// 得到选中的“回路实例”。
		m_pSelectedLoopInst = m_pActiveLoopInstance;
		if (!m_pSelectedLoopInst)
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
		LoopMicroJointPtr pSelectedLoopMicroJoint = m_pSelectedLoopInst->GetCacheData()->GetLoopMicroJoint();

		// check whether the geom can hold the micro joint.
		BOOL bDir;
		IPatternLoopPtr pSelectedLoop = m_pSelectedLoopInst->GetCacheData()->GetPatternLoop();
		IPatternPtr pPattern = pSelectedLoop->GetPatternByLoopNodeIndex(m_iPatIndex, bDir);
		if (MicroJointMgr::GeomCanHoldMJ(pPattern, *m_pMicroJointPt, dDflWidth))
		{
			if (m_pSpreadSamePartDlg->IsSpread())
			{
				// 需要新增的微连接数据
				vector<pair<LoopMicroJointPtr, MicroJointPtr>> microJointDataList;
				for (unsigned int i = 0; i < m_pAllLoopInstanceList->size(); i++)
				{
					LoopInstancePtr pLoopInstance =  m_pAllLoopInstanceList->at(i);
					if (pLoopInstance->GetCacheData()->GetPatternLoop()->GetID() == pSelectedLoopMicroJoint->GetPatternLoop()->GetID())
					{
						LoopMicroJointPtr pLoopMicroJoint = pLoopInstance->GetCacheData()->GetLoopMicroJoint();
						MicroJointPtr pMicroJoint_new = MicroJointMgr::BuildMicroJoint(pLoopMicroJoint, m_iPatIndex, *m_pMicroJointPt, dDflWidth, pMicroJointConfigItem);
						microJointDataList.push_back(make_pair(pLoopMicroJoint, pMicroJoint_new));
					}
				}

				// 执行命令
				EditorData editorData = GetEditorData();
				ICommandPtr pCommand(new MicroJointsAddCommand(editorData, microJointDataList));
				pCommand->Do();
				CommandManager::GetInstance()->AddCommand(pCommand);
			}
			else
			{
				// create new micro joint.
				MicroJointPtr pMicroJoint = MicroJointMgr::BuildMicroJoint(pSelectedLoopMicroJoint, m_iPatIndex, *m_pMicroJointPt, dDflWidth, pMicroJointConfigItem);

				// create the command.
				EditorData editorData = GetEditorData();
				ICommandPtr pCommand(new MicroJointAddCommand(editorData, pSelectedLoopMicroJoint, pMicroJoint));
				pCommand->Do();
				CommandManager::GetInstance()->AddCommand(pCommand);
			}
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

void MicroJointAddAction::AddMicroJointOnSelectedLoopInstance(LoopInstancePtr pSelectedLoopInstance)
{
    m_pActiveLoopInstance = pSelectedLoopInstance;
    m_pSelectedLoopInst = pSelectedLoopInstance;
    m_state = ADD_MICROJOINT_STATE_SELECTLOOP;
}

BOOL MicroJointAddAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_state == ADD_MICROJOINT_STATE_NONE)
	{
		// 找到距离光标最近的回路。
		m_pActiveLoopInstance = LoopInstanceManager::GetClosestLoopInst(m_pAllLoopInstanceList, Point2D(dPtX, dPtY));
	} 
	else if (m_state == ADD_MICROJOINT_STATE_SELECTLOOP)
	{
        // 新的微连接点
        UpdateNewPosition(Point2D(dPtX, dPtY));
	}

	return TRUE;
}

BOOL MicroJointAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL MicroJointAddAction::DrawBeforeFinish()
{
	if (m_state == ADD_MICROJOINT_STATE_NONE)
	{
		// draw the active loop in highlight mode.
		if (m_pActiveLoopInstance)
		{
			LoopDrawer loopDrawer(m_pViewPort);
            COLORREF iColor = LoopToolManager::GetLoopDisplayColor(m_pActiveLoopInstance->GetCacheData()->GetLoopTool().get());
			loopDrawer.DrawActiveLoop(m_pActiveLoopInstance->GetCacheData()->GetPatternLoop(), iColor, m_pActiveLoopInstance->GetTransformMatrix());
		}
	} 
	else if (m_state == ADD_MICROJOINT_STATE_SELECTLOOP)
	{
		// draw the selected loop in highlight mode.
		LoopDrawer loopDrawer(m_pViewPort);
        COLORREF iColor = LoopToolManager::GetLoopDisplayColor(m_pSelectedLoopInst->GetCacheData()->GetLoopTool().get());
		loopDrawer.DrawActiveLoop(m_pSelectedLoopInst->GetCacheData()->GetPatternLoop(), iColor, m_pSelectedLoopInst->GetTransformMatrix());

		// 绘制微连接点。
		if (m_pMicroJointPt)
		{
			LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);
			Point2D pt = *m_pMicroJointPt;
			pt *= m_pSelectedLoopInst->GetTransformMatrix();
			loopFeatureDrawer.DrawPosRing(Point2D(pt.X(), pt.Y()));
		}
	}

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void MicroJointAddAction::UpdateNewPosition(const Point2D& cursorPos)
{
    IPatternLoopPtr pSelectedLoop = m_pSelectedLoopInst->GetCacheData()->GetPatternLoop();

    // 添加的微连接在回路上的位置。
    if (pSelectedLoop->GetPatternLoopType() == PatternLoop_Array)
    {
        m_iPatIndex = 0;

        // get the first inst of array pattern.
        BOOL bDir;
        IPatternPtr pPattern = pSelectedLoop->GetPatternByLoopNodeIndex(m_iPatIndex, bDir);
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
        Matrix2D loopInstMat = m_pSelectedLoopInst->GetTransformMatrix();

        // 得到添加微连接的位置，这个位置是板材下的坐标。
        Point2D tmpPt;
        pSelectedLoop->GetClosestPt(cursorPos, tmpPt, m_iPatIndex, loopInstMat);

        // 得到零件坐标系下的微连接的位置。
        loopInstMat.Inverse();
        tmpPt *= loopInstMat;
        m_pMicroJointPt.reset(new Point2D(tmpPt));
    }
}

END_CUTLEADER_NAMESPACE()
