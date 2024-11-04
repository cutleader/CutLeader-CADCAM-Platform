#pragma once

#include "cutLeaderCommon.h"

#if defined(ClUILib_Home)
#    define ClUILib_Export _declspec(dllexport)
#else
#    define ClUILib_Export _declspec(dllimport)
#endif

// the in-place editor type.
typedef enum tagCellEditorType
{
	READEDITBOX		= 0, // the read only.
	STREDITBOX		= 1,
	INTEDITBOX		= 2,
	FLOATEDITBOX	= 3,
	COMBOBOX		= 4,
	COMBOEDIT		= 5,

} CELL_EDITOR_TYPE;

// the types of units, these type are only used for dimension.
typedef enum tagDimUnitType 
{
    DIM_UNIT_INCH  = 0,
    DIM_UNIT_MM    = 1,
} DIM_UNIT_TYPE;

// the NONE action type, under this action, user can do nothing.
typedef enum tagNoneActionType
{
    ACTION_NONE				= -1,
} NONE_ACTION_TYPE;

typedef enum tagViewActionType
{
    ACTION_ZOOM_IN			= -2,
    ACTION_ZOOM_OUT			= -3,

} VIEW_ACTION_TYPE;
