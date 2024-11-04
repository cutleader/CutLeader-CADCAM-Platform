#include "stdafx.h"
#include "clNestJobUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CLNestJobUIApp, CWinApp)
END_MESSAGE_MAP()

CLNestJobUIApp::CLNestJobUIApp()
{
}

CLNestJobUIApp theApp;

BOOL CLNestJobUIApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
