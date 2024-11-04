#pragma once

#include "clDataManagerCommon.h"
#include "taoresource.h"
#include "CLResizableDialog.h"
#include "InvCellGrid.h"

DECLARE_CUTLEADER_CLASS(NestJob)
DECLARE_CUTLEADER_CLASS(ImgData)

BEGIN_CUTLEADER_NAMESPACE()

// this page will display the sheet info.
// notes:
//   1) if we changed some props in "Basic info" group, we can get the changes from "m_pNestJob", so we can save these props outside the dialog.
class ClDataManager_Export NestJobInfoPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(NestJobInfoPage)

public:
	NestJobInfoPage();
	~NestJobInfoPage();

	enum { IDD = IDD_clDataManagerUI_NestJobInfo };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected: // message mapping.
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	afx_msg void OnKillNestJobName();
	afx_msg void OnKillNestJobComment();
	afx_msg void OnMoreNestInfo();
	afx_msg void OnSelchangedJobSheet(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	// display the nest job that will be edited.
	void DisplayNestJobInfo(NestJobPtr pNestJob);

	NestJobPtr GetNestJob() { return m_pNestJob; }

	// update the modify date.
	// notes:
	//   1) get the modify date from NestJob object.
	void UpdateModifyDate();

private:
	// init the column header of the list ctrl.
	void InitGridStyle();

	// init the content of the sheet list.
	void InitSheetGridData();

	// release the data which stored in grid.
	void ReleaseGridItemData();

	// ��ʼ��Ԥ�����ݡ�
	void InitPreviewData(int iRow);

private: // controls
	/************************************************************************/
	// basic info group.

	CString m_strName;
	CString m_strComment;

	CString m_strCreateDate;
	CString m_strModifyDate;
	/************************************************************************/

	// wnd for preview.
	CStatic m_previewWnd;

	// sheet info group.
	InvCellGrid m_sheetGrid;

private: // data member
	// ����Ű���ҵ�еİ��ֻ�л�����Ϣ���Ű�����Ҳû�м��أ�������鿴����ʱ�Ű�������Ϣ������ȥ��
	NestJobPtr m_pNestJob;

	// Ԥ�����ݡ�
	ImgDataPtr m_pImgData;

	BOOL m_bIgnoreSelectionChange_of_grid;
};

END_CUTLEADER_NAMESPACE()
