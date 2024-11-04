
///////////////////////////////////////////////////////////////////////////////
// ID range is (21500~22000)
///////////////////////////////////////////////////////////////////////////////

#define ClDataManager_Resource_StartID	21500

/************************************************************************/
/*                the string ID                                         */

// the name of the task
#define IDS_TASK_DELETE_ITEMS				ClDataManager_Resource_StartID+40
#define IDS_TASK_DELETE_CAMDATA				ClDataManager_Resource_StartID+41
#define IDS_TASK_VERIFY_PART				ClDataManager_Resource_StartID+42
#define IDS_TASK_VERIFY_SHEET				ClDataManager_Resource_StartID+43
#define IDS_TASK_REPAIR_SHEET				ClDataManager_Resource_StartID+44
#define IDS_TASK_REPAIR_PART				ClDataManager_Resource_StartID+45
#define IDS_TASK_CLONE_SHEET				ClDataManager_Resource_StartID+46
// save
#define IDS_TASK_SAVE_PART_PAGE				ClDataManager_Resource_StartID+50
#define IDS_TASK_SAVE_SHEET_PAGE			ClDataManager_Resource_StartID+51
#define IDS_TASK_SAVE_FOLDER_PAGE			ClDataManager_Resource_StartID+52
#define IDS_TASK_SAVE_NEST_PAGE				ClDataManager_Resource_StartID+53
//

