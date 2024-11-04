#include "StdAfx.h"
#include "MicroJointEditAction2.h"

#include "ValueInputManager.h"
#include "DummyAction.h"
#include "GlViewPort.h"
#include "CommandManager.h"
#include "Point2DList.h"
#include "CursorDrawer.h"
#include "clBaseDataResource.h"

#include "IPattern.h"
#include "IArrayPattern.h"
#include "PatternLoopNode.h"
#include "PatternLoopNodeList.h"
#include "IPatternLoop.h"
#include "PatternLoopList.h"
#include "PatternList.h"

#include "LoopMicroJointList.h"
#include "MicroJointList.h"
#include "MicroJoint.h"
#include "LoopMicroJoint.h"
#include "LoopCutFeatureList.h"
#include "LoopCutFeature.h"
#include "MicroJointPtModifyCommand.h"
#include "MicroJointDeleteCommand.h"
#include "MicroJointPropertyBar.h"
#include "LoopCutFeatureDrawer.h"
#include "clCutFeatureResource.h"
#include "MicroJointMgr.h"

#include "MicroJointsPtModifyCommand.h"
#include "LoopInstanceCache.h"
#include "LoopInstance.h"
#include "PartInstanceList.h"
#include "LoopInstanceList.h"
#include "PartPlacementList.h"
#include "LoopInstanceManager.h"

#include "CLOptionInfo.h"
#include "DisplayParam.h"

BEGIN_CUTLEADER_NAMESPACE()

MicroJointEditAction2::MicroJointEditAction2(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartGroupListPtr pPartGroups,
											 GlViewPortPtr pViewPort, SpreadSamePartDlg* pSpreadSamePartDlg)
    : MicroJointPreSelectAction2(pMaterialSize, pPartPlacements, pPartInstances, pPartGroups, pViewPort)
{
	m_pSpreadSamePartDlg = pSpreadSamePartDlg;
	m_pSpreadSamePartDlg->AddObserver(this);
	m_pAllLoopInstanceList = LoopInstanceManager::GetLoopInstanceList(pPartInstances);
	MicroJointPropertyBar::GetInstance()->IsChangingMJPt(FALSE);
}

MicroJointEditAction2::~MicroJointEditAction2(void)
{
	m_pSpreadSamePartDlg->RemoveObserver(GetID());
}

