#include "StdAfx.h"
#include "PartPlacementEditAction.h"

#include "baseConst.h"
#include "GlViewPort.h"
#include "ValueInputManager.h"
#include "LogMgr.h"

#include "clPartLayoutResource.h"
#include "PartPlacement.h"
#include "PartPlacementList.h"


using namespace std;


BEGIN_CUTLEADER_NAMESPACE()

PartPlacementEditAction::PartPlacementEditAction(IMaterialSizePtr pMaterialSize, PartPlacementListPtr pPartPlacementList, PartInstanceListPtr pPartInstanceList,
                                                 PartGroupListPtr pPartGroups, GlViewPortPtr pViewPort)
{
    m_pMatSize = pMaterialSize;
    m_pPartPlacementList = pPartPlacementList;
    m_pPartInstanceList = pPartInstanceList;
    m_pPartGroupList = pPartGroups;
    m_pViewPort = pViewPort;
}

PartPlacementEditAction::~PartPlacementEditAction(void)
{
}

EditorData PartPlacementEditAction::GetEditorData() const
{
	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_LAYOUT);

	EditorData editorData(CWnd::FromHandle(m_pViewPort->GetWnd()), str);
	return editorData;
}

BOOL PartPlacementEditAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		m_bDisplayValInput = FALSE;
        SendMessage2EnterPartPlacementOrLoopInstanceSelectAction();
	}

	return ActionBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

void PartPlacementEditAction::SendMessage2EnterPartPlacementSelectAction()
{
    // ת��ѡ��������ö���
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
    CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
    CView* pView = pChildFrame->GetActiveView();
    if (!pView)
    {
        LogMgr::DebugWarn(_T("021796"));
        return;
    }
    pView->PostMessage(WM_EnterSelectPartPlacementOrLoopInstanceAction, 2, 0);
}

void PartPlacementEditAction::SendMessage2EnterLoopInstanceSelectAction()
{
    // ת��ѡ������ʵ������
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
    CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
    CView* pView = pChildFrame->GetActiveView();
    if (!pView)
    {
        LogMgr::DebugWarn(_T("784631"));
        return;
    }
    pView->PostMessage(WM_EnterSelectPartPlacementOrLoopInstanceAction, 1, 0);
}

void PartPlacementEditAction::SendMessage2EnterPartPlacementOrLoopInstanceSelectAction()
{
    // ת��ѡ��������û�����ʵ������
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
    CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
    CView* pView = pChildFrame->GetActiveView();
    if (!pView)
    {
        LogMgr::DebugWarn(_T("546231"));
        return;
    }
    pView->PostMessage(WM_EnterSelectPartPlacementOrLoopInstanceAction, 3, 0);
}

void PartPlacementEditAction::SendMessage2EnterPartPlacementSelectAction(const PartPlacementList* pPartPlacements_preSelect)
{
    // ҪԤѡ�е�������õ�ID��
    if (!pPartPlacements_preSelect || pPartPlacements_preSelect->size() == 0)
    {
        LogMgr::DebugWarn(_T("356846"));
        return;
    }
    vector<LONGLONG>* pPartPlacementIDs_preSelect = new vector<LONGLONG>();
    for (unsigned int i = 0; i < pPartPlacements_preSelect->size(); i++)
    {
        const PartPlacement* pPartPlacement = pPartPlacements_preSelect->operator [](i).get();
        pPartPlacementIDs_preSelect->push_back(pPartPlacement->GetID());
    }

    // ת��ѡ��������ö���
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
    CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
    CView* pView = pChildFrame->GetActiveView();
    if (!pView)
    {
        LogMgr::DebugWarn(_T("256842"));
        return;
    }
    pView->PostMessage(WM_EnterSelectPartPlacementAction, (WPARAM)pPartPlacementIDs_preSelect, 0);
}

void PartPlacementEditAction::SendMessage2EnterPartPlacementSelectAction(const PartPlacement* pPartPlacement_preSelect)
{
    // ҪԤѡ�е�������õ�ID��
    if (!pPartPlacement_preSelect)
    {
        LogMgr::DebugWarn(_T("658742"));
        return;
    }
    vector<LONGLONG>* pPartPlacementIDs_preSelect = new vector<LONGLONG>();
    pPartPlacementIDs_preSelect->push_back(pPartPlacement_preSelect->GetID());

    // ת��ѡ��������ö���
    CMDIFrameWnd* pMainFrame = (CMDIFrameWnd*)AfxGetApp()->GetMainWnd();
    CMDIChildWnd* pChildFrame = (CMDIChildWnd*)pMainFrame->GetActiveFrame();
    CView* pView = pChildFrame->GetActiveView();
    if (!pView)
    {
        LogMgr::DebugWarn(_T("540398"));
        return;
    }
    pView->PostMessage(WM_EnterSelectPartPlacementAction, (WPARAM)pPartPlacementIDs_preSelect, 0);
}

END_CUTLEADER_NAMESPACE()
