#include "stdafx.h"
#include "clKnowledgeBaseConfig.h"

#include "KBConfigView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_CUTLEADER_NAMESPACE()

KBConfigView::KBConfigView(UINT nIDTemplate)
	: CResizableFormView(nIDTemplate)
{

}

KBConfigView::~KBConfigView()
{
}

KBConfigDoc* KBConfigView::GetDocument() const
{ 
	return reinterpret_cast<KBConfigDoc*>(m_pDocument); 
}

END_CUTLEADER_NAMESPACE()
