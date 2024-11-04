#pragma once

#include "clDataImportExportCommon.h"
#include "clDataImportExportUICommon.h"
#include "baseConst.h"
#include "taoResource.h"
#include "LineArc2DStaticControl.h"


DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(DxfData)


BEGIN_CUTLEADER_NAMESPACE()

// ����Ի�������ѡ�����ⲿ���ݡ�
// ע��
//   1) Ŀǰ֧��dxf/dwg��
//   2) ����Ի���֧��Ŀ¼��ר�ҿ�ѡ��
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
	// ��ʼ����

	// ��ʼ������·����
	/* ������
	*  bEnable: �Ƿ��ܱ༭·����
	*/
	void SetSavePath(FolderNodePtr pFolderNode, BOOL bEnable);

	// ����ר�ҿ��
	/* ������
	*  bEnable: �Ƿ��ܱ༭ר�ҿ⡣
	*/
	void SetExpLibItem(LONGLONG iExpLibItemID, BOOL bEnable);

	// ���ò������á�
	void SetParamConfig(LONGLONG iParamConfigID, BOOL bEnable);
	/************************************************************************/

	/************************************************************************/
	// ����û�ѡ������

	// get the data type which will be imported.
	// notes:
	//   1) this function will check whether "strFilePath" is a legal importable file, if not, return IMPEXP_DATA_NOT_INIT.
	IMPEXP_DATA_TYPE GetImpType(CString strFilePath) const;

	// �õ�����·����
	FolderNodePtr GetSavePath() const { return m_pSaveFolder; }

	// �õ����������
	LONGLONG GetPartScheme() const { return m_iParamConfigID; }

    // �����ߵ���dxf/dwgʱ����Ҫ������������õ�dxf data
    DxfDataPtr GetDxfData() const { return m_pDxfData; }
	/************************************************************************/

private:
	// ����ר�ҿ���Ͽ��ʼ�����������á���Ͽ�
	/* ������
	*  iCurParamConfigID: ��ǰҪѡ�еĲ�������ID����Ч�Ļ�ѡ��һ����
	*/
	void InitParamConfigCBX(LONGLONG iCurParamConfigID = INVALID_ID);

	void RlsExpLibItemData();
	void RlsPartSchemeItemData();

private: // �ؼ�
	// Ԥ�����ڡ�
	LineArc2DStaticControl m_ctrlPreviewWnd;

	// ����·��
	CString m_ctrlSavePath;

	// ר�ҿ��
	CComboBox m_ctrlExpLibItem;

	// ���������
	CComboBox m_ctrlPartScheme;

private: // ����
	// ����Ŀ¼��
	FolderNodePtr m_pSaveFolder;

	// �Ƿ��ܱ༭Ŀ¼��
	BOOL m_bEnableFolder;

	// Ĭ��ר�ҿ��
	LONGLONG m_iExpLibItemID;

	// �Ƿ��ܱ༭ר�ҿ⡣
	BOOL m_bEnableExpLib;

	// �������á�
	LONGLONG m_iParamConfigID;

	// �Ƿ��ܱ༭�������á�
	BOOL m_bEnableConfig;

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
