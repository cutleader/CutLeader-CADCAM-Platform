#pragma once

#include "cutLeaderCommon.h"

#if defined(ClBaseData_Home)
#    define ClBaseData_Export _declspec(dllexport)
#else
#    define ClBaseData_Export _declspec(dllimport)
#endif
