#pragma once

#include "clDataImportExportCommon.h"
#include "clDataImportExportUICommon.h"
#include "taoResource.h"
#include "LineArc2DStaticControl.h"


DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(DxfData)


BEGIN_CUTLEADER_NAMESPACE()


// 这个对话框用来选择导入数据。
// 注：
//   1) 目前支持dxf/dwg, 零件，板材。
//   2) 这个对话框支持目录选择。
class ClDataImportExportUI_Export ImpDataSelDlgEx : public CFileDialog
{
	DECLARE_DYNAMIC(ImpDataSelDlgEx)

public:
	ImpDataSelDlgEx(LPCTSTR lpszDefExt = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
	virtual ~ImpDataSelDlgEx();

	enum { IDD = IDD_clDataImportExportUI_SelectExternalData };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnFileNameChange();
	virtual BOOL OnFileNameOK();

protected: // message mapping.
	afx_msg void OnDestroy();
	afx_msg void OnSelectFolder();
	DECLARE_MESSAGE_MAP()

public:
	// 初始化保存路径。
	void SetSavePath(FolderNodePtr pFolderNode, BOOL bEnable) { m_pSaveFolder = pFolderNode; m_bEnableFolder = bEnable; }

	// get the data type which will be imported.
	// notes:
	//   1) this function will check whether "strFilePath" is a legal importable file, if not, return IMPEXP_DATA_NOT_INIT.
	IMPEXP_DATA_TYPE GetImpType(CString strFilePath);

	// 得到保存路径。
	FolderNodePtr GetSavePath() { return m_pSaveFolder; }

    // 调用者导入dxf/dwg时，需要调用这个函数得到dxf data
    DxfDataPtr GetDxfData() const { return m_pDxfData; }

private: // 控件
	// 预览窗口。
	LineArc2DStaticControl m_ctrlPreviewWnd;

	// 保存路径
	CString m_ctrlSavePath;

private: // 数据
	// 保存目录。
	FolderNodePtr m_pSaveFolder;

	// 是否能编辑目录。
	BOOL m_bEnableFolder;

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
