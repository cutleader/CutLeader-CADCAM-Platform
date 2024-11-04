#pragma once

#include "clFrameCommon.h"
#include "ChildFrm.h"

BEGIN_CUTLEADER_NAMESPACE()

class CLFRAME_Export StartPageFrame : public CChildFrame
{
	DECLARE_DYNCREATE(StartPageFrame)

public:
	StartPageFrame(void);
	~StartPageFrame(void);

public:
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);

	DECLARE_MESSAGE_MAP()
};

END_CUTLEADER_NAMESPACE();
