#pragma once

#include "cutLeaderCommon.h"

#if defined(PartInstanceCount_Home)
#    define PartInstanceCount_Export _declspec(dllexport)
#else
#    define PartInstanceCount_Export _declspec(dllimport)
#endif

