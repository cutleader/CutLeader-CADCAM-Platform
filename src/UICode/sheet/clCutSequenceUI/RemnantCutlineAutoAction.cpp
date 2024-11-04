#include "StdAfx.h"
#include "RemnantCutlineAutoAction.h"

#include <map>
#include "DBConnect.h"
#include "DataBaseManager.h"
#include "baseConst.h"
#include "GlViewPort.h"
#include "CursorDrawer.h"
#include "Point2DList.h"
#include "Polygon2D.h"
#include "Polygon2DList.h"
#include "Line2D.h"
#include "Line2DList.h"
#include "LineArc2DList.h"
#include "ValueInputManager.h"
#include "clBaseDataResource.h"
#include "LogMgr.h"

#include "IMaterialSize.h"
#include "PolygonMaterial.h"
#include "MaterialLibWriter.h"
#include "SequenceConfigItem.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"

#include "PartPlacementList.h"
#include "PartPlacementManager.h"
#include "PartPlacementEditAction.h"
#include "PartLayoutData.h"
#include "RemnantLineSeq.h"
#include "CutSequenceList.h"
#include "CutSequenceSelectAction.h"
#include "CutNodeManager.h"
#include "clCutSequenceResource.h"
#include "ConfigData.h"
#include "CutSequenceManager.h"
#include "CutSequenceDrawer.h"


using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

RemnantCutlineAutoAction::RemnantCutlineAutoAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort, GenerateRemCutLineDlg* pGenerateRemCutLineDlg)
								 : SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_iMsgID = IDS_SEQ_AUTO_REM_CUTLINE_ACTION;
	m_pRemCutLines.reset(new Line2DList);

	// ��ʼ���Ի���
	{
		m_pGenerateRemCutLineDlg = pGenerateRemCutLineDlg;
		m_pGenerateRemCutLineDlg->AddObserver(this);

		// ��ʼ�����Ϸ�λ
		RECT_SIDE iRemnantPosition;
		Rect2D sheetRect = m_pConfigData->GetMatSize()->GetOuterRect();
		Point2D sheetCenterPt = sheetRect.GetCenterPt();
		Rect2D partsRect = m_pPartLayoutData->GetPartPlacements()->GetRectBox();
		Point2D partsCenterPt = partsRect.GetCenterPt();
		if (fabs(sheetCenterPt.Y() - partsCenterPt.Y()) > fabs(sheetCenterPt.X() - partsCenterPt.X())) // �Ӱ���ϲ���²�ȡ����
		{
			if (sheetCenterPt.Y() > partsCenterPt.Y()) // ���ϲ�ȡ����
			{
				iRemnantPosition = RECT_TOP;
			}
			else // ���²�ȡ����
			{
				iRemnantPosition = RECT_BOTTOM;
			}
		}
		else // �Ӱ�������Ҳ�ȡ����
		{
			if (sheetCenterPt.X() < partsCenterPt.X()) // �����ȡ����
			{
				iRemnantPosition = RECT_LEFT;
			}
			else // ���Ҳ�ȡ����
			{
				iRemnantPosition = RECT_RIGHT;
			}
		}

		// ��ʼ�Ĺ������
		double dBeamWidth;
		if (iRemnantPosition == RECT_TOP || iRemnantPosition == RECT_BOTTOM)
		{
			dBeamWidth = sheetRect.GetWidth() / 2.0;
		}
		else if (iRemnantPosition == RECT_LEFT || iRemnantPosition == RECT_RIGHT)
		{
			dBeamWidth = sheetRect.GetHeight() / 2.0;
		}

		m_pGenerateRemCutLineDlg->Init(iRemnantPosition, dBeamWidth);
	}
}

RemnantCutlineAutoAction::~RemnantCutlineAutoAction(void)
{
	m_pGenerateRemCutLineDlg->RemoveObserver(GetID());
}

BOOL RemnantCutlineAutoAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	SequenceEditActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL RemnantCutlineAutoAction::DrawBeforeFinish()
{
	// prepare some data.
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	DisplayParamPtr pDisplayParam = pCLOptionInfo->GetDisplayParam();

	// �ϵ����á�
	COLORREF dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);
	int iOldLineWid = m_pViewPort->GetLineWidth();

	// ���������и���
	m_pViewPort->SetDrawColor(*pDisplayParam->GetSequenceColor());
    m_pViewPort->SetLineWidth(1);
	for (unsigned int i = 0; i < m_pRemCutLines->size(); i++)
	{
		Line2DPtr pLine2D = m_pRemCutLines->at(i);
		const Point2D& startPt = pLine2D->GetStartPoint();
		const Point2D& endPt = pLine2D->GetEndPoint();
		m_pViewPort->DrawLinePoint2D(startPt.X(), startPt.Y(), endPt.X(), endPt.Y());
	}

	// �ָ����á�
	m_pViewPort->SetDrawColor(dwOldColor);
	m_pViewPort->SetLineWidth(iOldLineWid);

	// draw cursor.
	CursorDrawer::DrawCrossLineCursor(m_pViewPort);

	return TRUE;
}

