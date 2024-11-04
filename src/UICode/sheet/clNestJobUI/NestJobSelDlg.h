#pragma once

#include "clNestJobUICommon.h"
#include "taoresource.h"
#include "NumEdit.h"
#include "CGridListCtrlGroups.h"

DECLARE_CUTLEADER_CLASS(ImgData)
DECLARE_CUTLEADER_CLASS(NestJob)

BEGIN_CUTLEADER_NAMESPACE()

// �Ű���ҵѡ��Ի���
class ClNestJobUI_Export NestJobSelDlg : public CLDialog
{
	DECLARE_DYNAMIC(NestJobSelDlg)

public:
	NestJobSelDlg(CWnd* pParent = NULL);
	~NestJobSelDlg(void);

	enum { IDD = IDD_clNestJobUI_NestJobSelect };

protected: // inherit from the base class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelchangedNestJob();
	afx_msg void OnSelchangedSheet(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnOk();

	DECLARE_MESSAGE_MAP()

public:
	NestJobPtr GetSelectNestJob() { return m_pNestJob; }

private:
	void InitListStyle();

	// ��ʾ�Ű���ҵ����Ϣ��
	void DisplayNestJob();

	// ��ʼ��Ԥ�����ݡ�
	void InitPreviewData(int iRow);

private: // for controls.
	CListBox m_listNestJob;

	// for "Usage of Material" group.
	CNumEdit m_ctrlUsedMatNum;
	CString m_strUtilization;
	CGridListCtrlGroups m_matGrid;

	// for "Elapsed Time(s)" group.
	CNumEdit m_ctrlNestTime;

	// for "Nested Part Statistics" group.
	CNumEdit m_ctrlSubmitPartNum;
	CNumEdit m_ctrlNestedPartNum;
	CGridListCtrlGroups m_nestPartGrid;

	// for "Nest Result files" group.
	CGridListCtrlGroups m_sheetGrid;

	// wnd for preview.
	CStatic m_previewWnd;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data.
	// ��ǰѡ�е��Ű���ҵ��
	// ע��
	//  1) ����Ҫ�������ء��Ű���ҵ����
	NestJobPtr m_pNestJob;

	// Ԥ�����ݡ�
	ImgDataPtr m_pImgData;

	BOOL m_bDlgInited;
};

END_CUTLEADER_NAMESPACE()
