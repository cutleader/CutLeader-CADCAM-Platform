#pragma once

#include "clDataImportExportCommon.h"

DECLARE_CUTLEADER_CLASS(DxfData)

BEGIN_CUTLEADER_NAMESPACE()

// this class defines the info of the import action.
// notes:
//   1) this data come from "ImpDataSelDlg".
class ClDataImportExport_Export ImportDataInfo
{
public:
	ImportDataInfo(void);
	ImportDataInfo(CString strImpFilePath, IMPEXP_DATA_TYPE impDataType);
	~ImportDataInfo(void);

public: // get/set functions.
	void SetImpFilePath(CString strImpFilePath) { m_strImpFilePath = strImpFilePath; }
	CString GetImpFilePath() const { return m_strImpFilePath; }

	void SetImpDataType(IMPEXP_DATA_TYPE impDataType) { m_impDataType = impDataType; }
	IMPEXP_DATA_TYPE GetImpDataType() const { return m_impDataType; }

    void SetDxfData(DxfDataPtr pDxfData) { m_pDxfData = pDxfData; }
    DxfDataPtr GetDxfData() const { return m_pDxfData; }

public:
	// the path of the data which will be imported.
	CString m_strImpFilePath;

	// the type of the data which will be imported. 
	IMPEXP_DATA_TYPE m_impDataType;

    // ���ֵ��Ϊ��ʱ��˵��ֻ�Ӵ򿪶Ի�����һ��dxf������ֱ��������������������ٴζ�ȡdxf�ˡ�
    DxfDataPtr m_pDxfData;
};

END_CUTLEADER_NAMESPACE()
