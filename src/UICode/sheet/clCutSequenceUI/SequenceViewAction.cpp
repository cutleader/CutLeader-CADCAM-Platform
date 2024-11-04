#include "StdAfx.h"
#include "SequenceViewAction.h"

#include "DBConnect.h"
#include "DataBaseManager.h"
#include "GlViewPort.h"
#include "Line2D.h"
#include "LineArc2DList.h"
#include "Rect2D.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "ProgressDlg.h"
#include "clBaseDataResource.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "LeadConfigItem.h"
#include "ParamConfig.h"
#include "ParamConfigLoader.h"

#include "LoopDrawer.h"
#include "PartPlacementList.h"
#include "PartInstanceList.h"
#include "LoopInstanceList.h"
#include "LoopInstance.h"
#include "PartPlacementManager.h"
#include "clCutSequenceResource.h"
#include "ICutPartSequence.h"
#include "CutSequenceList.h"
#include "SequenceSimulateShapeList.h"
#include "CutNodeManager.h"
#include "CutNodeList.h"
#include "CutNodeManager.h"
#include "CutNodeSimulator.h"
#include "PartLayoutData.h"
#include "CutSequenceDrawer.h"
#include "CutSequenceSelectAction.h"
#include "CutSequenceManager.h"
#include "GenerateCutNodeTask.h"
#include "ConfigData.h"
#include "clCutFeatureResource.h"


BEGIN_CUTLEADER_NAMESPACE()

SequenceViewAction::SequenceViewAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
	: SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_bDisplayValInput = FALSE;
	m_pSequenceSimulateShapeList.reset();
}

SequenceViewAction::~SequenceViewAction(void)
{
}

void SequenceViewAction::Start()
{
	const IMaterialSize* pMaterialSize = m_pConfigData->GetMatSize();
	const SequenceConfigItem* pSeqConfigItem = m_pConfigData->GetSeqConfigItem();

	// �ѡ��и��ת��Ϊ���и�ڵ㡱��
	CutNodeListPtr pCutNodeList;
	Point2D startSeqPt = pMaterialSize->GetOuterRect().GetCornerPt(pSeqConfigItem->GetStartCorner());
    std::set<LONGLONG> patternLoops_failedToOffset; // �˴���¼ƫ��ʧ�ܵļ���������
	if (pSeqConfigItem->IsSoftwareOffset())
	{
		// �õ�һЩר�ҿ����
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pSeqConfigItem->GetParentID());
		LeadConfigItemPtr pLeadConfigItem = boost::dynamic_pointer_cast<LeadConfigItem>(pParamConfig->GetItemByType(PARAMCONFIG_ITEM_LEAD_ASSIGN));

		// ����һ���������ɡ��и�ڵ㡱��
        {
            // init the task.
            PartPlacementListPtr pPartPlacements = m_pPartLayoutData->GetPartPlacements();
            PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances();
            GenerateCutNodeTaskPtr pGenCutNodeTask(new GenerateCutNodeTask(startSeqPt, pPartPlacements, pPartInstances, m_pCutSequences, pSeqConfigItem->IsCheckIntf(), pLeadConfigItem));

            // display the progress of the task.
            CString str;
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEN_CUTNODE_TASK);
            ProgressDlg progressDlg;
            progressDlg.Init(str, pGenCutNodeTask);
            progressDlg.DoModal();
            patternLoops_failedToOffset = pGenCutNodeTask->GetPatternLoops_failedToOffset();

            // ���ĳЩ�����������˲����ʵĵ��߳ߴ磬������ʾ�û���
            if (patternLoops_failedToOffset.size() > 0)
            {
                CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
                CString str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_LoopHaveWrongToolSize);
                MessageBox(m_pViewPort->GetWnd(), str, strProductName, MB_OK | MB_ICONWARNING);
            }

            // �õ������
            pCutNodeList = pGenCutNodeTask->GetCutNodes();
        }
	}
	else
	{
		pCutNodeList = CutNodeManager::CutSequences2CutNodes(startSeqPt, m_pCutSequences.get());
	}

	// ���ɡ�����ģ����״�����ϡ�
	m_pSequenceSimulateShapeList = CutNodeManager::CutNode2SimShape(pCutNodeList.get());

	// ��ʼ�����������и�ڵ�ģ��������
	m_pCutNodeSimulator = CutNodeSimulator::GetInstance();
	m_pCutNodeSimulator->Init(m_pViewPort, pCutNodeList, m_pSequenceSimulateShapeList);
	m_pCutNodeSimulator->StartSimulation();
}

BOOL SequenceViewAction::LButtonDown(double& dPtX, double& dPtY)
{
	// check whether we need to pause/resume the simulation.
	if (m_pCutNodeSimulator->IsPaused())
		m_pCutNodeSimulator->ResumeSimulation();
	else
		m_pCutNodeSimulator->PauseSimulation();

	return TRUE;
}

BOOL SequenceViewAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		// send "stop" message to simulator.
		if (!m_pCutNodeSimulator->IsFinished())
			m_pCutNodeSimulator->StopSimulation();
	}
	else if (ValueInputManager::GetInstance()->IsInputAdd(nChar))
	{
		// send "speed-up" message to simulator.
		if (!m_pCutNodeSimulator->IsFinished())
			m_pCutNodeSimulator->SpeedUp();
	}
	else if (ValueInputManager::GetInstance()->IsInputDes(nChar))
	{
		// send "speed-down" message to simulator.
		if (!m_pCutNodeSimulator->IsFinished())
			m_pCutNodeSimulator->SpeedDown();
	}

	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL SequenceViewAction::DrawBeforeFinish()
{
    const DisplayParam* pColorInfo = CLOptionInfo::GetInstance()->GetDisplayParam().get();
    CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// set the prompt message.
	if (m_pCutNodeSimulator->IsPaused())
		m_iMsgID = IDS_SEQ_VIEW_PAUSE;
	else if (m_pCutNodeSimulator->IsFinished())
		m_iMsgID = IDS_SEQ_VIEW_FINISH;
	else
		m_iMsgID = GetMsgBySpeed(m_pCutNodeSimulator->GetSpeed());

	// �ϵ����á�
	DWORD dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	// ����ģ��ͼ�Ρ�
	SequenceSimulateShapeListPtr pSeqSimShapeList = m_pCutNodeSimulator->GetSimulatedShapes();
	m_pViewPort->SetDrawColor(*pColorInfo->GetSeqSimColor());
	m_pViewPort->SetLineWidth(2);
	cutSequenceDrawer.DrawSequenceSimulateShapes(pSeqSimShapeList.get(), false);

	// �ָ���ͼ���á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

int SequenceViewAction::GetMsgBySpeed(int iSpeed)
{
	int iMsgID;

	if (iSpeed == 1)
		iMsgID = IDS_SEQ_VIEW_SPEED1;
	else if (iSpeed == 2)
		iMsgID = IDS_SEQ_VIEW_SPEED2;
	else if (iSpeed == 3)
		iMsgID = IDS_SEQ_VIEW_SPEED3;
	else if (iSpeed == 4)
		iMsgID = IDS_SEQ_VIEW_SPEED4;
	else if (iSpeed == 5)
		iMsgID = IDS_SEQ_VIEW_SPEED5;
	else if (iSpeed == 6)
		iMsgID = IDS_SEQ_VIEW_SPEED6;
	else if (iSpeed == 7)
		iMsgID = IDS_SEQ_VIEW_SPEED7;
	else if (iSpeed == 8)
		iMsgID = IDS_SEQ_VIEW_SPEED8;

	return iMsgID;
}

END_CUTLEADER_NAMESPACE()
