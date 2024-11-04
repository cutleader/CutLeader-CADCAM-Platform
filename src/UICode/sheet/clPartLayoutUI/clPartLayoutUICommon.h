#pragma once

#include "cutLeaderCommon.h"

#if defined(ClPartLayoutUI_Home)
#    define ClPartLayoutUI_Export _declspec(dllexport)
#else
#    define ClPartLayoutUI_Export _declspec(dllimport)
#endif

enum EDIT_SHEET_ACTION_TYPE
{
	// ������֡�
	EDIT_ADDPARTPMT		= 100,
	EDIT_ADDPARTPMT_EX	= 101,
	EDIT_SELECTPRTPMT	= 110,
	EDIT_SELECTPRTINST	= 111,
	EDIT_DELETEPARTPMT	= 112,
	EDIT_ROTATEPARTPMT	= 113,
	EDIT_MOVEPRTPMT		= 114,
	EDIT_COPYPRTPMT		= 115,
	EDIT_GRIDPRTPMT		= 116,
	EDIT_EXPLODE_GRID	= 117,
    EDIT_CreatePartGroup	= 118,
    EDIT_ExplodePartGroup	= 119,
    EDIT_SelectLoopInstance	= 120,
    EDIT_SelectPartOrLoopInstance	= 121, // �༭��ʹ����һ�ε�ѡ��״̬

	// ���칤�ա�
	LEAD_ADJUST			= 219,
	START_PT_ADJUST		= 220,
	CUTSIDE_ADJUST		= 222,
	DIRECTION_ADJUST	= 223,
	PMT_MJ_ADD			= 224,
	PMT_MJ_EDIT			= 225,
	PMT_MJ_DELETE		= 226,
	PMT_CORNER_ADD		= 227,
	PMT_CORNER_EDIT		= 228,
	PMT_CORNER_DELETE	= 229,
	LOOPTOOL_ADJUST		= 231,

	// �������롣
	MEASURE_PT_DISTANCE	= 340,
};