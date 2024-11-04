#include "StdAfx.h"
#include "SequenceEditActionBase.h"

#include "GlViewPort.h"
#include "ValueInputManager.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"

#include "CutNodeManager.h"
#include "clCutSequenceResource.h"
#include "ConfigData.h"
#include "PartPlacementEditAction.h"


BEGIN_CUTLEADER_NAMESPACE()

SequenceEditActionBase::SequenceEditActionBase(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
{
	m_pPartLayoutData = pPartLayoutData;
	m_pCutSequences = pCutSequences;
	m_pConfigData = pConfigData;
	m_pViewPort = pViewPort;

	m_bDisplayValInput = FALSE;

	// 更新切割节点缓存。
	RefreshCutNodes();
}

SequenceEditActionBase::~SequenceEditActionBase(void)
{
}

EditorData SequenceEditActionBase::GetEditorData() const
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SEQ);
	EditorData editorData(CWnd::FromHandle(m_pViewPort->GetWnd()), str);
	return editorData;
}

BOOL SequenceEditActionBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
    {
        PartPlacementEditAction::SendMessage2EnterPartPlacementOrLoopInstanceSelectAction();
    }

	return ActionBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

void SequenceEditActionBase::RefreshCutNodes()
{
	const IMaterialSize* pMaterialSize = m_pConfigData->GetMatSize();
	const SequenceConfigItem* pSeqConfigItem = m_pConfigData->GetSeqConfigItem();
	Point2D startSeqPt = pMaterialSize->GetOuterRect().GetCornerPt(pSeqConfigItem->GetStartCorner());
	m_pCutNodeList = CutNodeManager::CutSequences2CutNodes(startSeqPt, m_pCutSequences.get());
}

END_CUTLEADER_NAMESPACE()
