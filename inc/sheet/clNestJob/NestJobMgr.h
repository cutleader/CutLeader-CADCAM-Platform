#pragma once

#include "clNestJobCommon.h"
#include <vector>
#include "AreaInfo.h"

DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(ReportData)
DECLARE_CUTLEADER_CLASS(ReportTpl)
DECLARE_CUTLEADER_CLASS(Sheet)
DECLARE_CUTLEADER_CLASS(NestJob)
DECLARE_CUTLEADER_CLASS(NestTask)
DECLARE_CUTLEADER_CLASS(NestJobMaterialList)
DECLARE_CUTLEADER_CLASS(MaterialSizeList)

BEGIN_CUTLEADER_NAMESPACE()

class ClNestJob_Export NestJobMgr
{
public:
	// create the nest job.
	static NestJobPtr CreateNestJob(NestTaskPtr pNestTask, NestJobMaterialListPtr pNestJobMats); // ��Ĭ������

	// ըɢ����е�������á���ʱ�����û�й�����Ϣ��
	static void ExplodePartPmts_in_sheets(NestJobPtr pNestJob);

	// save the nest job into database.
	// ע��
	//   1) ���ر���İ�����ݡ�
	static std::vector<SheetPtr> SaveNestJob(NestJobPtr pNestJob, FolderNodePtr pParentFolder);

	// delete the nest job.
	// notes:
	//   1) this function do not delete the relative sheets.
	//   2) this function do not return the material to matLib.
	// To be implemented: we should make sure that no other Job consumed this skeleton.
	static void DeleteNestJob(LONGLONG iNestJobID, BOOL bTransaction);

	// go through each sheet among "pNestJob", and statistic how many part:"iPartID" were nested.
	/* params:
	*  bFromDB: sometimes not all data were loaded into the memory, so need to load from db.
	*/
	// notes:
	//   1) NestTask object may not initialized in "pNestJob".
	static int GetNestedPartCount(const NestJob* pNestJob, LONGLONG iPartID);
    static int GetNestedPartCount_fromDB(const NestJob* pNestJob, LONGLONG iPartID, std::map<LONGLONG, std::map<LONGLONG, int>>& partCountDataInSheets); // nestjob��û���������ݣ�������Ҫ�����ݿ����

	// calculate the utilization of the material.
	/* params:
	*  bFromDB: sometimes not all data were loaded into the memory, so need to load from db.
	*/
	// notes:
	//   1) if no sheets generated during the nesting, return 0.
	static double CalcMatUtilization(const NestJob* pNestJob); // nest job�е�nest task����û�б���ʼ��
	static double CalcMatUtilization_fromDB(const NestJob* pNestJob, std::map<LONGLONG, std::map<LONGLONG, int>>& partCountDataInSheets); // nest job�е�nest task����û�б���ʼ��
	static double CalcMatUtilizationEx(NestJobPtr pNestJob, const AreaInfo& areaInfo, MaterialSizeListPtr pMaterialSizeList);

	// �ӡ��Ű���ҵ�����ɱ���
	static ReportDataPtr GenerateNestRpt(NestJobPtr pNestJob, ReportTplPtr pReportTpl);
};

END_CUTLEADER_NAMESPACE()
