#pragma once

#include "clFrameCommon.h"
#include "cLeaderDoc.h"

DECLARE_CUTLEADER_CLASS(Sheet)

BEGIN_CUTLEADER_NAMESPACE()

// this class stands for a Sheet document.
class CLFRAME_Export SheetDocument : public CLeaderDoc
{
	DECLARE_DYNCREATE(SheetDocument)

public:
	SheetDocument(void);
	~SheetDocument(void);

public: // inherit from CDocument class.
	virtual BOOL DoFileSave() { return OnSaveDocument(); }

public: // inherit from basic class.
	// do some initializations here when create/open a sheet.
	virtual BOOL OnNewDocument();

	// save the part in this function.
	virtual BOOL OnSaveDocument();

	virtual void SetModifyStatus();

public: // get/set functions.
	// set sheet, and update the title.
	void SetSheet(SheetPtr pSheet);

	SheetPtr GetSheet() { return m_pSheet; }

private:
	SheetPtr m_pSheet;
};

END_CUTLEADER_NAMESPACE()
