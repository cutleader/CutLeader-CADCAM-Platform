#include "StdAfx.h"
#include "BridgeSequenceBreakAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "LineArc2DList.h"
#include "BasicGeometryDrawer.h"
#include "ValueInputManager.h"

#include "LoopInstanceList.h"

#include "clCutSequenceResource.h"
#include "CutNodeList.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "CutNodeManager.h"
#include "BridgeSequence.h"
#include "BridgeSequenceBreakCommand.h"
#include "BridgeSequenceManager.h"
#include "PartLayoutData.h"
#include "CutSequenceSelectAction.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

BridgeSequenceBreakAction::BridgeSequenceBreakAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
									   : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iMsgID = IDS_SEQ_BREAK_BDGE;

	m_pCurBdgeGeom.reset();
	m_pCurBdgeNode.reset();
}

BridgeSequenceBreakAction::~BridgeSequenceBreakAction(void)
{
}

BOOL BridgeSequenceBreakAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pCurBdgeNode)
	{
		// �õ���Ӧ�ġ��Žӹ��򡱡�
		BridgeSequencePtr pModBdgeSeqUnit;
		CutSequenceListPtr pBdgeSeqUnits = m_pCutSequences->GetCutSequencesByType(CutSequence_Bridge);
		for (unsigned int i = 0; i < pBdgeSeqUnits->size(); i++)
		{
			BridgeSequencePtr pBridgeSequence = boost::dynamic_pointer_cast<BridgeSequence>(pBdgeSeqUnits->at(i));
			if (pBridgeSequence->IncludeBdgeNode(m_pCurBdgeNode))
			{
				pModBdgeSeqUnit = pBridgeSequence;
				break;
			}
		}

		// �ڡ�����Žӵ㡱���Ͽ����Žӽڵ㡱��
		ICommandPtr pCommand(new BridgeSequenceBreakCommand(GetEditorData(), m_pCutSequences, m_pConfigData,
											    pModBdgeSeqUnit, m_pCurBdgeNode));
		pCommand->Do();
		CommandManager::GetInstance()->AddCommand(pCommand);

		// ���¡��и�ڵ㡱���档
		RefreshCutNodes();

		// ���µ�ǰ�ġ�����š�����ͼ�μ���Ӧ�ġ��Žӽڵ㡱��
		m_pCurBdgeGeom.reset();
		m_pCurBdgeNode.reset();
	}
	else
	{
		CString strProductName, strMsg;
		strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_BREAK_BDGE_NO_SEL);
		MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
	}

	return TRUE;
}

BOOL BridgeSequenceBreakAction::MovePoint(double& dPtX, double& dPtY)
{
	// �������������ġ�����š�����ͼ�μ���Ӧ�ġ��Žӽڵ㡱��
	Point2D pt(dPtX, dPtY);
	CalcCurBdgeGeomInfo(pt);

	return TRUE;
}

BOOL BridgeSequenceBreakAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		// figure out the next action.
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL BridgeSequenceBreakAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();
	BasicGeometryDrawer geomBaseDrawer(m_pViewPort);
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	// �������еġ��Žӹ��򡱣���ʱ������ƫ�ơ�
	m_pViewPort->SetDrawColor(*pDisplayParam->GetSequenceColor());
	CutSequenceListPtr pBdgeSeqUnits = m_pCutSequences->GetCutSequencesByType(CutSequence_Bridge);
	cutSequenceDrawer.DrawCutSequences(pBdgeSeqUnits.get(), m_pCutNodeList.get());

	// ���Ƶ�ǰ���š���
	if (m_pCurBdgeGeom)
	{
		m_pViewPort->SetDrawColor(*pDisplayParam->GetActiveObjColor());
		geomBaseDrawer.Draw2DLineArcs(m_pCurBdgeGeom.get());
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void BridgeSequenceBreakAction::UpdateForUndoRedo()
{
	// ���¡��и�ڵ㡱���档
	RefreshCutNodes();
}

void BridgeSequenceBreakAction::CalcCurBdgeGeomInfo(const Point2D& pt)
{
	// �õ����С��Žӽڵ㡱�����ġ�����š��Ļ���ͼ�Ρ�
	vector<pair<BridgeNodePtr, LineArc2DListPtr> > bdgeGeomInfos = BridgeSequenceManager::GetAllBdgeGeomInfo(m_pCutSequences);

	// �õ������������ġ��Ż���ͼ�Ρ���
	double dDis = INFINITE_VAL;
	int iIndex = INVALID_IDX;
	for (unsigned int i = 0; i < bdgeGeomInfos.size(); i++)
	{
		pair<BridgeNodePtr, LineArc2DListPtr> bdgeGeomInfo = bdgeGeomInfos.at(i);
		double dTmpDis = bdgeGeomInfo.second->GetClosestDis(pt);
		if (dTmpDis < dDis)
		{
			dDis = dTmpDis;
			iIndex = i;
		}
	}

	if (iIndex != INVALID_IDX)
	{
		m_pCurBdgeGeom = bdgeGeomInfos[iIndex].second;
		m_pCurBdgeNode = bdgeGeomInfos[iIndex].first;
	}
}

END_CUTLEADER_NAMESPACE()