// the prompt message
// for part cam data grid.
#define IDS_MSG_DEL_CAMDATA					ClDataManager_Resource_StartID+60
#define IDS_MSG_CHANGE_MAT					ClDataManager_Resource_StartID+61
#define IDS_MSG_CHANGE_SHEETSCHEME			ClDataManager_Resource_StartID+63
#define IDS_MSG_DEL_LAST					ClDataManager_Resource_StartID+64
#define IDS_MSG_DEL_USED_CAMDATA			ClDataManager_Resource_StartID+65
#define IDS_MSG_CAMDATA_USED_SHEET			ClDataManager_Resource_StartID+66
// for delete tree node.
#define IDS_MSG_SEL_FOLDER					ClDataManager_Resource_StartID+70
#define IDS_MSG_DEL_FOLDER					ClDataManager_Resource_StartID+71
#define IDS_MSG_DEL_PART					ClDataManager_Resource_StartID+72
#define IDS_MSG_DEL_SHEET					ClDataManager_Resource_StartID+73
#define IDS_MSG_DEL_NESTJOB					ClDataManager_Resource_StartID+74
#define IDS_MSG_DEL_SEL_ITEMS				ClDataManager_Resource_StartID+75
#define IDS_MSG_DEL_OPENED_ITEMS			ClDataManager_Resource_StartID+76
#define IDS_MSG_DEL_ROOT					ClDataManager_Resource_StartID+77
// for dependence
#define IDS_MSG_PART_USED					ClDataManager_Resource_StartID+80
#define IDS_MSG_SHEET_USED					ClDataManager_Resource_StartID+81
#define IDS_MSG_NESTJOB_USED				ClDataManager_Resource_StartID+82
#define IDS_MSG_FOLDER_USED					ClDataManager_Resource_StartID+83
#define IDS_MSG_SEL_ITEMS_USED				ClDataManager_Resource_StartID+84
#define IDS_MSG_NO_DEPENDENCE				ClDataManager_Resource_StartID+85
// for copy
#define IDS_MSG_CANNOT_COPY_SHEET			ClDataManager_Resource_StartID+90
#define IDS_MSG_CANNOT_COPY_NEST			ClDataManager_Resource_StartID+91
// for repair part
#define IDS_MSG_PART_REPAIR					ClDataManager_Resource_StartID+100
#define IDS_MSG_PART_NOT_REPAIR				ClDataManager_Resource_StartID+101
#define IDS_MSG_CAMDATA_SCHEME_LOST			ClDataManager_Resource_StartID+102
#define IDS_MSG_CAMDATA_SCHEME_REPAIRED		ClDataManager_Resource_StartID+103
// for repair sheet
#define IDS_MSG_SHEET_REPAIR				ClDataManager_Resource_StartID+110
#define IDS_MSG_SHEET_MAT_LOST				ClDataManager_Resource_StartID+111
#define IDS_MSG_SHEET_MAT_REPAIRED			ClDataManager_Resource_StartID+112
#define IDS_MSG_SHEET_SCHEME_LOST			ClDataManager_Resource_StartID+113
#define IDS_MSG_SHEET_SCHEME_REPAIRED		ClDataManager_Resource_StartID+114
#define IDS_MSG_CAMDATA_EXPLIB_NOTSAME		ClDataManager_Resource_StartID+115
#define IDS_MSG_CAMDATA_REPLACED			ClDataManager_Resource_StartID+116
#define IDS_MSG_CAMDATA_CREATE				ClDataManager_Resource_StartID+117
// for save
#define IDS_MSG_SAVE_PART					ClDataManager_Resource_StartID+150
#define IDS_MSG_SAVE_SHEET					ClDataManager_Resource_StartID+151
#define IDS_MSG_SAVE_NESTJOB				ClDataManager_Resource_StartID+152
#define IDS_MSG_SAVE_FOLDER					ClDataManager_Resource_StartID+153
#define IDS_PART_PAGE_SAVED					ClDataManager_Resource_StartID+154
#define IDS_PART_PAGE_NOT_CHANGED			ClDataManager_Resource_StartID+155
#define IDS_SHEET_PAGE_SAVED				ClDataManager_Resource_StartID+156
#define IDS_SHEET_PAGE_NOT_CHANGED			ClDataManager_Resource_StartID+157
#define IDS_FOLDER_PAGE_SAVED				ClDataManager_Resource_StartID+158
#define IDS_FOLDER_PAGE_NOT_CHANGED			ClDataManager_Resource_StartID+159
#define IDS_NESTJOB_PAGE_SAVED				ClDataManager_Resource_StartID+160
#define IDS_NESTJOB_PAGE_NOT_CHANGED		ClDataManager_Resource_StartID+161
// for opened data item.
#define IDS_MSG_PART_OPENED					ClDataManager_Resource_StartID+220
#define IDS_MSG_SHEET_OPENED				ClDataManager_Resource_StartID+221
// for execute task.
#define IDS_MSG_IMPORT_DATA					ClDataManager_Resource_StartID+250
#define IDS_MSG_PARSING_DXFDWG				ClDataManager_Resource_StartID+251
#define IDS_MSG_SAVING_DATA_DB				ClDataManager_Resource_StartID+252
#define IDS_MSG_SAVED_DATA_DB				ClDataManager_Resource_StartID+253
#define IDS_MSG_IMPORTED_ALL_DATA			ClDataManager_Resource_StartID+254
#define IDS_MSG_IMPORT_TASK_OK				ClDataManager_Resource_StartID+255
#define IDS_MSG_IMPORT_TASK_CANCEL			ClDataManager_Resource_StartID+256
//

// used for popup menu.
#define IDS_CREATE_FOLDER					ClDataManager_Resource_StartID+350
#define IDS_CREATE_PART						ClDataManager_Resource_StartID+351
#define IDS_CREATE_SHEET					ClDataManager_Resource_StartID+352
#define IDS_CREATE_NESTJOB					ClDataManager_Resource_StartID+353
#define IDS_SPLIT_IMPORT_DATA				ClDataManager_Resource_StartID+354
#define IDS_VIEW_DEPEND						ClDataManager_Resource_StartID+355
//

/************************************************************************/

/************************************************************************/
/*                the control created by code                           */

// used for popup menu.
#define IDC_CREATE_FOLDER					ClDataManager_Resource_StartID+400
#define IDC_CREATE_PART						ClDataManager_Resource_StartID+401
#define IDC_CREATE_SHEET					ClDataManager_Resource_StartID+402

#define IDC_EXPL_IMPORT						ClDataManager_Resource_StartID+410
#define IDC_EXPL_SPLIT_IMPORT				ClDataManager_Resource_StartID+411
#define IDC_EXPL_EXPORT						ClDataManager_Resource_StartID+412

#define IDC_VIEW_DEPEND						ClDataManager_Resource_StartID+420
#define IDC_DEL_PRODUCTITEM					ClDataManager_Resource_StartID+421
//

/************************************************************************/
