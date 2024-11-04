#pragma once

#include "clDataImportExportCommon.h"
#include "DataProperty.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(IBusinessData)
DECLARE_CUTLEADER_CLASS(PartCadData)
DECLARE_CUTLEADER_CLASS(ExpertLibItem)
DECLARE_CUTLEADER_CLASS(Material)
DECLARE_CUTLEADER_CLASS(DxfData)

BEGIN_CUTLEADER_NAMESPACE()

// this class will care the details of import.
// notes:
//   1) this class only import data from backup db file to business object.
//   2) each released application edition should know a version list of the db which it support,
//      if importing db version is not same with that current app used and it's within the support list, we should write a "convertor" to import it.
class ClDataImportExport_Export ImportManager
{
public:
	/************************************************************************/
	// about importing dxf/dwg.
	
	// get the basic geoms of dxf/dwg file.
	static LineArc2DListPtr GetDxfDwgBaseGeoms(CString strFilePath, BOOL bDwg);

    // 得到dxf的基本图形和dxf data信息。
    static std::pair<LineArc2DListPtr, DxfDataPtr> GetDxfDwgBaseGeomsAndDxfData(CString strFilePath, BOOL bDwg);

	// 导入dxf/dwg文件为零件对象。
	static IBusinessDataPtr ImportDxfDwg2Part(CString strFilePath, BOOL bDwg);
    static IBusinessDataPtr ImportDxfDwg2Part(CString strFilePath, DxfDataPtr pDxfData);

	// 导入dxf/dwg文件为PartCadData对象。
	/* 参数：
	*  bProcessGrid: 要不要组成阵列。
	*/
	static PartCadDataPtr DxfDwg2CadData(CString strFilePath, BOOL bDwg, BOOL bProcessGrid);
    static PartCadDataPtr DxfDwg2CadData(CString strFilePath, const DxfData* pDxfData, BOOL bProcessGrid);
	/************************************************************************/

	/************************************************************************/
	// about importing part.

	// get the basic geoms of .CLPrt file.
	static LineArc2DListPtr GetPrtBaseGeom(CString strFilePath);

	// import a Prt file as a part object.
	// notes:
	//   1) the create/modify date will be set in this function.
	static IBusinessDataPtr ImportPartObj(CString strFilePath);
	/************************************************************************/

	/************************************************************************/
	// about importing sheet.

	// get the basic geoms of .CLSht file.
	static LineArc2DListPtr GetShtBaseGeom(CString strFilePath);

	// import a Sht file as a sheet object.
	// notes:
	//   1) this function should also return the exported part info, we use them to create part.
	//   2) the create/modify date of the sheet will be set in this function.
	static IBusinessDataPtr ImportSheetObj(CString strFilePath, std::vector<DataProperty>& prtExpInfoList);
	/************************************************************************/

	// import a TAOExpLib file as a ExpertLibItem object.
	// notes:
	//   1) driver version: when import expert library or driver, we must make sure the driver version is
	//      same with the current.
	static ExpertLibItemPtr ImportExpLibItem(CString strFilePath);

	// 导入材料。
	static MaterialPtr ImportMat(CString strFilePath);
};

END_CUTLEADER_NAMESPACE()
