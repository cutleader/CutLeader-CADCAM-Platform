#pragma once

#include "clUILibCommon.h"
#include "ValueInputEditBox.h"
#include "XBarBase.h"

BEGIN_CUTLEADER_NAMESPACE()

// 交互bar。
// 注：
//  1) 这个bar可能让用户输入数据。
//  2) 这个bar会显示提示信息给用户。
class ClUILib_Export InteractBar : public XBarBase
{
private:
	InteractBar(void);

public:
	~InteractBar(void);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()

public:
	static InteractBar* GetInstance(CWnd* pParent = NULL);

	// display message in InteractBar.
	void DisplayMessage(int iMsg, BOOL bShowInputEdit = FALSE);

	// display the user input in "ValueInputEditBox".
	void DisplayUserInput(CString strUserInput);

private:
	// the singleton object
	static InteractBar *m_pInteractBar;

private:
	/************************************************************************/
	// about the prompt message.
	
	// this edit box show the message to user.
	CEdit m_msgEdit;

	// keep the previous message ID, if the next msg ID is same as this,
	// we do not show it again, otherwise in PartView::OnMouseMove we will
	// display the same message for many time.
	int m_iMsg;
	/************************************************************************/


	/************************************************************************/
	// about the user input.

	// the edit box which can accept input.
	ValueInputEditBox m_valEdit;

	// height for command window
	int m_iComHeight;

	// whether display the "ValueInputEditBox"
	BOOL m_bDisplay;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
