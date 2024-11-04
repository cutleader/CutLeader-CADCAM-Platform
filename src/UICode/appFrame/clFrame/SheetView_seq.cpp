#include "StdAfx.h"
#include "SheetView.h"
#include "SheetDocument.h"

#include "UIManager.h"
#include "CommandManager.h"
#include "InteractBar.h"
#include "clBaseDataResource.h"
#include "versionInfo.h"
#include "clSecureResource.h"
#include "CommandList.h"
#include "ProgressDlg.h"
#include "LogMgr.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "LeadConfigItem.h"
#include "ParamConfig.h"
#include "SequenceConfigItem.h"
#include "ConfigData.h"

#include "PartPlacementAddPanel.h"
#include "clSheetResource.h"
#include "clCutSequenceResource.h"
#include "CutSequencePane.h"
#include "SequenceEditor.h"
#include "CutSequenceList.h"
#include "SequenceSortDlg.h"
#include "NCViewBar.h"
#include "PartInstanceList.h"
#include "SheetProcessor.h"
#include "Sheet.h"
#include "SheetSequenceData.h"
#include "AutoSequenceTask.h"
#include "PartLayoutEditor.h"
#include "CutSequenceDeleteCommand.h"
#include "PartPlacementSelectAction.h"
#include "LoopInstanceManager.h"
#include "clCutFeatureResource.h"


BEGIN_CUTLEADER_NAMESPACE()


void SheetView::OnAutoSequence()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	// �Զ����乤��
    std::set<LONGLONG> patternLoops_failedToOffset; // �˴���¼ƫ��ʧ�ܵļ���������
    {
        SheetPtr pSheet = GetCurrentSheet();

        // check whether all part inst have been assigned sequence.
        PartInstanceListPtr pPartInstances = pSheet->GetPartInstances();
        SheetSequenceDataPtr pSheetSequenceData = pSheet->GetSheetSequenceData();
        PartInstanceListPtr pUnCutPartInstanceList = pSheetSequenceData->GetUnSequencedPartInstances(pPartInstances.get());
        if (pUnCutPartInstanceList->size() == 0)
        {
            CString str, strProductName;
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_ALL_INST_ASSIGN_SEQ);
            strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            ::MessageBox(m_hWnd, str, strProductName, MB_OK | MB_ICONINFORMATION);

            // �������ʵ��ѡ������
            {
                SwitchEditor(m_pPartLayoutEditor);
                HideLayoutEditorPane();
                m_pPartLayoutEditor->SetEditAction(EDIT_SELECTPRTPMT);
            }

            return;
        }

        // �Զ�����֮ǰ�������ݵ��޸�ʱ�����
        LONGLONG iLatestModifyTimeStamp_pre = pSheet->GetSheetSequenceData()->GetLatestModifyTimeStamp();

        // create a task to auto sequence.
        AutoSequenceTaskPtr pAutoSeqTask(new AutoSequenceTask(pSheet));
        CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_AUTO_ASSIGN_SEQ);
        ProgressDlg progressDlg;
        progressDlg.Init(str, pAutoSeqTask);
        progressDlg.DoModal();
        patternLoops_failedToOffset = pAutoSeqTask->GetPatternLoops_failedToOffset();

        // �Զ�����֮�������ݵ��޸�ʱ�����
        LONGLONG iLatestModifyTimeStamp_post = pSheet->GetSheetSequenceData()->GetLatestModifyTimeStamp();
        if (iLatestModifyTimeStamp_post > iLatestModifyTimeStamp_pre) // ����仯�ˣ������undo/redo�б���Ϊauto-sequence��֧��undo��
        {
            CommandManager::GetInstance()->GetUndoCommands()->clear();
            CommandManager::GetInstance()->GetRedoCommands()->clear();
        }

        // notify all observers of sequence unit.
        CutSequenceListPtr pSeqUnitList = pSheetSequenceData->GetCutSequences();
        pSeqUnitList->NotifyAllObserver(INVALID_ID, VARIANT());
    }

	// �������ʵ��ѡ������
    {
        SwitchEditor(m_pPartLayoutEditor);
        HideLayoutEditorPane();
        m_pPartLayoutEditor->SetEditAction(EDIT_SELECTPRTPMT);

        // �����ѡ��action����ʾƫ��ʧ�ܵļ���������
        if (patternLoops_failedToOffset.size() > 0)
        {
            // ���ĳЩ�����������˲����ʵĵ��߳ߴ磬������ʾ�û���
            CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LoopHaveWrongToolSize);
            MessageBox(str, strProductName, MB_OK | MB_ICONWARNING);

            // ��ʾƫ��ʧ�ܵļ���������
            if (PartPlacementSelectActionPtr pPartPlacementSelectAction = boost::dynamic_pointer_cast<PartPlacementSelectAction>(m_pPartLayoutEditor->GetActionObject()))
            {
                SheetPtr pSheet = GetCurrentSheet();
                PartInstanceListPtr pPartInstances = pSheet->GetPartInstances();
                LoopInstanceListPtr pAllLoopInstances = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
                pPartPlacementSelectAction->GetWrongToolSizeLoopInstances()->Init(pAllLoopInstances.get(), patternLoops_failedToOffset);
            }
            else
            {
                LogMgr::DebugWarn(_T("623107"));
            }
        }
    }

	Invalidate();
}

