#pragma once

#include "clBaseDataCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

class ClBaseData_Export HelpVideo
{
public:
	HelpVideo(void);
	~HelpVideo(void);

public: // get/set functions.
	void SetName(CString strName) { m_strName = strName; }
	CString GetName() { return m_strName; }

	void SetComment(CString strComment) { m_strComment = strComment; }
	CString GetComment() { return m_strComment; }

	void SetFileName(CString strFileName) { m_strFileName = strFileName; }
	CString GetFileName() { return m_strFileName; }

private:
	// the video name.
	CString m_strName;

	// the video comment.
	CString m_strComment;

	// the video file name.
	CString m_strFileName;
};

END_CUTLEADER_NAMESPACE()
