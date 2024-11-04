#include "StdAfx.h"
#include "LoopSequenceDeleteAction.h"

#include "boost/bind.hpp"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "clBaseDataResource.h"
#include "clCutSequenceResource.h"
#include "CutSequenceDeleteCommand.h"
#include "CutSequenceList.h"
#include "CutSequenceSelectAction.h"
#include "sampleCodeResource.h"


BEGIN_CUTLEADER_NAMESPACE()


LoopSequenceDeleteAction::LoopSequenceDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, 
									 ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg)
	: CutSequencePreSelectAction(pPartLayoutData, pCutSequences, pConfigData, pViewPort, pApplyPartDlg)
{
	m_iMsgID = IDS_Delete_LoopSequence_TIP;

	// if some sequence unit were pre-selected, disable CutSequencePreSelectAction.
	if (m_pSelectCutSequences->size() > 0)
    {
		m_iPreSelectSequenceActionState = PreSelectSequenceAction_inactive;
    }
}

LoopSequenceDeleteAction::~LoopSequenceDeleteAction(void)
{
}

BOOL LoopSequenceDeleteAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_iPreSelectSequenceActionState == PreSelectSequenceAction_active)
	{
		CutSequencePreSelectAction::LButtonDown(dPtX, dPtY);
	}

    // 判断是不是选中了一个轮廓工序
    if (m_pSelectCutSequences->size() != 1 || m_pSelectCutSequences->at(0)->GetCutSequenceType() != CutSequence_Loop)
    {
        CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
        CString strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_Select_One_LoopSequence);
        MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);

        // 清空选中的工序。
        m_pCurrentCutSequences->clear();
        m_pSelectCutSequences->clear();

        return FALSE;
    }

	// 删除选中的工序
	{
		// 执行命令。
		ICommandPtr pCommand(new CutSequenceDeleteCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pSelectCutSequences));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// 清空选中的工序。
		m_pCurrentCutSequences->clear();
		m_pSelectCutSequences->clear();

		// 更新所有添加了工序的“回路实例”。
		m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();

		// 更新“切割节点”缓存。
		RefreshCutNodes();
	}

	return TRUE;
}

BOOL LoopSequenceDeleteAction::MovePoint(double& dPtX, double& dPtY)
{
	CutSequencePreSelectAction::MovePoint(dPtX, dPtY);

	return TRUE;
}

BOOL LoopSequenceDeleteAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CutSequencePreSelectAction::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL LoopSequenceDeleteAction::DrawBeforeFinish()
{
	CutSequencePreSelectAction::DrawBeforeFinish();

	return TRUE;
}

void LoopSequenceDeleteAction::UpdateForUndoRedo()
{
	// 缓存所有添加了工序的“回路实例”。
	m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();

	// 更新“切割节点”缓存。
	RefreshCutNodes();

	m_pCurrentCutSequences->clear();
	m_pSelectCutSequences->clear();
}

END_CUTLEADER_NAMESPACE()
