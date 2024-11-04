#pragma once

#include "cutLeaderCommon.h"

#if defined(ClCutSequenceUI_Home)
#    define ClCutSequenceUI_Export _declspec(dllexport)
#else
#    define ClCutSequenceUI_Export _declspec(dllimport)
#endif

// ��Щ��������������
enum SequenceActionType
{
	SEQUENCE_SELECT				= 100,
	SEQUENCE_REMOVE				= 101,
	SEQUENCE_SORT_A				= 102, // ��ͷ��ʼ���������˳��
	SEQUENCE_SORT_B				= 103, // ����ѡ�й����˳��
	SEQUENCE_REPLACE_HOLE_SEQ	= 104,

	// ���ڡ���·���򡱡�
	SEQUENCE_ADD_LOOPSEQ		= 110,
	SEQUENCE_INSERT_LOOPSEQ		= 111,

	// ���ڡ��Žӹ��򡱡�
	SEQUENCE_ADD_BDGESEQ		= 120,
	SEQUENCE_BREAK_BDGESEQ		= 121,
	SEQUENCE_BDGE_START_CHANGE	= 122,

	// ���ڡ�����򡱡�
    SEQUENCE_ADD_RESET_INSTRUCT	= 123, // ��Ӹ�λָ��
    SEQUENCE_DEL_RESET_INSTRUCT	= 124, // ɾ����λָ��
	SEQUENCE_ADD_CHAINSEQ		= 125,
	SEQUENCE_INSERT_CHAINNODE	= 126,
	SEQUENCE_DEL_CHAINNODE		= 127,

	// ���ڡ����ֹ��򡱡�
	SEQUENCE_ADD_GRIDSEQ		= 128,

	// ���ڡ������и�򡱡�
	SEQUENCE_REPLACE_AS_FLYCUT	= 130,

	// ���ڱ༭���˶��ڵ㡱��
	SEQUENCE_ADD_MOV_PT			= 140,
	SEQUENCE_DEL_MOV_PT			= 141,

	// �����и���
	SEQUENCE_AUTO_REM_CUTLINE	= 145,
	SEQUENCE_ADD_REM_CUTLINE	= 146,
	SEQUENCE_DEL_REM_CUTLINE	= 147,

	SEQUENCE_VIEW				= 150,

    // ��Щ��������ʾ��
    SEQUENCE_Remove_LoopSequence	= 180,
    SEQUENCE_RealTime_Cut           = 181,
};

