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
		// ��ʾ���ù�����塣
		CommonFuncPane::GetInstance()->ShowBar(TRUE);
		CommonFuncPane::GetInstance()->DisplaySheetPane();
		SheetView* pSheetView = dynamic_cast<SheetView*>(GetActiveView());
		CommonFuncPane::GetInstance()->GetSheetPane()->Init(pSheetView);

		// ���������Ҳ����塣
		UIManager::GetInstance()->GetBarManager()->HideAllRightBar();

		// ��ʾ��������
		InteractBar::GetInstance()->ShowBar(TRUE);

		// ����״̬����
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
