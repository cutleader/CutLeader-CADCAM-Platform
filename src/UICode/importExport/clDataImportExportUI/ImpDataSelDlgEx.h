#pragma once

#include "clDataImportExportCommon.h"
#include "clDataImportExportUICommon.h"
#include "taoResource.h"
#include "LineArc2DStaticControl.h"


DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(DxfData)


BEGIN_CUTLEADER_NAMESPACE()


// ����Ի�������ѡ�������ݡ�
// ע��
//   1) Ŀǰ֧��dxf/dwg, �������ġ�
//   2) ����Ի���֧��Ŀ¼ѡ��
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
	// ��ʼ������·����
	void SetSavePath(FolderNodePtr pFolderNode, BOOL bEnable) { m_pSaveFolder = pFolderNode; m_bEnableFolder = bEnable; }

	// get the data type which will be imported.
	// notes:
	//   1) this function will check whether "strFilePath" is a legal importable file, if not, return IMPEXP_DATA_NOT_INIT.
	IMPEXP_DATA_TYPE GetImpType(CString strFilePath);

	// �õ�����·����
	FolderNodePtr GetSavePath() { return m_pSaveFolder; }

    // �����ߵ���dxf/dwgʱ����Ҫ������������õ�dxf data
    DxfDataPtr GetDxfData() const { return m_pDxfData; }

private: // �ؼ�
	// Ԥ�����ڡ�
	LineArc2DStaticControl m_ctrlPreviewWnd;

	// ����·��
	CString m_ctrlSavePath;

private: // ����
	// ����Ŀ¼��
	FolderNodePtr m_pSaveFolder;

	// �Ƿ��ܱ༭Ŀ¼��
	BOOL m_bEnableFolder;

	/************************************************************************/
	// ���ڴ�dxf�ļ�

	// ������ʱ�û��ڡ��ļ�������������ļ�����ֱ��OK����ʱ����Ҫ֪����Ҫ�򿪵��ļ��ǲ����Ѿ���OnFileNameChange���ع��ˡ�
	CString m_strDxfFileName_loadedIn_OnFileNameChange;

	// ��������¸������ֵ��1)���ļ�������������ļ�������ȷ��OKʱ��2)ѡ�в�ͬ�ļ�ʱ��������ֵ��
	// ע������ѡ�ļ�ʱ�����������ֵ��
	DxfDataPtr m_pDxfData;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
