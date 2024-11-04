#pragma once

#include "IInteract.h"

DECLARE_CUTLEADER_CLASS(Point2D)
DECLARE_CUTLEADER_CLASS(IAction)

BEGIN_CUTLEADER_NAMESPACE()


// 所有“动作”的接口。
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
    //  2）有时不要使用这种机制，因为会要求上一个action知道下一个action的细节，可能会让上一个action变得臃肿。
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

	// 更新动作中的一些缓存信息。
	// 注：
	//  1) 为了提高效率，某些动作中缓存了一些数据，这些数据在某些时候需要被更新一下。
	virtual void UpdateActionBuffer() = 0;

	// 这个动作是不是会处理右键消息。处理的话，右键菜单需要让路。
	virtual bool IsProcessRightClickEvent() const = 0;
};

END_CUTLEADER_NAMESPACE()