BOOL MicroJointEditAction2::LButtonDown(double& dPtX, double& dPtY)
{
	BOOL bChangingMJPt = MicroJointPropertyBar::GetInstance()->IsChangingMJPt();
	if (!bChangingMJPt)
	{
		MicroJointPreSelectAction2::LButtonDown(dPtX, dPtY);
	}
	else // �޸�΢���ӵ�λ��
	{
		// if no loop instance selected, return.
		if (!m_pSelLoopInst)
			return FALSE;

		// Ҫ������΢����������
		MicroJointPtr pMJ = m_selMjPtPairs.at(0).first;

		// ������΢���ӵ�����任���������ϵ�¡�
		Point2D newPt_underPartMode = m_newPt;
		Point2D oldPt_underPartMode = m_selMjPtPairs.at(0).second;
		Matrix2D mat = m_pSelLoopInst->GetTransformMatrix();
		mat.Inverse();
		newPt_underPartMode *= mat;
		oldPt_underPartMode *= mat;

		// ����΢���ӵ㡣
		BOOL bDir;
		IPatternLoopPtr pPatternLoop = m_pSelLoopInst->GetCacheData()->GetPatternLoop();
		IPatternPtr pPattern = pPatternLoop->GetPatternByLoopNodeIndex(m_iPatIndex, bDir);
		if (MicroJointMgr::GeomCanHoldMJ(pPattern, newPt_underPartMode, pMJ->GetWidth()))
		{
			// ��΢����λ��
			PatternPosition secondPatPos;
			PatternPosition patternPosition = pPattern->GetPatPosByPoint(newPt_underPartMode, secondPatPos);

			// �޸�΢����λ��
			if (m_pSpreadSamePartDlg->IsSpread())
			{
				MicroJointListPtr pMJList_involveSameParts(new MicroJointList);
				for (unsigned int i = 0; i < m_pAllLoopInstanceList->size(); i++)
				{
					LoopInstancePtr pLoopInstance = m_pAllLoopInstanceList->at(i);
					if (pLoopInstance->GetPatternLoopID() == pPatternLoop->GetID())
					{
						LoopMicroJointPtr pLoopMicroJoint = pLoopInstance->GetCacheData()->GetLoopMicroJoint();
						MicroJointPtr pMicroJoint = pLoopMicroJoint->GetMicroJointAtLocation(oldPt_underPartMode);
						if (pMicroJoint)
						{
							pMJList_involveSameParts->push_back(pMicroJoint);
						}
					}
				}

				ICommandPtr pCommand(new MicroJointsPtModifyCommand(GetEditorData(), pMJList_involveSameParts, pPattern->GetID(), patternPosition));
				pCommand->Do();
				CommandManager::GetInstance()->AddCommand(pCommand);
			}
			else
			{
				ICommandPtr pCommand(new MicroJointPtModifyCommand(GetEditorData(), pMJ, pPattern->GetID(), patternPosition));
				pCommand->Do();
				CommandManager::GetInstance()->AddCommand(pCommand);
			}

			// ���µ�ǰ��΢���ӵ��ѡ�е�΢���ӵ㡣
			m_pActiveMJPt.reset(new Point2D(m_newPt));
			m_selMjPtPairs.at(0).second = m_newPt;

			MicroJointPropertyBar::GetInstance()->IsChangingMJPt(FALSE);
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

BOOL MicroJointEditAction2::LButtonUp(double& dPtX, double& dPtY)
{
	BOOL bChangingMJPt = MicroJointPropertyBar::GetInstance()->IsChangingMJPt();
	if (!bChangingMJPt)
	{
		MicroJointPreSelectAction2::LButtonUp(dPtX, dPtY);

		DisplayMJData();
	}

	return TRUE;
}

BOOL MicroJointEditAction2::MovePoint(double& dPtX, double& dPtY)
{
	BOOL bChangingMJPt = MicroJointPropertyBar::GetInstance()->IsChangingMJPt();
	if (!bChangingMJPt)
	{
		MicroJointPreSelectAction2::MovePoint(dPtX, dPtY);
	}
	else
	{
		// �µ�΢���ӵ㡣
		Point2D pt(dPtX, dPtY);
		IPatternLoopPtr pPatternLoop = m_pSelLoopInst->GetCacheData()->GetPatternLoop();
		Matrix2D mat = m_pSelLoopInst->GetTransformMatrix();
		if (pPatternLoop->GetPatternLoopType() == PatternLoop_Array)
		{
			PatternLoopNodePtr pPatternLoopNode = pPatternLoop->GetPatternLoopNodes()->at(0);
			IArrayPatternPtr pArrayPattern = boost::dynamic_pointer_cast<IArrayPattern>(pPatternLoopNode->GetPattern());
			IPatternPtr pFirstPat = pArrayPattern->GetFirstInstance();
			m_newPt = pFirstPat->GetClosestPt(pt, mat);
			m_iPatIndex = 0;
		}
		else
			pPatternLoop->GetClosestPt(pt, m_newPt, m_iPatIndex, mat);
	}

	return TRUE;
}

BOOL MicroJointEditAction2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bChangingMJPt = MicroJointPropertyBar::GetInstance()->IsChangingMJPt();
	if (bChangingMJPt)
	{
		if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
		{
			MicroJointPropertyBar::GetInstance()->IsChangingMJPt(FALSE);
			MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);
		}
	}
	else
	{
		MicroJointPreSelectAction2::OnKeyDown(nChar, nRepCnt, nFlags);

		// ѡ��ȫ����΢������������
		if (ValueInputManager::GetInstance()->IsInputCtrl() && ValueInputManager::GetInstance()->IsInputA(nChar))
		{
			// ��ʾѡ�еġ�΢�������������ԡ�
			if (m_selMjPtPairs.size() > 0)
			{
				LoopMicroJointListPtr pLoopMicroJointList = m_pPartPlacementList->GetAllLoopMicroJointList();
				MicroJointListPtr pMicroJointList(new MicroJointList);
				Point2DListPtr pMJPtList(new Point2DList);
				for (unsigned int i = 0; i < m_selMjPtPairs.size(); i++)
				{
					pMicroJointList->push_back(m_selMjPtPairs.at(i).first);
					pMJPtList->push_back(m_selMjPtPairs.at(i).second);
				}
				MicroJointPropertyBar::GetInstance()->DisplayMJData(GetEditorData(), pLoopMicroJointList, pMicroJointList, pMJPtList);
			}
		}
		else if (ValueInputManager::GetInstance()->IsInputDel(nChar))
		{
			// ɾ��ѡ�еġ�΢������������
			if (m_selMjPtPairs.size() > 0)
			{
				LoopMicroJointListPtr pLoopMicroJointList = m_pPartPlacementList->GetAllLoopMicroJointList();
				MicroJointListPtr pDelMJList(new MicroJointList);
				for (unsigned int i = 0; i < m_selMjPtPairs.size(); i++)
					pDelMJList->push_back(m_selMjPtPairs.at(i).first);
				ICommandPtr pCommand(new MicroJointDeleteCommand(GetEditorData(), pLoopMicroJointList, pDelMJList));
				pCommand->Do();
				CommandManager::GetInstance()->AddCommand(pCommand);

				// ���ѡ�еġ�΢������������
				m_selMjPtPairs.clear();

				// ����΢�������Խ��档
				MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);
			}
		}
	}

	return TRUE;
}

