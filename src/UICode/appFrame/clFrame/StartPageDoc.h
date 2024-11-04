#pragma once

#include "clFrameCommon.h"
#include "CLeaderDoc.h"

BEGIN_CUTLEADER_NAMESPACE()

class CLFRAME_Export StartPageDoc : public CLeaderDoc
{
	DECLARE_DYNCREATE(StartPageDoc)

protected: // create from serialization only
	StartPageDoc();

	virtual ~StartPageDoc();

public: // inherit from CLeaderDoc class.
	virtual BOOL OnNewDocument();
};

END_CUTLEADER_NAMESPACE()
