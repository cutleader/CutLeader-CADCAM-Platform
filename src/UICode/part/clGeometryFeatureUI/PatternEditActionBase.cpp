#include "StdAfx.h"
#include "PatternEditActionBase.h"

#include "GlViewPort.h"
#include "DummyAction.h"
#include "clUILibResource.h"
#include "ValueInputManager.h"

#include "clGeometryFeatureResource.h"
#include "PatternList.h"
#include "PatternPropertyBar.h"
#include "PatternSelectAction.h"

BEGIN_CUTLEADER_NAMESPACE()

PatternEditActionBase::PatternEditActionBase(void)
{
	m_pPatList.reset();
	m_pPatternLoopList.reset();
	m_bSupportDrawAssit = TRUE;
}

PatternEditActionBase::~PatternEditActionBase(void)
{
}

EditorData PatternEditActionBase::GetEditorData() const
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEOMETRY);

	EditorData editorData(CWnd::FromHandle(m_pViewPort->GetWnd()), str);
	return editorData;
}

BOOL PatternEditActionBase::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ActionBase::OnKeyDown(nChar, nRepCnt, nFlags);

	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
		m_pNextAction.reset(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));

	return TRUE;
}

void PatternEditActionBase::EnterPatSelAction(IPatternPtr pSelPat)
{
	// enter PatternSelectAction.
	PatternSelectActionPtr pPatSelectAction(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	m_pNextAction = pPatSelectAction;

	// select pattern.
	PatternListPtr pPatList(new PatternList);
	pPatList->push_back(pSelPat);
	pPatSelectAction->SelectPats(pPatList, TRUE);
}

void PatternEditActionBase::EnterPatSelAction(PatternListPtr pSelPatList)
{
	// enter PatternSelectAction.
	PatternSelectActionPtr pPatSelectAction(new PatternSelectAction(m_pPatList, m_pPatternLoopList, m_pViewPort));
	m_pNextAction = pPatSelectAction;

	// select pattern.
	pPatSelectAction->SelectPats(pSelPatList, TRUE);
}

END_CUTLEADER_NAMESPACE()
