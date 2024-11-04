#pragma once

#include "clDataImportExportCommon.h"
#include "IBusinessData.h"

DECLARE_CUTLEADER_CLASS(ExpertLibItem)
DECLARE_CUTLEADER_CLASS(Material)
DECLARE_CUTLEADER_CLASS(Part)
DECLARE_CUTLEADER_CLASS(Sheet)
DECLARE_CUTLEADER_CLASS(NestJob)
DECLARE_CUTLEADER_CLASS(ExportDataInfo)

BEGIN_CUTLEADER_NAMESPACE()

// ���ݵ�����������
class ClDataImportExport_Export ExportManager
{
public:
	// export part to a file.
	static BOOL ExportPart(PartPtr pPart, CString strFilePath, IMPEXP_DATA_TYPE expType);

	// export sheet to a file.
	// notes:
	//   1) you know, material info will come with sheet.
	static BOOL ExportSheet(SheetPtr pSheet, CString strFilePath, IMPEXP_DATA_TYPE expType);

	// �����Ű���ҵ
	// ע:
	//  1) ��������sheetΪdxf������ָ��Ŀ¼���棬����ȷ��Ŀ¼���ڣ��������̲����ظ��ļ��жϡ�
	static BOOL ExportNestJob(NestJobPtr pNestJob, CString strFolder);

	// export "pExpLibItem" to file.
	// notes:
	//   1) the MacDesc info also will be saved into the db.
	static BOOL ExportExpLibItem(ExpertLibItemPtr pExpLibItem, CString strFilePath);

	// �������ϡ�
	static BOOL ExportMat(MaterialPtr pMat, CString strFilePath);

private:
	/************************************************************************/
	// about exporting part.

	// export part as dxf/dwg file.
	static BOOL ExpPartAsDxfDwg(PartPtr pPart, CString strFilePath, IMPEXP_DATA_TYPE expType);

	// export part as part.
	static BOOL ExpPartAsPart(PartPtr pPart, CString strFilePath);
	/************************************************************************/

	/************************************************************************/
	// about exporting sheet.

	// export sheet as dxf/dwg file.
	static BOOL ExpSheetAsDxfDwg(SheetPtr pSheet, CString strFilePath, IMPEXP_DATA_TYPE expType);

	// export sheet as sheet.
	// ע:
	//   1) ֻ��������е������CAD���ݣ����ذ��ʱֻ��Ҫ�����CAD���ݡ�
	//   2) we will also export the material used by the sheet.
	static BOOL ExpShtAsSht(SheetPtr pSheet, CString strFilePath);
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
