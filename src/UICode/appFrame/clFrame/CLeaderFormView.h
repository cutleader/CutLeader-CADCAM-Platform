#pragma once

#include "cLeaderDoc.h"
#include "taoresource.h"
#include "ResizableFormView.h"

BEGIN_CUTLEADER_NAMESPACE()

// this basic view class.
// notes:
//   1) this basic view class will be used to display form-based content.
class CLeaderFormView : public CResizableFormView
{
public:
	CLeaderFormView(UINT nIDTemplate);
	virtual ~CLeaderFormView();

public:
	CLeaderDoc* GetDocument() const;
};

END_CUTLEADER_NAMESPACE()
