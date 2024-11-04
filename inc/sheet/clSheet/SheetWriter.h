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
	//   2) �������ֱ�Ӱ�����д�����ݿ⡣
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

	// �޸İ�ĵġ��������á���
	// notes:
	//   1) after call this, must call function "UpdateModifyDate".
	static void UpdateParamConfig(DBConnectPtr pDBConnect, SheetPtr pSheet, BOOL bTransaction = TRUE);
	/************************************************************************/

	// about update "clSheet_usedPart".
	static void SaveUsedPartInfo(DBConnectPtr pDBConnect, LONGLONG iSheetID, IntegerListPtr pPartIDList, BOOL bTransaction = TRUE);
	static void DeleteUsedPartInfo(DBConnectPtr pDBConnect, LONGLONG iSheetID, BOOL bTransaction = TRUE);

private:
	// ����Ԥ�����ݡ�
	// ע��
	//   1) �������ֱ�Ӱ�����д�����ݿ⡣
	static void SavePreviewData(DBConnectPtr pDBConnect, ImgDataPtr pImgData, BOOL bTransaction = TRUE);

    // �����������Ϣд�����ݿ⡣
    static void SavePartGroups2DB(DBConnectPtr pDBConnect, PartGroupListPtr pPartGroupList);
};

END_CUTLEADER_NAMESPACE()
