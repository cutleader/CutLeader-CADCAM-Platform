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

// 数据导出管理器。
class ClDataImportExport_Export ExportManager
{
public:
	// export part to a file.
	static BOOL ExportPart(PartPtr pPart, CString strFilePath, IMPEXP_DATA_TYPE expType);

	// export sheet to a file.
	// notes:
	//   1) you know, material info will come with sheet.
	static BOOL ExportSheet(SheetPtr pSheet, CString strFilePath, IMPEXP_DATA_TYPE expType);

	// 导出排版作业
	// 注:
	//  1) 导出所有sheet为dxf，导到指定目录下面，事先确认目录存在，导出过程不做重复文件判断。
	static BOOL ExportNestJob(NestJobPtr pNestJob, CString strFolder);

	// export "pExpLibItem" to file.
	// notes:
	//   1) the MacDesc info also will be saved into the db.
	static BOOL ExportExpLibItem(ExpertLibItemPtr pExpLibItem, CString strFilePath);

	// 导出材料。
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
	// 注:
	//   1) 只导出板材中的零件的CAD数据，加载板材时只需要零件的CAD数据。
	//   2) we will also export the material used by the sheet.
	static BOOL ExpShtAsSht(SheetPtr pSheet, CString strFilePath);
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
