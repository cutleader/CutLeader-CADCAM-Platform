#include "StdAfx.h"
#include "SheetFrame.h"

#include "baseConst.h"
#include "InteractBar.h"

#include "PartPlacementAddPanel.h"
#include "PartPlacementPropertyBar.h"
#include "PartPlacementViewBar.h"
#include "CutSequencePane.h"
#include "CutNodeViewBar.h"
#include "NCViewBar.h"

#include "Sheet.h"
#include "SheetProcessor.h"

#include "CommonFuncPane.h"
#include "DataItemTracker.h"
#include "UIManager.h"
#include "SheetView.h"
#include "SheetDocument.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(SheetFrame, CChildFrame)

BEGIN_MESSAGE_MAP(SheetFrame, CChildFrame)
	ON_WM_MDIACTIVATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

SheetFrame::SheetFrame(void)
{
}

SheetFrame::~SheetFrame(void)
{
}

void SheetFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CChildFrame::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	if (bActivate)
	{
		// 显示常用功能面板。
		CommonFuncPane::GetInstance()->ShowBar(TRUE);
		CommonFuncPane::GetInstance()->DisplaySheetPane();
		SheetView* pSheetView = dynamic_cast<SheetView*>(GetActiveView());
		CommonFuncPane::GetInstance()->GetSheetPane()->Init(pSheetView);

		// 隐藏所有右侧的面板。
		UIManager::GetInstance()->GetBarManager()->HideAllRightBar();

		// 显示交互栏。
		InteractBar::GetInstance()->ShowBar(TRUE);

		// 更新状态栏。
		CString str1, str2;
		str1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_WORK_MODE);
		str2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_SHEET_MODE);
		str1.Replace(_T("WILLBEREPLACED1"), str2);
		UIManager::GetInstance()->GetStatusBar()->Show();
		UIManager::GetInstance()->GetStatusBar()->UpdateStatusBar(str1, ID_FIRST_PANE);
	}
}

void SheetFrame::OnClose()
{
	// check whether the current document has been modified.
	CDocument* pTmpDoc = GetActiveDocument();
	SheetDocument* pSheetDoc = dynamic_cast<SheetDocument*>(pTmpDoc);
	pSheetDoc->SetModifyStatus();

	CChildFrame::OnClose();
}

void SheetFrame::OnDestroy()
{
	/************************************************************************/
	// remove the sheet from DataItemTracker. 

	CLeaderView *pView = (CLeaderView*)GetActiveView();
	SheetDocument* pSheetDocument = (SheetDocument*)pView->GetDocument();
	SheetPtr pSheet = pSheetDocument->GetSheet();
	LONGLONG iSheetID = pSheet->GetID();
	DataItemTracker::GetInstance()->RemoveDataItem(iSheetID);
	/************************************************************************/

	// tell CMainFrame that i has been destroyed.
	HWND hwnd = GetParent()->GetParent()->GetSafeHwnd();
	::SendMessage(hwnd, WM_CHILDFRM_CLOSE, 0, 0);

	CChildFrame::OnDestroy();
}

END_CUTLEADER_NAMESPACE()
