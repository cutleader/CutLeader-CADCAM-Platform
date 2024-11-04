#include "StdAfx.h"
#include "PartPlacementTransformAction.h"

#include "baseConst.h"
#include "ValueInputManager.h"
#include "CommandManager.h"
#include "GlViewPort.h"
#include "clUILibResource.h"

#include "Part.h"
#include "PatternDrawer.h"

#include "PartPlacement.h"
#include "PartPlacementList.h"
#include "PartPlacementDrawer.h"
#include "PartPlacementTransformCommand.h"
#include "PartPlacementSelectAction.h"
#include "clPartLayoutResource.h"

#include "DisplayParam.h"
#include "CLOptionInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

PartPlacementTransformAction::PartPlacementTransformAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList, PartGroupListPtr pPartGroups, DistanceDetectDlg* pDisDetectDlg, GlViewPortPtr pViewPort)
    : PartPlacementPreSelectAction(pMaterialSize, pPartPlacementList, pPartInstanceList, pPartGroups, pDisDetectDlg, pViewPort)
{
	m_actionState = TRANSFORM_PARTPMT_NOTSTART;
	m_pNewPartPlacements.reset(new PartPlacementList());
}

PartPlacementTransformAction::~PartPlacementTransformAction(void)
{
}

BOOL PartPlacementTransformAction::LButtonDown(double& dPtX, double& dPtY)
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::LButtonDown(dPtX, dPtY);
	}
	else
	{
		if (m_actionState == TRANSFORM_PARTPMT_NOTSTART || m_actionState == TRANSFORM_PARTPMT_SECOND_PT_SELECTED)
		{
			m_actionState = TRANSFORM_PARTPMT_BASE_PT_SELECTED;
			m_iMsgID = GetMsgID();

			// set the transform matrix.
			m_dBasePtX = dPtX;
			m_dBasePtY = dPtY;
			m_dFirstPtX = dPtX;
			m_dFirstPtY = dPtY;
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			SetMatrixValue();

			// create a new copy of selected part pmt.
			for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
			{
				PartPlacementPtr pPartPlacement;
				m_pSelectedPartPlacements->at(i)->CopyTo(pPartPlacement);
				m_pNewPartPlacements->push_back(pPartPlacement);
			}
		}
		else if (m_actionState == TRANSFORM_PARTPMT_BASE_PT_SELECTED)
		{
			m_actionState = TRANSFORM_PARTPMT_SECOND_PT_SELECTED;
			m_iMsgID = GetMsgID();

			// set the transform matrix.
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			m_dFirstPtX = m_dBasePtX;
			m_dFirstPtY = m_dBasePtY;
			SetMatrixValue();

			// create command to transform the part pmts.
			ICommandPtr pCommand(new PartPlacementTransformCommand(GetActionName(), GetEditorData(), m_pPartPlacementList, m_pSelectedPartPlacements, m_pNewPartPlacements));
			pCommand->Do();
			CommandManager::GetInstance()->AddCommand(pCommand);

			// enter PartPlacementSelectAction.
            SendMessage2EnterPartPlacementSelectAction(m_pSelectedPartPlacements.get());
		}
	}

	return TRUE;
}

BOOL PartPlacementTransformAction::LButtonUp(double& dPtX, double& dPtY)
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::LButtonUp(dPtX, dPtY);

		if (m_pSelectedPartPlacements->size() > 0)
		{
			m_iMsgID = GetMsgID();
			m_bDisplayValInput = TRUE;
		}
	}

	return TRUE;
}

BOOL PartPlacementTransformAction::MovePoint(double& dPtX, double& dPtY)
{
	if (m_pSelectedPartPlacements->size() == 0)
	{
		PartPlacementPreSelectAction::MovePoint(dPtX, dPtY);
	}
	else
	{
		if (m_actionState == TRANSFORM_PARTPMT_NOTSTART)
		{
			m_iMsgID = GetMsgID();
		}
		else if (m_actionState == TRANSFORM_PARTPMT_BASE_PT_SELECTED)
		{
			// set the transform matrix.
			m_dFirstPtX = m_dSecondPtX;
			m_dFirstPtY = m_dSecondPtY;
			m_dSecondPtX = dPtX;
			m_dSecondPtY = dPtY;
			SetMatrixValue();

			// update the new copy of the selected part pmts.
			for (unsigned int i = 0; i < m_pNewPartPlacements->size(); i++)
				m_pNewPartPlacements->at(i)->Transform(m_modifyMatrix);
		}
	}

	return TRUE;
}

BOOL PartPlacementTransformAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_pSelectedPartPlacements->size() > 0)
	{
		double d1, d2;
		if (ValueInputManager::GetInstance()->IsInputEnd(nChar))
		{
			if (!ValueInputManager::GetInstance()->GetInput(d1, d2))
				m_pViewPort->GetCursorPos(d1, d2);

			ConvertKBInput(d1, d2);
			MovePoint(d1, d2);
			LButtonDown(d1, d2);
		}
		else if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
		{
            SendMessage2EnterPartPlacementSelectAction();
		}

		ValueInputManager::GetInstance()->AcceptInputChar(nChar);
	}

	return __super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void PartPlacementTransformAction::UpdateForUndoRedo()
{
	for (unsigned int i = 0; i < m_pSelectedPartPlacements->size(); i++)
	{
		PartPlacementPtr pPartPlacement = m_pSelectedPartPlacements->at(i);
		if (!m_pPartPlacementList->GetPartPlacementByID(pPartPlacement->GetID()))
		{
			m_pSelectedPartPlacements->clear();
			m_pNewPartPlacements->clear();
			m_pActivePartPlacements->clear();

			m_actionState = TRANSFORM_PARTPMT_NOTSTART;
			m_iMsgID = IDS_OUTPUT_SELECTPRTPMT;

			break;
		}
	}
}

END_CUTLEADER_NAMESPACE()
