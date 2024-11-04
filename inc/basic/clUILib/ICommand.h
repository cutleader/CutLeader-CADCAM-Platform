#pragma once

#include "IData.h"
#include "EditorData.h"

BEGIN_CUTLEADER_NAMESPACE()


// ����ӿڡ�
// ע��
//  1) ��ʵÿ�����������һ���༭����
class ClUILib_Export ICommand : virtual public IData
{
public:
    virtual ~ICommand() {}

public:
	// execute/redo the command.
	virtual void Do() = 0;

	// undo the command.
	virtual void UnDo() = 0;

	// get the command name.
	virtual CString GetCommandName() const = 0;

	// ��������Ӧ�ġ��༭�����ݡ���
	virtual EditorData GetEditorData() const = 0;
};

END_CUTLEADER_NAMESPACE()
