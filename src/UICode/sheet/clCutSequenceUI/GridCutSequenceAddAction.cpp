#include "StdAfx.h"
#include "GridCutSequenceAddAction.h"

#include "baseConst.h"
#include "MathEx.h"
#include "IntegerList.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"
#include "ValueInputManager.h"
#include "Point2DList.h"
#include "LoopToolData.h"
#include "LoopCutFeature.h"
#include "PartCamData.h"
#include "PartItem.h"
#include "PartPlacementList.h"
#include "PartInstance.h"
#include "PartInstanceList.h"
#include "PartPlacement.h"
#include "PartPlacementDrawer.h"
#include "clCutSequenceResource.h"
#include "GridCutSequence.h"
#include "GridCutSequenceAddCommand.h"
#include "CutNodeList.h"
#include "CutSequenceList.h"
#include "CutSequenceDrawer.h"
#include "PartLayoutData.h"
#include "CutNodeManager.h"
#include "CutSequenceSelectAction.h"
#include "GridCutSequenceManager.h"
#include "SequenceConfigItem.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"


BEGIN_CUTLEADER_NAMESPACE()

GridCutSequenceAddAction::GridCutSequenceAddAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
									 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iState = NONE;
	m_iMsgID = IDS_SEQ_ADD_GRID_SEQ_TIP;
	m_pRectSelectPartPlacements.reset(new PartPlacementList);
}

GridCutSequenceAddAction::~GridCutSequenceAddAction(void)
{
}

BOOL GridCutSequenceAddAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_iState == NONE || m_iState == GRID_SEQ_SECOND_POINT)
	{
		m_iState = GRID_SEQ_FIRST_POINT;

		m_data[0] = dPtX;
		m_data[1] = dPtY;
		m_data[2] = dPtX;
		m_data[3] = dPtY;
	}

	return TRUE;
}

BOOL GridCutSequenceAddAction::LButtonUp(double& dPtX, double& dPtY)
{
	CString strProductName, strMsg;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	if (m_iState == GRID_SEQ_FIRST_POINT)
	{
		m_iState = GRID_SEQ_SECOND_POINT;

		// �õ�ѡ�е����ʵ����
		PartInstanceListPtr pSelectedPartInsts(new PartInstanceList);
		if (m_pRectSelectPartPlacements->size() > 0)
		{
			if (m_pRectSelectPartPlacements->GetPartIDList()->size() > 1) // ֻ��ѡ��һ�������
			{
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GRID_SEQ_ONE_PART_SEL_TIP);
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
				return TRUE;
			}

			for (unsigned int i = 0; i < m_pRectSelectPartPlacements->size(); i++)
			{
				PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances()->GetPartInstanceByPartPlacement(m_pRectSelectPartPlacements->at(i).get());
				pSelectedPartInsts->insert(pSelectedPartInsts->end(), pPartInstances->begin(), pPartInstances->end());
			}
		}
		else
		{
			if (m_pActivePartPlacement)
			{
				PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances()->GetPartInstanceByPartPlacement(m_pActivePartPlacement.get());
				pSelectedPartInsts->insert(pSelectedPartInsts->end(), pPartInstances->begin(), pPartInstances->end());
			}
			else
			{
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GRID_SEQ_NO_PART_SEL);
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
				return TRUE;
			}
		}

		// ���ѡ�е����ʵ���ǲ����е��Ѿ��й����ˡ�
		CutSequenceListPtr pExistSeqs = m_pCutSequences->GetCutSequencesByPartInstances(pSelectedPartInsts); // �õ�������Щ�����ʵ�����ϵĹ���
		if (pExistSeqs->size() > 0)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GRID_SEQ_EXIST_SEQ_TIP);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK|MB_ICONWARNING);
			return TRUE;
		}

		if (GridCutSequenceManager::CheckPartInsts_4_gridSeq(m_pConfigData, m_pViewPort, pSelectedPartInsts)) // ���Դ�������
		{
			RECT_CORNER iStartCorner = GridCutSequenceManager::CalcStartCornerOfGridSeq(m_pCutSequences, m_pConfigData, pSelectedPartInsts);
			PartCamDataPtr pPartCamData = pSelectedPartInsts->at(0)->GetPartPlacement()->GetPartItem()->GetCamData();
			double dToolWidth = pPartCamData->GetBoundaryLoopFeature()->GetLoopTool()->GetToolWidth();

			// ���ӹ���
			ICommandPtr pCommand(new GridCutSequenceAddCommand(GetEditorData(), m_pCutSequences, m_pConfigData, m_pPartLayoutData->GetPartInstances(), pSelectedPartInsts, iStartCorner, dToolWidth));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// ���¡��и�ڵ㡱���档
			RefreshCutNodes();
		}
	}

	return TRUE;
}

BOOL GridCutSequenceAddAction::MovePoint(double& dPtX, double& dPtY)
{
	Point2D pt(dPtX, dPtY);

	if (m_iState == GRID_SEQ_FIRST_POINT)
	{
		m_data[2] = dPtX;
		m_data[3] = dPtY;

		Rect2D rect;
		rect.SetXMin(m_data[0] < m_data[2] ? m_data[0] : m_data[2]);
		rect.SetXMax(m_data[0] < m_data[2] ? m_data[2] : m_data[0]);
		rect.SetYMin(m_data[1] < m_data[3] ? m_data[1] : m_data[3]);
		rect.SetYMax(m_data[1] < m_data[3] ? m_data[3] : m_data[1]);
		m_pRectSelectPartPlacements = m_pPartLayoutData->GetPartPlacements()->GetPartPlacementInRect(rect);
		m_pActivePartPlacement.reset();
	}
	else
	{
		m_pActivePartPlacement = m_pPartLayoutData->GetPartPlacements()->GetPartPlacementClosestTo(pt);
		m_pRectSelectPartPlacements->clear();
	}

	return TRUE;
}

BOOL GridCutSequenceAddAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL GridCutSequenceAddAction::DrawBeforeFinish()
{
	const DisplayParam* pDisplayParam = CLOptionInfo::GetInstance()->GetDisplayParam().get();
	PartPlacementDrawer partPlacementDrawer(m_pViewPort);
	CutSequenceDrawer cutSequenceDrawer(m_pViewPort);

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	if (m_iState == GRID_SEQ_FIRST_POINT) // ��ѡ��
	{
		// ���ƿ��
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

		// ���ƿ�ѡ����partpmt��
		for (unsigned int i = 0; i < m_pRectSelectPartPlacements->size(); i++)
		{
			partPlacementDrawer.DrawActivePartPlacement(m_pRectSelectPartPlacements->at(i).get());
		}
	}
	else
	{
		// ���Ƶ�ǰ��partpmt��
		if (m_pActivePartPlacement)
		{
			partPlacementDrawer.DrawActivePartPlacement(m_pActivePartPlacement.get());
		}
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void GridCutSequenceAddAction::UpdateForUndoRedo()
{

}

END_CUTLEADER_NAMESPACE()