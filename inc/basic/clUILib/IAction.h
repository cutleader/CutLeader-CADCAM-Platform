#pragma once

#include "IInteract.h"

DECLARE_CUTLEADER_CLASS(Point2D)
DECLARE_CUTLEADER_CLASS(IAction)

BEGIN_CUTLEADER_NAMESPACE()


// ���С��������Ľӿڡ�
class ClUILib_Export IAction : virtual public IInteract
{
public:
    virtual ~IAction() {}

public:
	/************************************************************************/
	// get some info from the action.

	// get the action type.
	virtual int GetActionType() const = 0;

	// get the first mouse input for this action.
	// notes: 
	//   1) we should perform some mouse inputs for an action. at least one mouse input, e.g. delete a pattern, some actions need two mouse input, e.g. draw a line.
	//   2) return FALSE if no input exist.
	virtual Point2DPtr GetFirstInput() const = 0;

	// get the interact info for the action.
	/* params:
	*   1) msgID: the prompt message that will be displayed to user.
	*   2) bShowValInput: whether let user input value.
	*/
	virtual void GetPromptMsg(int& msgID, BOOL& bShowValInput) const = 0;

	// sometimes we should switch into other actions from current action.
	// notes:
	//  1) e.g., in PatternSelectAction action we can enter "grip edit" action.
    //  2����ʱ��Ҫʹ�����ֻ��ƣ���Ϊ��Ҫ����һ��action֪����һ��action��ϸ�ڣ����ܻ�����һ��action���ӷ�ס�
	virtual IActionPtr GetNextAction() const = 0;
	/************************************************************************/


	// draw something during the action.
	virtual BOOL DrawBeforeFinish() = 0;

	// convert the keyboard input to the values that can be transferred into "LButtonDown" and "MovePoint" function.
	// notes:
	//   1) e.g., under CirclePatternEditor action, if you input the radius from the keyboard, this function will transform the radius to the
	//      proper point on the circle, then can transfer this point into "LButtonDown" and "MovePoint" function.
	virtual void ConvertKBInput(double& dVal1, double& dVal2) = 0;

	// update the action after the undo/redo operation.
	virtual void UpdateForUndoRedo() = 0;

	// ���¶����е�һЩ������Ϣ��
	// ע��
	//  1) Ϊ�����Ч�ʣ�ĳЩ�����л�����һЩ���ݣ���Щ������ĳЩʱ����Ҫ������һ�¡�
	virtual void UpdateActionBuffer() = 0;

	// ��������ǲ��ǻᴦ���Ҽ���Ϣ������Ļ����Ҽ��˵���Ҫ��·��
	virtual bool IsProcessRightClickEvent() const = 0;
};

END_CUTLEADER_NAMESPACE()
