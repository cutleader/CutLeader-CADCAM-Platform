#pragma once

#include "cutLeaderCommon.h"

#if defined(ClNestJobUI_Home)
#    define ClNestJobUI_Export _declspec(dllexport)
#else
#    define ClNestJobUI_Export _declspec(dllimport)
#endif

