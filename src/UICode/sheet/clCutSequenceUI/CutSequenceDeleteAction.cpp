#include "StdAfx.h"
#include "CutSequenceDeleteAction.h"

#include "boost/bind.hpp"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "ValueInputManager.h"
#include "clBaseDataResource.h"
#include "clCutSequenceResource.h"
#include "CutSequenceDeleteCommand.h"
#include "CutSequenceList.h"
#include "CutSequenceSelectAction.h"


BEGIN_CUTLEADER_NAMESPACE()

CutSequenceDeleteAction::CutSequenceDeleteAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, 
									 ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, ApplyPartDlg* pApplyPartDlg)
	: CutSequencePreSelectAction(pPartLayoutData, pCutSequences, pConfigData, pViewPort, pApplyPartDlg)
{
	m_iMsgID = IDS_SEQ_DELETE_SEQUENCE;

	// if some sequence unit were pre-selected, disable CutSequencePreSelectAction.
	if (m_pSelectCutSequences->size() > 0)
    {
		m_iPreSelectSequenceActionState = PreSelectSequenceAction_inactive;
    }
}

CutSequenceDeleteAction::~CutSequenceDeleteAction(void)
{
}

BOOL CutSequenceDeleteAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_iPreSelectSequenceActionState == PreSelectSequenceAction_active)
	{
		CutSequencePreSelectAction::LButtonDown(dPtX, dPtY);
	}

	// ɾ��ѡ�еĹ���
	if (m_pSelectCutSequences->size() > 0)
	{
		// �Ȱѹ�����˳����һ����
		sort(m_pSelectCutSequences->begin(), m_pSelectCutSequences->end(), boost::bind(&IsSeqBefore, _1, _2, m_pCutSequences));

		// ִ�����
		ICommandPtr pCommand(new CutSequenceDeleteCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pSelectCutSequences));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// ���ѡ�еĹ���
		m_pCurrentCutSequences->clear();
		m_pSelectCutSequences->clear();

		// ������������˹���ġ���·ʵ������
		m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();

		// ���¡��и�ڵ㡱���档
		RefreshCutNodes();
	}
	else
	{
		CString strProductName, strMsg;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_NO_SEQ_SELECT);
		MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
	}

	return TRUE;
}

BOOL CutSequenceDeleteAction::MovePoint(double& dPtX, double& dPtY)
{
	CutSequencePreSelectAction::MovePoint(dPtX, dPtY);

	return TRUE;
}

BOOL CutSequenceDeleteAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CutSequenceDeleteAction::DrawBeforeFinish()
{
	return __super::DrawBeforeFinish();
}

void CutSequenceDeleteAction::UpdateForUndoRedo()
{
	// ������������˹���ġ���·ʵ������
	m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();

	m_pCurrentCutSequences->clear();
	m_pSelectCutSequences->clear();
}

void CutSequenceDeleteAction::SelectSeqUnits(CutSequenceListPtr pCutSequences)
{
	ICommandPtr pCommand(new CutSequenceDeleteCommand(GetEditorData(), m_pCutSequences, m_pConfigData, pCutSequences));
	pCommand->Do();
	CommandManager::GetInstance()->AddCommand(pCommand);

	// ������������˹���ġ���·ʵ������
	m_pAllSequencedLoopInstances = m_pCutSequences->GetAllLoopInstances();

	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

END_CUTLEADER_NAMESPACE()
