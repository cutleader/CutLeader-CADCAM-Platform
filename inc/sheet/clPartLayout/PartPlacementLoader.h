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

// ��������á���������
class ClPartLayout_Export PartPlacementLoader
{
public:
	// ���ذ�������еġ�������á�����
	/* ������
	*  cadDataList�� ������������õ�������ġ�CAD���ݡ���
	*/
	static void LoadPartPmts(DBConnectPtr pDBConnect, std::map<LONGLONG, PartCadDataPtr>& cadDataList, PartPlacementListPtr pPartPlacements);
	static void LoadPartPmts(DBConnectPtr pDBConnect, std::map<LONGLONG, std::pair<CString, PartCadDataPtr>>& cadDataList, PartPlacementListPtr pPartPlacements);

	// load part pmt by ID.
	static PartPlacementPtr LoadPartPmt(DBConnectPtr pDBConnect, LONGLONG iPartPmtID);

	// how many part is involved in sheet.
	static int GetPartInstanceCount(DBConnectPtr pDBConnect, LONGLONG iSheetId, LONGLONG iPartID);

    // �õ�����ϸ������������
    static std::map<LONGLONG, int> GetPartCountData_inSheet(DBConnectPtr pDBConnect, LONGLONG iSheetId, int iSheetCount);

	// �õ�����в�ͬ�����Ϣ��
	// ע��
	//   1) data in a DataProperty:
	//      1) ���ID��
	//		2) ������ơ�
	static std::vector<DataProperty> LoadPartInfoList(DBConnectPtr pDBConnect);

	// ���ذ�����������õ�������ġ�CAD���ݡ���
	// ע:
	//   1) ��CAD���ݡ��ĸ����Ͱ�������õ�������ĸ���һ����
	//   2) map�еĵ�һ�������������ID��
	static std::map<LONGLONG, PartCadDataPtr> LoadAllPartCadData(DBConnectPtr pDBConnect);

private:
	// ���ء�������
	// ע��
	//   1) �������������ݡ��Ӱ�������ļ��м��ء�
	static PartItemPtr LoadPartItem(DBConnectPtr pDBConnect, LONGLONG iPartID, LONGLONG iCamDataID);
	static PartItemPtr LoadPartItem(DBConnectPtr pDBConnect, LONGLONG iPartID, PartCadDataPtr pPartCadData, LONGLONG iCamDataID);
	static PartItemPtr LoadPartItem(DBConnectPtr pDBConnect, LONGLONG iPartID, CString strPartName, PartCadDataPtr pPartCadData, LONGLONG iCamDataID);
};

END_CUTLEADER_NAMESPACE()
