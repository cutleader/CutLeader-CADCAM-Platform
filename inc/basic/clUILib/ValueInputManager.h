#pragma once

#include "clUILibCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// sometimes we need to get user input from keyboard, e.g. draw a line,... this class can check and manage the keyboard input.
class ClUILib_Export ValueInputManager
{
private:
	ValueInputManager(void);

public:
	~ValueInputManager(void);

public:
	static ValueInputManager* GetInstance();

	// this function will process "nChar" which is directly from keyboard, then display the result in InteractBar.
	void AcceptInputChar(const UINT& nChar);

	/************************************************************************/
	// check the input key.

	// whether user wants to end input by press "enter" key.
	BOOL IsInputEnd(const UINT& nChar);

	// whether user wants to cancel the input by press "escape" key.
	BOOL IsInputCancel(UINT nInputChar);

	// 是否按下字母键。
	BOOL IsInputA(UINT nInputChar);
	BOOL IsInputC(UINT nInputChar);
	BOOL IsInputS(UINT nInputChar);
	BOOL IsInputV(UINT nInputChar);
	BOOL IsInputZ(UINT nInputChar);
    BOOL IsInputQ(UINT nInputChar);
    BOOL IsInputW(UINT nInputChar);

	// 是否按下了delete键。
	BOOL IsInputDel(UINT nInputChar);

	// 是否按下了ctrl键。
	BOOL IsInputCtrl();

	// whether user press "+"/"-"
	BOOL IsInputAdd(UINT cInputChar);
	BOOL IsInputDes(UINT cInputChar);

	// 是否按下了4个方向键。
	BOOL IsInputUpArrow(UINT nInputChar);
	BOOL IsInputDownArrow(UINT nInputChar);
	BOOL IsInputLeftArrow(UINT nInputChar);
	BOOL IsInputRightArrow(UINT nInputChar);
	/************************************************************************/

	// get the input from user.
	// notes:  
	//   1) if valid input exists, return them, (one or two value). if no valid input, return FALSE;
	BOOL GetInput(double& dVal1, double& dVal2);

private:
	// whether the input is valid.
	BOOL IsInputValid(UINT nInputChar);

	// convert key code to ascii code.
	// e,g, for '.', ascii code is 46, BUT the keyboard code is 190.
	char KeyCodeToAsciiCode(UINT nInputChar);

private:
	// the singleton object
	static ValueInputManager *g_pValInputMgr;

	// this string will record the legal input until user input an end char.
	// then this string will be put into "ValueInputEditBox".
	CString m_strInputString;

	int m_iBlankNum;
};

END_CUTLEADER_NAMESPACE()
