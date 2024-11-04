#pragma once

#include "clSheetCommon.h"


DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(ImgData)
DECLARE_CUTLEADER_CLASS(Sheet)
DECLARE_CUTLEADER_CLASS(PartGroupList)


BEGIN_CUTLEADER_NAMESPACE()

// use this class to write sheet to database.
class ClSheet_Export SheetWriter
{
public:
	// save sheet to db.
	// notes:
	//   1) this function only update sheet db.
	//   2) 这个函数直接把数据写入数据库。
	static BOOL SaveSheet(DBConnectPtr pDBConnect, SheetPtr pSheet, BOOL bTransaction = TRUE);

	// delete the sheet from db.
	// notes:
	//   1) this function only update sheet db.
	static void DeleteSheet(DBConnectPtr pDBConnect, BOOL bTransaction = TRUE);

	/************************************************************************/
	// update some lib info of the sheet into database.

	// only update "Sheet::m_pMatSize" into database.
	// notes:
	//   1) after call this, must call function "UpdateModifyDate".
	static void UpdateMatSize(DBConnectPtr pDBConnect, SheetPtr pSheet, BOOL bTransaction = TRUE);

	// 修改板材的“参数配置”。
	// notes:
	//   1) after call this, must call function "UpdateModifyDate".
	static void UpdateParamConfig(DBConnectPtr pDBConnect, SheetPtr pSheet, BOOL bTransaction = TRUE);
	/************************************************************************/

	// about update "clSheet_usedPart".
	static void SaveUsedPartInfo(DBConnectPtr pDBConnect, LONGLONG iSheetID, IntegerListPtr pPartIDList, BOOL bTransaction = TRUE);
	static void DeleteUsedPartInfo(DBConnectPtr pDBConnect, LONGLONG iSheetID, BOOL bTransaction = TRUE);

private:
	// 保存预览数据。
	// 注：
	//   1) 这个函数直接把数据写入数据库。
	static void SavePreviewData(DBConnectPtr pDBConnect, ImgDataPtr pImgData, BOOL bTransaction = TRUE);

    // 把零件分组信息写入数据库。
    static void SavePartGroups2DB(DBConnectPtr pDBConnect, PartGroupListPtr pPartGroupList);
};

END_CUTLEADER_NAMESPACE()
