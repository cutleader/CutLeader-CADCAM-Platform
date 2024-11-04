#pragma once

#include "EditorData.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(CommandList)
DECLARE_CUTLEADER_CLASS(ICommand)

BEGIN_CUTLEADER_NAMESPACE()


// the command list.
class ClUILib_Export CommandList : public std::vector<ICommandPtr>
{
public:
	CommandList(void);
	~CommandList(void);

public:
	// ���ݱ༭���õ�����ϡ�
	CommandListPtr GetCmdByEditor(EditorData editorData);

	// ������ͼ�õ�����ϡ�
	CommandListPtr GetCommandByView(CWnd* pView);

	// remove the commands from the list.
	void RemoveCommand(CommandListPtr pCommandList);

	// remove the command from the list.
	// notes:
	//   1) before call this, caller should make sure the list has "iCommandID".
	void RemoveCommand(LONGLONG iCommandID);
};

END_CUTLEADER_NAMESPACE()
