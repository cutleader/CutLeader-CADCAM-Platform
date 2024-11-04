#pragma once

#include "clUILibCommon.h"
#include "ICommand.h"
#include "ClData.h"

BEGIN_CUTLEADER_NAMESPACE()

// the basic implementation for ICommand interface.
class ClUILib_Export CommandBase : virtual public ICommand, public ClData
{
public:
	CommandBase(void);
	virtual ~CommandBase(void);

public: // implement interface ICommand.
	virtual CString GetCommandName() const { return m_strCommandName; }
	virtual EditorData GetEditorData() const { return m_categoryData; }

protected:
	// the name of the command.
	CString m_strCommandName;

	// the view's category info which the command is associated with.
	EditorData m_categoryData;
};

END_CUTLEADER_NAMESPACE()
