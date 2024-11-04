#include "stdafx.h"
#include "cLeaderDoc.h"
#include "cLeaderFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_CUTLEADER_NAMESPACE()

CLeaderFormView::CLeaderFormView(UINT nIDTemplate)
	: CResizableFormView(nIDTemplate)
{

}

CLeaderFormView::~CLeaderFormView()
{
}

CLeaderDoc* CLeaderFormView::GetDocument() const
{ 
	return reinterpret_cast<CLeaderDoc*>(m_pDocument); 
}

END_CUTLEADER_NAMESPACE()
