#pragma once

#include "IAction.h"
#include "InteractBase.h"

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(Point2DList)

BEGIN_CUTLEADER_NAMESPACE()

// 所有动作的基类。
class ClUILib_Export ActionBase : virtual public IAction, public InteractBase
{
public:
	ActionBase(void);
	virtual ~ActionBase(void);

public: // implement IAction interface.
	virtual IActionPtr GetNextAction() const override { return m_pNextAction; }
	virtual Point2DPtr GetFirstInput() const override;
	virtual void GetPromptMsg(int& msgID, BOOL& bShowValInput) const override;
	virtual void ConvertKBInput(double& dVal1, double& dVal2) {}
	virtual void UpdateForUndoRedo() {}
	virtual void UpdateActionBuffer() {}
	virtual bool IsProcessRightClickEvent() const override { return false; }

protected:
	// all mouse input during this action.
	Point2DListPtr m_pInputPtList;

	// the drawer.
	GlViewPortPtr m_pViewPort;

	// the action which we will switch into.
	IActionPtr m_pNextAction;

	/************************************************************************/
	// used for InteractBar.

	// the message that will be displayed in InteractBar Bar.
	int m_iMsgID;

	// whether need to enable the "ValueInputEditBox" in InteractBar Bar.
	BOOL m_bDisplayValInput;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
