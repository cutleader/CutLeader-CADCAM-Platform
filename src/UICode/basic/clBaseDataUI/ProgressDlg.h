#pragma once

#include "clBaseDataUICommon.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(TaskEx)

BEGIN_CUTLEADER_NAMESPACE()

// ��ʾ������ȵĶԻ���
// ע:
//   1) ����Ի�������������
//   2) task should notify the dialog to update the progress.
class ClBaseDataUI_Export ProgressDlg : public CDialogEx
{
	DECLARE_DYNCREATE(ProgressDlg)

public:
	ProgressDlg();
	~ProgressDlg(void);

	enum { IDD = IDD_clBaseDataUI_Progress };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg LRESULT OnUpdateProgress(WPARAM wParam, LPARAM lParam);
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	void Init(CString strTitle, TaskExPtr pTask);

private:
	CProgressCtrl m_ctrlTaskProgress;
	CEdit m_ctrlSubTask;

private: // for data
	CString m_strTitle;

	// the task object.
	TaskExPtr m_pTask;
};

END_CUTLEADER_NAMESPACE()