void SheetView::OnClearAllSequence()
{
    CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
    SheetPtr pSheet = GetCurrentSheet();
    CutSequenceListPtr pAllCutSequences = pSheet->GetSheetSequenceData()->GetCutSequences();
    if (pAllCutSequences->size() == 0) // if no sequence exist, warn user.
    {
        MessageBox(MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_NO_CUTSEQ_EXIST), strProductName, MB_OK | MB_ICONINFORMATION);
    }
    else
    {
        if (MessageBox(MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_CLEAR_ALL), strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            // ִ������������й���
            CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ);
            EditorData editorData(CWnd::FromHandle(m_pViewPort->GetWnd()), MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ));
            LeadConfigItemPtr pLeadConfigItem = boost::dynamic_pointer_cast<LeadConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_LEAD_ASSIGN));
            SequenceConfigItemPtr pSeqConfigItem = boost::dynamic_pointer_cast<SequenceConfigItem>(m_pParamConfig->GetItemByType(PARAMCONFIG_ITEM_GENERATE_SEQ));
            ConfigDataPtr pConfigData(new ConfigData(pLeadConfigItem, pSeqConfigItem, m_pMaterialSize));
            CutSequenceListPtr pCutSequenceList_willDelete(new CutSequenceList);
            pCutSequenceList_willDelete->insert(pCutSequenceList_willDelete->end(), pAllCutSequences->begin(), pAllCutSequences->end());
            ICommandPtr pCommand(new CutSequenceDeleteCommand(editorData, pAllCutSequences, pConfigData, pCutSequenceList_willDelete));
            pCommand->Do();
            CommandManager::GetInstance()->AddCommand(pCommand);

            // ����ѡ���������
            OnPartPmtSelect();
        }
    }
}

void SheetView::OnSelectSequence()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	// ���빤��ѡ������
	m_pSequenceEditor->SetAction(SEQUENCE_SELECT);
}

void SheetView::OnSortSequence()
{
	SequenceSortDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		// �л��༭����
		SwitchEditor(m_pSequenceEditor);

		if (dlg.IsSortFromStart())
			m_pSequenceEditor->SetAction(SEQUENCE_SORT_A);
		else
			m_pSequenceEditor->SetAction(SEQUENCE_SORT_B);
	}
}

void SheetView::OnReplaceHoleSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_REPLACE_HOLE_SEQ);
}

void SheetView::OnAddBdgeSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_ADD_BDGESEQ);
}

void SheetView::OnAddChainSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_ADD_CHAINSEQ);
}

void SheetView::OnAddGridSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_ADD_GRIDSEQ);
}

void SheetView::OnAddFlyCutSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_REPLACE_AS_FLYCUT);
}

void SheetView::OnInsertChainNode()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_INSERT_CHAINNODE);
}

void SheetView::OnDelChainNode()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_DEL_CHAINNODE);
}

void SheetView::OnInsertResetInstruction()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_ADD_RESET_INSTRUCT);
}

void SheetView::OnDelResetInstruction()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_DEL_RESET_INSTRUCT);
}

void SheetView::OnBreakBdgeSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_BREAK_BDGESEQ);
}

void SheetView::OnBdgeStartLoop()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_BDGE_START_CHANGE);
}

void SheetView::OnAddLoopSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_ADD_LOOPSEQ);
}

void SheetView::OnInsertLoopSeq()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_INSERT_LOOPSEQ);
}

void SheetView::OnRemoveSequence()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_REMOVE);
}

void SheetView::OnAddMovePoint()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_ADD_MOV_PT);
}

void SheetView::OnDelMovePoint()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_DEL_MOV_PT);
}

void SheetView::OnAutoRemCutline()
{
	SwitchEditor(m_pSequenceEditor);
	m_pSequenceEditor->SetAction(SEQUENCE_AUTO_REM_CUTLINE);
}

void SheetView::OnAddRemCutline()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_ADD_REM_CUTLINE);
}

void SheetView::OnDelRemCutline()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_DEL_REM_CUTLINE);
}

void SheetView::OnViewSequence()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	m_pSequenceEditor->SetAction(SEQUENCE_VIEW);
}

void SheetView::OnDeleteLoopSequence()
{
    // �л��༭����
    SwitchEditor(m_pSequenceEditor);

    m_pSequenceEditor->SetAction(SEQUENCE_Remove_LoopSequence);
}

void SheetView::OnRealTimeCut()
{
    // �л��༭����
    SwitchEditor(m_pSequenceEditor);

    m_pSequenceEditor->SetAction(SEQUENCE_RealTime_Cut);
}

void SheetView::OnShowSeqUnitPane()
{
	// �л��༭����
	SwitchEditor(m_pSequenceEditor);

	if (!CutSequencePane::GetInstance()->IsVisible())
	{
		SheetSequenceDataPtr pSheetSequenceData = GetCurrentSheet()->GetSheetSequenceData();
		CutSequencePane::GetInstance()->DisplaySeqUnit(pSheetSequenceData->GetCutSequences(), m_pSequenceEditor);
	}
	else
		CutSequencePane::GetInstance()->ShowBar(FALSE);
}

void SheetView::OnUpdateSeqUnitPane(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(CutSequencePane::GetInstance()->IsVisible());
}


END_CUTLEADER_NAMESPACE()
