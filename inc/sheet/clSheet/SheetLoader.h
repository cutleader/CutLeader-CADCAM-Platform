#pragma once

#include "clSheetCommon.h"
#include "DataProperty.h"
#include "DataPropItem.h"
#include <vector>


DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(ImgData)
DECLARE_CUTLEADER_CLASS(Sheet)
DECLARE_CUTLEADER_CLASS(PartGroup)
DECLARE_CUTLEADER_CLASS(PartGroupList)
DECLARE_CUTLEADER_CLASS(PartPlacementList)


BEGIN_CUTLEADER_NAMESPACE()

// the sheet load type.
typedef enum tagSheetLoadType
{
	// basic data/MatSize/schemeID will be loaded.
	SHEETLOAD_BASIC				= 0,

	// load for preview, compare with "SHEETLOAD_BASIC" option, preview data will be loaded.
	SHEETLOAD_PREVIEW			= 1,

	// load all data.
	SHEETLOAD_ALL				= 2,
} SHEETLOAD_TYPE;

// use this class to load sheet from database.
class ClSheet_Export SheetLoader
{
public:
	// �õ����������������а����Ϣ��
	// ע:
	//   1) data in a DataProperty: 
	//      1) sheet id
	//      2) sheet name
	//      3) create date
	//      4) modify date
	//      5) sheet comment
	static BOOL GetSheetInfoList(DBConnectPtr pDBConnect, std::vector<DataProperty>& sheetInfoList, LONGLONG iPartID);

	// load the sheet from database.
	// ע:
	//   1) ���ĳ������·������������Ч����������Ϊ�գ����Ժ�����Ҫ��������
	//   2) �������������˻�·����ô����½�û����֮��Ӧ�ġ���·�ӹ����ա������Ժ�����Ҫ��������
	//   3) after loading sheet, the cache data in sheet will be all updated.
	static SheetPtr LoadSheet(DBConnectPtr pDBConnect, LONGLONG iSheetID, SHEETLOAD_TYPE loadType = SHEETLOAD_ALL);

	// get the matLib/expLib info of sheet.
	static LONGLONG GetMaterialSizeID(DBConnectPtr pDBConnect);
	static LONGLONG GetParamConfigID(DBConnectPtr pDBConnect);

	// ����Ԥ�����ݡ�
	// ע��
	//   1) ���û��Ԥ�����ݵĻ������ؿ�ֵ��
	static ImgDataPtr LoadPreviewData(DBConnectPtr pDBConnect);

    // ��������������顣
    static void LoadPartGroups(DBConnectPtr pDBConnect, const PartPlacementList* pPartPlacements, PartGroupListPtr pPartGroups);

private:
    // ��һ�ε���ʱ���붥�������ID�������ǵݹ���ذ���������������ڵ�����������飬������pPartGroups�С�
    // ע��
    // 1��ÿ��ݹ鶼���ض�ӦiPartGroupID����������顣
    static PartGroupPtr LoadPartGroup(DBConnectPtr pDBConnect, const PartPlacementList* pAllPartPlacements, PartGroupListPtr pPartGroups, LONGLONG iPartGroupID);
};

END_CUTLEADER_NAMESPACE()
