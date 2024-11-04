#pragma once

#include "ISubject.h"

DECLARE_CUTLEADER_CLASS(ObserverList)

BEGIN_CUTLEADER_NAMESPACE()

// the basic implementation for interface ISubject.
class ClBaseData_Export SubjectBase : virtual public ISubject
{
public:
	SubjectBase();
	virtual ~SubjectBase();

public: // implement interface ISubject.
	virtual void AddObserver(IObserver* pObserver) override;
	virtual void RemoveObserver(LONGLONG iObserverID) override;
	virtual void NotifyAllObserver(int iEventID, VARIANT varEventContent) override;

private:
	// these observers will watch this subject.
	ObserverListPtr m_pObserverList;
};

END_CUTLEADER_NAMESPACE()
