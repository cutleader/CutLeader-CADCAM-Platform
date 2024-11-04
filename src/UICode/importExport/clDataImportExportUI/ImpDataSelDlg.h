#pragma once

#include "clDataImportExportUICommon.h"
#include "clDataImportExportCommon.h"
#include "LineArc2DStaticControl.h"


DECLARE_CUTLEADER_CLASS(DxfData)


BEGIN_CUTLEADER_NAMESPACE()

// 这个对话框用来选择导入数据。
// 注：
//   1) 这个对话框用来：零件方式打开dxf/dwg, 导入备份数据，导入专家库。
class ClDataImportExportUI_Export ImpDataSelDlg : public CFileDialog
{
	DECLARE_DYNAMIC(ImpDataSelDlg)

public:
	ImpDataSelDlg(BOOL bNeedPreview, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);

	virtual ~ImpDataSelDlg();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnFileNameChange();
	virtual BOOL OnFileNameOK();

protected:
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

public:
	// get the data type which will be imported.
	// notes:
	//   1) this function will check whether "strFilePath" is a legal importable file, if not, return IMPEXP_DATA_NOT_INIT.
	IMPEXP_DATA_TYPE GetImpType(CString strFilePath);

    // 调用者导入dxf/dwg时，需要调用这个函数得到dxf data
    DxfDataPtr GetDxfData() const { return m_pDxfData; }

private: // for control
	// use this window to show the thumbnail
	LineArc2DStaticControl m_previewStatic;

private:
	// whether need the preview control.
	BOOL m_bNeedPreview;

	/************************************************************************/
	// 关于打开dxf文件

	// 由于有时用户在“文件名输入框”输入文件名后直接OK，此时就需要知道将要打开的文件是不是已经被OnFileNameChange加载过了。
	CString m_strDxfFileName_loadedIn_OnFileNameChange;

	// 两种情况下更新这个值：1)在文件名输入框输入文件名并且确认OK时，2)选中不同文件时会更新这个值。
	// 注：当单选文件时，才能用这个值。
	DxfDataPtr m_pDxfData;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