void RemnantCutlineAutoAction::Update(int iEventID, VARIANT varEventContent)
{
	CString strMsg, strProductName;
	strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);

	// ���
	if (m_pPartLayoutData->GetPartPlacements()->size() == 0)
	{
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_NO_PART_EXIST);
		MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		return;
	}

	if (m_pGenerateRemCutLineDlg->GetCmdType() == GenerateRemCutLineDlg_previewCmd || // Ԥ��
		m_pGenerateRemCutLineDlg->GetCmdType() == GenerateRemCutLineDlg_generateCmd) // ���������и���
	{
		// �ù��յķ�ʽ�õ������и��ߡ�
		RECT_SIDE iRayDir = m_pGenerateRemCutLineDlg->GetRayDir();
		double dOffsetDistance = m_pGenerateRemCutLineDlg->GetOffsetDistance();
		int iEvalFactor = BeamWidth_2_BeamNum(iRayDir, m_pGenerateRemCutLineDlg->GetStepWidth());
		Line2DListPtr pRemCutLines = PartPlacementManager::CalcRemCutLine(m_pPartLayoutData->GetPartPlacements(), m_pConfigData->GetMatSize(), iRayDir, iEvalFactor, dOffsetDistance);

		// ��ȡ�и���
		m_pRemCutLines->clear();
		if (pRemCutLines->size() > 0)
		{
			m_pRemCutLines->insert(m_pRemCutLines->end(), pRemCutLines->begin(), pRemCutLines->end());
			CWnd::FromHandle(m_pViewPort->GetWnd())->Invalidate();

			// ���������и���
			if (m_pGenerateRemCutLineDlg->GetCmdType() == GenerateRemCutLineDlg_generateCmd)
			{
				// �Ƿ�Ҫ����˳���и��ߵ��и���и��ߵĿ�ʼ��Ҫ�����ĵ����е��һЩ��
				const SequenceConfigItem* pSeqConfigItem = m_pConfigData->GetSeqConfigItem();
				Point2D startCutPt_of_wholeSheet = m_pConfigData->GetMatSize()->GetOuterRect().GetCornerPt(pSeqConfigItem->GetStartCorner());
				if (startCutPt_of_wholeSheet.DistanceTo(m_pRemCutLines->front()->GetStartPoint()) > startCutPt_of_wholeSheet.DistanceTo(m_pRemCutLines->back()->GetEndPoint()))
				{
					m_pRemCutLines->Reverse();
				}

				RemnantLineSeqPtr pRemnantLineSeq(new RemnantLineSeq);
				Line2DListPtr pLines = pRemnantLineSeq->GetCutLines();
				pLines->insert(pLines->end(), m_pRemCutLines->begin(), m_pRemCutLines->end());
				CutSequenceManager::InsertCutSeq(m_pCutSequences, -1, pRemnantLineSeq, m_pConfigData->GetMatSize(), m_pConfigData->GetSeqConfigItem());
				m_pCutSequences->SetModified(TRUE);
				RefreshCutNodes(); // ���¡��и�ڵ㡱���档

				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_AUTO_REM_CUTLINE_OK);
				MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			}
		}
		else
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_AUTO_REM_CUTLINE_ERROR);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
		}
	}
	else if (m_pGenerateRemCutLineDlg->GetCmdType() == GenerateRemCutLineDlg_saveMatCmd) // ��������
	{
		// �ù��յķ�ʽ�õ���Ч����
		const IMaterialSize* pMaterialSize = m_pConfigData->GetMatSize();
		RECT_SIDE iRayDir = m_pGenerateRemCutLineDlg->GetRayDir();
		int iEvalFactor = BeamWidth_2_BeamNum(iRayDir, m_pGenerateRemCutLineDlg->GetStepWidth());
		Polygon2DPtr pUsefulRegion = PartPlacementManager::CalcUsefulRegion(m_pPartLayoutData->GetPartPlacements(), pMaterialSize, iRayDir, iEvalFactor);
		if (!pUsefulRegion)
		{
			strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REM_MAT_ERROR);
			MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
			return;
		}

		// ��������
		DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
		LineArc2DListPtr pOuterLoop(new LineArc2DList);
		Line2DListPtr pLine2DList = pUsefulRegion->GetLineList();
		for (unsigned int i = 0; i < pLine2DList->size(); i++)
		{
			pOuterLoop->push_back(pLine2DList->at(i));
		}
		Rect2D matRect = pUsefulRegion->GetBoundaryRect();
		CString strMatName;
		strMatName.Format(_T("%.1fx%.1f"), matRect.GetWidth(), matRect.GetHeight());
		PolygonMaterialPtr pPolyMat(new PolygonMaterial(pMaterialSize->GetParentID(), strMatName, TRUE, matRect.GetWidth(), matRect.GetHeight(), 1, 1));
		pPolyMat->SetOuterLoop(pOuterLoop);
		MaterialLibWriter::SaveMatSize(pMatLibDBCon, pPolyMat, TRUE);

		// ��ʾ
		strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ_REM_MAT_OK);
		strMsg.Replace(_T("WILLBEREPLACED1"), strMatName);
		MessageBox(m_pViewPort->GetWnd(), strMsg, strProductName, MB_OK | MB_ICONWARNING);
	}
	else if (m_pGenerateRemCutLineDlg->GetCmdType() == GenerateRemCutLineDlg_cancelCmd) // �˳�����
	{
        PartPlacementEditAction::SendMessage2EnterPartPlacementOrLoopInstanceSelectAction();
	}
}

int RemnantCutlineAutoAction::BeamWidth_2_BeamNum(RECT_SIDE iRayDir, double dBeamWidth)
{
	int iBeamNum;

	Rect2D sheetRect = m_pConfigData->GetMatSize()->GetOuterRect();
	if (iRayDir == RECT_TOP || iRayDir == RECT_BOTTOM)
	{
		iBeamNum = (int)(sheetRect.GetWidth() / dBeamWidth);
	}
	else if (iRayDir == RECT_LEFT || iRayDir == RECT_RIGHT)
	{
		iBeamNum = (int)(sheetRect.GetHeight() / dBeamWidth);
	}

	if (iBeamNum == 0)
	{
		iBeamNum = 1;
	}

	return iBeamNum;
}

END_CUTLEADER_NAMESPACE()
