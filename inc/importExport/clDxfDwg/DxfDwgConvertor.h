#pragma once

#include "clDxfDwgCommon.h"

DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(DxfData)

BEGIN_CUTLEADER_NAMESPACE()

// this class cares about conversions about dxf/dwg.
class ClDxfDwg_Export DxfDwgConvertor
{
public:
	// convert dxf/dwg file to DxfData object.
	// notes:
	//   1) after get all patterns, update the cache data of pattern object.
	static DxfDataPtr ExtractPatList(CString strFilePath, BOOL bDwg = TRUE);

	// export the pattern list to dxf/dwg file.
	/* params:
	*  strFilePath: e.g. "d:\file\a.dxf"
	*/
	static BOOL ExportDxfDwg(const PatternList* pPatList, CString strFilePath, BOOL bDwg = TRUE);

private:
	// convert to dxf/dwg.
	/* params:
	*  bDxf: whether convert to dxf file.
	*/
	static BOOL ConvertDXFDWG(CString strSrcFile, CString strDesFile, BOOL bDxf = TRUE);
};

END_CUTLEADER_NAMESPACE()
