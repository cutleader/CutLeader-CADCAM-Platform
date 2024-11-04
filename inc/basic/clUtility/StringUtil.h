#pragma once

#include "clUtilityCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// this class will perform some conversions about the string.
class ClUtility_Export StringUtil
{
public:
	// convert CString object to char*
	// notes: 
	//   1) for Unicode env, we need to convert.
	//   2) "pszData" will be allocated by the caller.
	static void UnicodeToAscii(const CString &csFilestr, char* pszData);

	// notes: allocate "wchars" and "schars" by the caller.
	static void WideCharToMulByte(WCHAR* wchars, CHAR* schars, int scharsLen);

	// notes: allocate "schars" by the caller.
	static CString MulByteToWideChar(char* schars);

	// convert utf-8 string to unicode.
	static CString UTF8ToUnicode(const char* pszUtf8, UINT iLength);

	// convert unicode string to utf-8.
	// notes:
	//   1) "pszData" will be allocated by the caller.
	static void UnicodeToUtf8(CString strUnicode, char* pszData);
};

END_CUTLEADER_NAMESPACE()
