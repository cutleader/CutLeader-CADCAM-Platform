#pragma once

#include "clFrameCommon.h"

DECLARE_CUTLEADER_CLASS(DataItem)
DECLARE_CUTLEADER_CLASS(PartDocument)
DECLARE_CUTLEADER_CLASS(SheetDocument)
DECLARE_CUTLEADER_CLASS(CLeaderDoc)
DECLARE_CUTLEADER_CLASS(CLeaderView)

BEGIN_CUTLEADER_NAMESPACE()

// this class care something about the documents.
class CLFRAME_Export DocManager
{
public:
	/************************************************************************/
	// about new document.

	// new a part document.
	static void NewPartDoc(CMultiDocTemplate* pPartDocTemplate);

	// new a sheet document.
	/* 参数：
	*  bGuideMode: 是否在引导模式。
	*/
	static void NewShtDoc(CMultiDocTemplate* pShtDocTemplate, bool bGuideMode = false);

	// 新建排版。
	static void NewNestJob(CMultiDocTemplate* pShtDocTemplate, bool bGuideMode = false);
	/************************************************************************/

	/************************************************************************/
	// about open document.

	// open the part.
	static void OpenPartDoc(DataItemPtr pDataItem, CMultiDocTemplate* pPartDocTemplate);

	// open the sheet.
	static void OpenShtDoc(DataItemPtr pDataItem, CMultiDocTemplate* pShtDocTemplate);
	/************************************************************************/

	/************************************************************************/
	// about save as document.

	// save as the part in a location.
	static void SaveAsPart(PartDocument* pPartDoc);

	// save as the sheet in a location.
	static void SaveAsSht(SheetDocument* pShtDoc);
	/************************************************************************/

	/************************************************************************/
	// about active document.

	// if "pDataItem" is opened, active it and return TRUE.
	static BOOL ActivePartDoc(DataItemPtr pDataItem, CMultiDocTemplate* pPartDocTemplate);

	// if "pDataItem" is opened, active it and return TRUE.
	static BOOL ActiveShtDoc(DataItemPtr pDataItem, CMultiDocTemplate* pShtDocTemplate);
	/************************************************************************/

	// get active document.
	static CLeaderDoc* GetActiveDocument();

	// get active view
	static CLeaderView* GetActiveView();

	// 关闭“开始页面”。
	static void CloseStartPage();
};

END_CUTLEADER_NAMESPACE()
