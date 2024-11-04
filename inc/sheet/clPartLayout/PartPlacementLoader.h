#pragma once

#include "clPartLayoutCommon.h"
#include "DataProperty.h"
#include <vector>
#include <map>

DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(PartCadData)
DECLARE_CUTLEADER_CLASS(PartItem)
DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartPlacementList)

BEGIN_CUTLEADER_NAMESPACE()

// “零件放置”加载器。
class ClPartLayout_Export PartPlacementLoader
{
public:
	// 加载板材中所有的“零件放置”对象。
	/* 参数：
	*  cadDataList： 板材中所有引用到的零件的“CAD数据”。
	*/
	static void LoadPartPmts(DBConnectPtr pDBConnect, std::map<LONGLONG, PartCadDataPtr>& cadDataList, PartPlacementListPtr pPartPlacements);
	static void LoadPartPmts(DBConnectPtr pDBConnect, std::map<LONGLONG, std::pair<CString, PartCadDataPtr>>& cadDataList, PartPlacementListPtr pPartPlacements);

	// load part pmt by ID.
	static PartPlacementPtr LoadPartPmt(DBConnectPtr pDBConnect, LONGLONG iPartPmtID);

	// how many part is involved in sheet.
	static int GetPartInstanceCount(DBConnectPtr pDBConnect, LONGLONG iSheetId, LONGLONG iPartID);

    // 得到板材上各种零件的数量
    static std::map<LONGLONG, int> GetPartCountData_inSheet(DBConnectPtr pDBConnect, LONGLONG iSheetId, int iSheetCount);

	// 得到板材中不同零件信息。
	// 注：
	//   1) data in a DataProperty:
	//      1) 零件ID。
	//		2) 零件名称。
	static std::vector<DataProperty> LoadPartInfoList(DBConnectPtr pDBConnect);

	// 加载板材中所有引用到的零件的“CAD数据”。
	// 注:
	//   1) “CAD数据”的个数和板材中引用到的零件的个数一样。
	//   2) map中的第一个数据是零件的ID。
	static std::map<LONGLONG, PartCadDataPtr> LoadAllPartCadData(DBConnectPtr pDBConnect);

private:
	// 加载“零件项”。
	// 注：
	//   1) “制造特征数据”从板材数据文件中加载。
	static PartItemPtr LoadPartItem(DBConnectPtr pDBConnect, LONGLONG iPartID, LONGLONG iCamDataID);
	static PartItemPtr LoadPartItem(DBConnectPtr pDBConnect, LONGLONG iPartID, PartCadDataPtr pPartCadData, LONGLONG iCamDataID);
	static PartItemPtr LoadPartItem(DBConnectPtr pDBConnect, LONGLONG iPartID, CString strPartName, PartCadDataPtr pPartCadData, LONGLONG iCamDataID);
};

END_CUTLEADER_NAMESPACE()
