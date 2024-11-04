#pragma once

#include "cutLeaderCommon.h"

#if defined(ClNestJob_Home)
#    define ClNestJob_Export _declspec(dllexport)
#else
#    define ClNestJob_Export _declspec(dllimport)
#endif

enum PART_ROT_STYLE
{
	// part can be rotated freely.
	PART_ROT_FREE				= 0,

	// part can be rotated by 90 degree increment, that is, part can be placed in sheet with 0/90/180/270 degree.
	PART_ROT_PID2_INCREMENT		= 1,

	// part can be rotated by 0 degree.
	PART_ROT_ZERO_FIXED			= 2,

	// part can be rotated by 90 degree.
	PART_ROT_PID2_FIXED			= 3,

	// part can be rotated by 180 degree.
	PART_ROT_PI_FIXED			= 4,

	// part can be rotated by 270 degree.
	PART_ROT_PID23_FIXED		= 5,

	// 0度、180度旋转。
	PART_ROT_HORIZONTAL			= 6,

	// 90度、270度旋转。
	PART_ROT_VERTICAL			= 7,
};