#pragma once

#include "IObserver.h"
#include "ClData.h"

BEGIN_CUTLEADER_NAMESPACE()

// the basic implementation for IObserver.
class ClBaseData_Export ObserverBase : virtual public IObserver, public ClData
{
public:
	ObserverBase();
	virtual ~ObserverBase();
};

END_CUTLEADER_NAMESPACE()
