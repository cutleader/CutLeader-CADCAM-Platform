#pragma once

#include "IInteract.h"

DECLARE_CUTLEADER_CLASS(GlViewPort)

BEGIN_CUTLEADER_NAMESPACE()

class IAction;
typedef boost::shared_ptr<IAction> IActionPtr;

// ±à¼­Æ÷½Ó¿Ú¡£
class ClUILib_Export IEditor : virtual public IInteract
{
public:
	// get the current action of the editor.
	// notes:
	//   1) return NULL, means no current action is available for the editor.
	virtual IActionPtr GetActionObject() = 0;

	// set the current action to dummy action.
	virtual void SetDummyAction() = 0;

	// get the drawer which associate with the editor.
	virtual GlViewPortPtr GetDrawer() = 0;

	// update the InteractBar based on the current action.
	virtual void UpdateOutputBar() = 0;

	// update the action after the undo/redo operation.
	virtual void UpdateActionForUndoRedo() = 0;

	// ±à¼­Æ÷µÄÃû×Ö
	virtual CString GetEditorName() = 0;
};

END_CUTLEADER_NAMESPACE()
