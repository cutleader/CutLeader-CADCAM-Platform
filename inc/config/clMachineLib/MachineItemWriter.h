#pragma once

#include "clMachineLibCommon.h"

DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(MachineItem)
DECLARE_CUTLEADER_CLASS(MachineLibNode)
DECLARE_CUTLEADER_CLASS(MachineParam)
DECLARE_CUTLEADER_CLASS(DriverData)
DECLARE_CUTLEADER_CLASS(ConditionTypeList)
DECLARE_CUTLEADER_CLASS(ConditionType)
DECLARE_CUTLEADER_CLASS(NCConfigTypeList)
DECLARE_CUTLEADER_CLASS(NCConfigType)

BEGIN_CUTLEADER_NAMESPACE()

// this class save the machine desc into the database.
class ClMachineLib_Export MachineItemWriter
{
public:
	/************************************************************************/
	// deal with the basic data of MachineItem.

	// update the basic data of the MachineItem.
	// notes:
	//   1) after call this, must call function "UpdateModifyDate".
	static void UpdateBasicData(DBConnectPtr pDBConnect, MachineItemPtr pMacItem, BOOL bTransaction = TRUE);

	// update the modified data of the MachineItem.
	static void UpdateModifyDate(DBConnectPtr pDBConnect, LONGLONG iMacItemID, LONGLONG iModifyTimeStamp, BOOL bTransaction = TRUE);
	/************************************************************************/

	// save mac item.
	// notes:
	//   1) user should update create/modify data before call this.
	static void SaveMacItem(DBConnectPtr pDBConnect, MachineItemPtr pMacItem, BOOL bCreate, BOOL bTransaction = TRUE);

	// save machine param.
	static void SaveMacParam(DBConnectPtr pDBConnect, LONGLONG iMacItemID, MachineParamPtr pMacParam, 
							 BOOL bCreate, BOOL bTransaction = TRUE);

	// save the driver data.
	static void SaveDriverData(DBConnectPtr pDBConnect, LONGLONG iMacItemID, DriverDataPtr pDriverData, 
							   BOOL bCreate, BOOL bTransaction = TRUE);

	// save the mac conditions.
	static void SaveMacConds(DBConnectPtr pDBConnect, LONGLONG iMacItemID, ConditionTypeListPtr pCondTypes, 
							 BOOL bCreate, BOOL bTransaction = TRUE);

	// save the nc config types.
	static void SaveNCConfigTypes(DBConnectPtr pDBConnect, LONGLONG iMacItemID, NCConfigTypeListPtr pNCConfigTypeList, 
								  BOOL bCreate, BOOL bTransaction = TRUE);

	// delete the mac item.
	static void DeleteMacItem(DBConnectPtr pDBConnect, LONGLONG iMacItemID, BOOL bTransaction = TRUE);

	// 设置可见的机器。
	static void SetVisibleMac(DBConnectPtr pDBConnect, IntegerListPtr pVisibleMacList);
};

END_CUTLEADER_NAMESPACE()
