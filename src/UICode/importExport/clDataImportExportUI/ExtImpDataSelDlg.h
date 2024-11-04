#pragma once

#include "clDataImportExportCommon.h"
#include "clDataImportExportUICommon.h"
#include "baseConst.h"
#include "taoResource.h"
#include "LineArc2DStaticControl.h"


DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(DxfData)


BEGIN_CUTLEADER_NAMESPACE()

// 这个对话框用来选择导入外部数据。
// 注：
//   1) 目前支持dxf/dwg。
//   2) 这个对话框支持目录和专家库选择。
class ClDataImportExportUI_Export ExtImpDataSelDlg : public CFileDialog
{
	DECLARE_DYNAMIC(ExtImpDataSelDlg)

public:
	ExtImpDataSelDlg(LPCTSTR lpszDefExt = NULL, DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
	virtual ~ExtImpDataSelDlg();

	enum { IDD = IDD_clDataImportExportUI_SelectExternalData };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnFileNameChange();
	virtual BOOL OnFileNameOK();

protected: // message mapping.
	afx_msg void OnDestroy();
	afx_msg void OnSelectFolder();
	afx_msg void OnSelchangeExpLibItem();

	DECLARE_MESSAGE_MAP()

public:
	/************************************************************************/
	// 初始化。

	// 初始化保存路径。
	/* 参数：
	*  bEnable: 是否能编辑路径。
	*/
	void SetSavePath(FolderNodePtr pFolderNode, BOOL bEnable);

	// 设置专家库项。
	/* 参数：
	*  bEnable: 是否能编辑专家库。
	*/
	void SetExpLibItem(LONGLONG iExpLibItemID, BOOL bEnable);

	// 设置参数配置。
	void SetParamConfig(LONGLONG iParamConfigID, BOOL bEnable);
	/************************************************************************/

	/************************************************************************/
	// 获得用户选择结果。

	// get the data type which will be imported.
	// notes:
	//   1) this function will check whether "strFilePath" is a legal importable file, if not, return IMPEXP_DATA_NOT_INIT.
	IMPEXP_DATA_TYPE GetImpType(CString strFilePath) const;

	// 得到保存路径。
	FolderNodePtr GetSavePath() const { return m_pSaveFolder; }

	// 得到零件方案。
	LONGLONG GetPartScheme() const { return m_iParamConfigID; }

    // 调用者导入dxf/dwg时，需要调用这个函数得到dxf data
    DxfDataPtr GetDxfData() const { return m_pDxfData; }
	/************************************************************************/

private:
	// 根据专家库组合框初始化“参数配置”组合框。
	/* 参数：
	*  iCurParamConfigID: 当前要选中的参数配置ID，无效的话选第一个。
	*/
	void InitParamConfigCBX(LONGLONG iCurParamConfigID = INVALID_ID);

	void RlsExpLibItemData();
	void RlsPartSchemeItemData();

private: // 控件
	// 预览窗口。
	LineArc2DStaticControl m_ctrlPreviewWnd;

	// 保存路径
	CString m_ctrlSavePath;

	// 专家库项。
	CComboBox m_ctrlExpLibItem;

	// 零件方案。
	CComboBox m_ctrlPartScheme;

private: // 数据
	// 保存目录。
	FolderNodePtr m_pSaveFolder;

	// 是否能编辑目录。
	BOOL m_bEnableFolder;

	// 默认专家库项。
	LONGLONG m_iExpLibItemID;

	// 是否能编辑专家库。
	BOOL m_bEnableExpLib;

	// 参数配置。
	LONGLONG m_iParamConfigID;

	// 是否能编辑参数配置。
	BOOL m_bEnableConfig;

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