BOOL MicroJointEditAction2::DrawBeforeFinish()
{
	BOOL bChangingMJPt = MicroJointPropertyBar::GetInstance()->IsChangingMJPt();
	if (!bChangingMJPt)
	{
		MicroJointPreSelectAction2::DrawBeforeFinish();
	}
	else
	{
		// ���ƻ���
		LoopCutFeatureDrawer loopFeatureDrawer(m_pViewPort);
		loopFeatureDrawer.DrawPosRing(m_newPt);

		// draw cursor.
		CursorDrawer::DrawCrossLineCursor(m_pViewPort);
	}

	return TRUE;
}

void MicroJointEditAction2::UpdateForUndoRedo()
{
	// ����ʾ����ǰ�ġ��͡�ѡ�еġ�΢���ӵ㡣
	m_selMjPtPairs.clear();

	// ����΢�������Խ��档
	MicroJointPropertyBar::GetInstance()->ShowBar(FALSE);
}

void MicroJointEditAction2::Update(int iEventID, VARIANT varEventContent)
{
	// SpreadSamePartDlg�б仯��������Ҫ������ʾһ��
	DisplayMJData();
}

void MicroJointEditAction2::DisplayMJData()
{
	if (m_selMjPtPairs.size() > 0)
	{
		// ����ѡ�еĻ�·ʵ����
		if (m_selMjPtPairs.size() == 1)
			m_pSelLoopInst = m_pAllLoopInstanceList->GetInstanceByMicroJointPt(m_selMjPtPairs.at(0).second);

		// ��ʾ΢���ӱ༭���档
		LoopMicroJointListPtr pLoopMicroJointList = m_pPartPlacementList->GetAllLoopMicroJointList();
		MicroJointListPtr pMicroJointList(new MicroJointList);
		Point2DListPtr pMJPtList(new Point2DList);
		for (unsigned int i = 0; i < m_selMjPtPairs.size(); i++)
		{
			pMicroJointList->push_back(m_selMjPtPairs.at(i).first);
			pMJPtList->push_back(m_selMjPtPairs.at(i).second);
		}
		if (m_pSpreadSamePartDlg->IsSpread())
		{
			MicroJointListPtr pMJList_involveSameParts(new MicroJointList);
			for (unsigned int i = 0; i < m_selMjPtPairs.size(); i++)
			{
				MicroJointPtr pMJ = m_selMjPtPairs.at(i).first;
				Point2D microjointPt = m_selMjPtPairs.at(i).second;
				LoopInstancePtr pLoopInstance = m_pAllLoopInstanceList->GetInstanceByMicroJointPt(microjointPt); // ΢������������

				// �����΢���ӵ�ת��partģʽ 
				Point2D mjPt_underPartMode = microjointPt;
				Matrix2D mat = pLoopInstance->GetTransformMatrix();
				mat.Inverse();
				mjPt_underPartMode *= mat;

				for (unsigned int j = 0; j < m_pAllLoopInstanceList->size(); j++)
				{
					LoopInstancePtr pLoopInstance_tmp = m_pAllLoopInstanceList->at(j);
					if (pLoopInstance_tmp->GetPatternLoopID() == pLoopInstance->GetPatternLoopID())
					{
						LoopMicroJointPtr pLoopMicroJoint = pLoopInstance_tmp->GetCacheData()->GetLoopMicroJoint();
						MicroJointPtr pMicroJoint = pLoopMicroJoint->GetMicroJointAtLocation(mjPt_underPartMode);
						if (pMicroJoint)
						{
							pMJList_involveSameParts->push_back(pMicroJoint);
						}
					}
				}
			}

			MicroJointPropertyBar::GetInstance()->DisplayMJData(GetEditorData(), pLoopMicroJointList, pMJList_involveSameParts, pMJPtList);
		}
		else
		{
			MicroJointPropertyBar::GetInstance()->DisplayMJData(GetEditorData(), pLoopMicroJointList, pMicroJointList, pMJPtList);
		}
	}
}

END_CUTLEADER_NAMESPACE()
