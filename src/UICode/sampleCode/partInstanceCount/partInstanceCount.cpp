#include "stdafx.h"
#include "partInstanceCount.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(PartInstanceCountApp, CWinApp)
END_MESSAGE_MAP()

PartInstanceCountApp::PartInstanceCountApp()
{
}

PartInstanceCountApp theApp;

BOOL PartInstanceCountApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
