#include "StdAfx.h"
#include "StartPageFrame.h"

#include "InteractBar.h"
#include "UIManager.h"
#include "CommonFuncPane.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(StartPageFrame, CChildFrame)

BEGIN_MESSAGE_MAP(StartPageFrame, CChildFrame)
	ON_WM_MDIACTIVATE()
END_MESSAGE_MAP()

StartPageFrame::StartPageFrame(void)
{
}

StartPageFrame::~StartPageFrame(void)
{
}

void StartPageFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CChildFrame::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
}

END_CUTLEADER_NAMESPACE()
