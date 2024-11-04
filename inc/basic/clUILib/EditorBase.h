#pragma once

#include "IEditor.h"
#include "InteractBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// ±à¼­Æ÷»ùÀà¡£
class ClUILib_Export EditorBase : virtual public IEditor,
								public InteractBase
{
public:
	EditorBase(void);
	~EditorBase(void);

public: // implement IEditor interface.
	IActionPtr GetActionObject() { return m_pAction; }
	virtual void SetDummyAction();
	virtual GlViewPortPtr GetDrawer() { return m_pViewPort; }
	virtual void UpdateOutputBar();
	virtual void UpdateActionForUndoRedo();
	virtual CString GetEditorName() { return _T(""); }

protected:
	// the current action object.
	IActionPtr m_pAction;

	// the drawer.
	GlViewPortPtr m_pViewPort;
};

END_CUTLEADER_NAMESPACE()
