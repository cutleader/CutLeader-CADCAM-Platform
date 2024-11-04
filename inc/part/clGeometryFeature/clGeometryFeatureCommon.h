#pragma once

#include "cutLeaderCommon.h"

#if defined(ClGeometryFeature_Home)
#    define ClGeometryFeature_Export _declspec(dllexport)
#else
#    define ClGeometryFeature_Export _declspec(dllimport)
#endif

