#include "StdAfx.h"
#include "PartFrame.h"

#include "baseConst.h"
#include "InteractBar.h"

#include "PartCadData.h"
#include "PartItem.h"

#include "DataItemTracker.h"
#include "UIManager.h"
#include "PartView.h"
#include "PartDocument.h"
#include "CommonFuncPane.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(PartFrame, CChildFrame)

BEGIN_MESSAGE_MAP(PartFrame, CChildFrame)
	ON_WM_MDIACTIVATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()

END_MESSAGE_MAP()

PartFrame::PartFrame(void)
{
}

PartFrame::~PartFrame(void)
{
}

void PartFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CChildFrame::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if (bActivate)
	{
		// 显示常用功能面板。
		CommonFuncPane::GetInstance()->ShowBar(TRUE);
		CommonFuncPane::GetInstance()->DisplayPartPane();
		PartView* pPartView = dynamic_cast<PartView*>(GetActiveView());
		CommonFuncPane::GetInstance()->GetPartPane()->Init(pPartView);

		// 隐藏所有右侧的面板。
		UIManager::GetInstance()->GetBarManager()->HideAllRightBar();

		// 显示交互栏。
		InteractBar::GetInstance()->ShowBar(TRUE);

		// 更新状态栏。
		CString str1, str2;
		str1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_WORK_MODE);
		str2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PART_MODE);
		str1.Replace(_T("WILLBEREPLACED1"), str2);
		UIManager::GetInstance()->GetStatusBar()->Show();
		UIManager::GetInstance()->GetStatusBar()->UpdateStatusBar(str1, ID_FIRST_PANE);
	}
}

void PartFrame::OnClose()
{
	// check whether the current document has been modified.
	CDocument* pTmpDoc = GetActiveDocument();
	PartDocument* pPartDoc = dynamic_cast<PartDocument*>(pTmpDoc);
	pPartDoc->SetModifyStatus();

	CChildFrame::OnClose();
}

void PartFrame::OnDestroy()
{
	/************************************************************************/
	// remove the part from DataItemTracker. 

	CLeaderView *pView = (CLeaderView*)GetActiveView();
	PartDocument* pPartDocument = (PartDocument*)pView->GetDocument();
	PartItemPtr pPartItem = pPartDocument->GetPartItem();
	LONGLONG iPartID = pPartItem->GetPartID();
	DataItemTracker::GetInstance()->RemoveDataItem(iPartID);
	/************************************************************************/

	// tell CMainFrame that i has been destroyed.
	HWND hwnd = GetParent()->GetParent()->GetSafeHwnd();
	::SendMessage(hwnd, WM_CHILDFRM_CLOSE, 0, 0);

	CChildFrame::OnDestroy();
}

END_CUTLEADER_NAMESPACE()
