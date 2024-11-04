#pragma once

#include "clDataImportExportCommon.h"
#include "clDataImportExportUICommon.h"


BEGIN_CUTLEADER_NAMESPACE()


// use this dialog to export files out of the CAM.
class ClDataImportExportUI_Export ExpDataDialog : public CFileDialog
{
	DECLARE_DYNAMIC(ExpDataDialog)

public:
	ExpDataDialog(LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
	virtual ~ExpDataDialog();

protected:
	virtual BOOL OnInitDialog();

protected:
	virtual BOOL OnFileNameOK();

	DECLARE_MESSAGE_MAP()

public:
	// get the data type which we will export as.
	IMPEXP_DATA_TYPE GetExpType();
};

END_CUTLEADER_NAMESPACE()
