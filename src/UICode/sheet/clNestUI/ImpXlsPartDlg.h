#pragma once

#include "clNestUICommon.h"
#include "NumEdit.h"
#include "CGridListCtrlGroups.h"
#include "taoresource.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(ExtPropValList)
DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(NestPartList)

BEGIN_CUTLEADER_NAMESPACE()

// ��XLS�ļ����������
class ImpXlsPartDlg : public CLDialog
{
	DECLARE_DYNAMIC(ImpXlsPartDlg)

public:
	ImpXlsPartDlg(CWnd* pParent = NULL);
	virtual ~ImpXlsPartDlg(void);

	enum { IDD = IDD_clNestUI_ImportXLSPart };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelectXlsFile();
	afx_msg void OnSelectFolder();
	afx_msg void OnDeleteRow();
	afx_msg void OnCheckFromFile();
	afx_msg void OnMgrExtProp();
	afx_msg void OnOK();

	afx_msg LRESULT GetCellEditorData(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	void SetConfigParamID(LONGLONG iParamConfigID) { m_iParamConfigID = iParamConfigID; }

	NestPartListPtr GetNestParts() { return m_pNestPartList; }

private:
	// ��excel�ļ���������ʾ���б��С�
	void InitXlsDataList(CString strXlsPath);

	// �õ��б�������������һ�в�Ҫ��
	std::vector<CString> GetColumnNames();

	// �õ������������չ���ԡ�
	// ע��
	//  1) �����б��������˳��
	std::vector<ExtPropValListPtr> GetPartExtProps(int iStartRow);

private: // controls.
	CString m_editXLSPath;
	CNumEdit m_editStartRow;
	CString m_editSavePath;
	CGridListCtrlGroups m_listXlsData;
	CComboBox m_ctrlRotateAng;

	// ����ӳ�䡣
	CComboBox m_comboPartName;
	CComboBox m_comboPartWidth;
	CComboBox m_comboPartHeight;
	CComboBox m_comboPartCount;
	CComboBox m_comboPartPath;
	BOOL m_bLoadPartFromPath;

	// ��չ���ԡ�
	CGridListCtrlGroups m_listExtProp;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data
	// �������á�
	LONGLONG m_iParamConfigID;

	// ����Ŀ¼��
	FolderNodePtr m_pSaveFolder;

	// ��������������ϡ�
	NestPartListPtr m_pNestPartList;
};

END_CUTLEADER_NAMESPACE()
