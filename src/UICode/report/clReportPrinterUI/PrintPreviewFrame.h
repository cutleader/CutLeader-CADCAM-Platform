#pragma once

#include "clReportPrinterCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// ¥Ú”°‘§¿¿µƒœ‘ æøÚº‹°£
class PrintPreviewFrame : public CFrameWnd
{
	DECLARE_DYNCREATE (PrintPreviewFrame);

public:
	PrintPreviewFrame(void);

protected:
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
};

END_CUTLEADER_NAMESPACE()
