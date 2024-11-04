#include "stdafx.h"
#include "StartPageDoc.h"

#include "clFrameResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(StartPageDoc, CLeaderDoc)

StartPageDoc::StartPageDoc()
{
}

StartPageDoc::~StartPageDoc()
{
}

BOOL StartPageDoc::OnNewDocument()
{
	// set the title.
	CString strTitle;
	strTitle = MultiLanguageMgr::GetInstance()->TranslateString(IDS_START_PAGE);
	SetTitle(strTitle);

	return CLeaderDoc::OnNewDocument();
}

END_CUTLEADER_NAMESPACE()
