#pragma once

#include "clDataImportExportUICommon.h"
#include "clDataImportExportCommon.h"
#include "LineArc2DStaticControl.h"


DECLARE_CUTLEADER_CLASS(DxfData)


BEGIN_CUTLEADER_NAMESPACE()

// ����Ի�������ѡ�������ݡ�
// ע��
//   1) ����Ի��������������ʽ��dxf/dwg, ���뱸�����ݣ�����ר�ҿ⡣
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

    // �����ߵ���dxf/dwgʱ����Ҫ������������õ�dxf data
    DxfDataPtr GetDxfData() const { return m_pDxfData; }

private: // for control
	// use this window to show the thumbnail
	LineArc2DStaticControl m_previewStatic;

private:
	// whether need the preview control.
	BOOL m_bNeedPreview;

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
