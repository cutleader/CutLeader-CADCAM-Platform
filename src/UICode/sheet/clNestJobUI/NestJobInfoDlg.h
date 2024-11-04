#pragma once

#include "clNestJobUICommon.h"
#include "DataProperty.h"
#include "taoresource.h"
#include "CGridListCtrlGroups.h"
#include "InvCellGrid.h"

DECLARE_CUTLEADER_CLASS(NestJob)

BEGIN_CUTLEADER_NAMESPACE()

// 显示排版作业详细信息
class ClNestJobUI_Export NestJobInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(NestJobInfoDlg)

public:
	NestJobInfoDlg(CWnd* pParent = NULL);
	virtual ~NestJobInfoDlg(void);

	enum { IDD = IDD_clNestJobUI_ViewNestTask };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnBnClickedOk();

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	void SetNestJob(NestJobPtr pNestJob) { m_pNestJob = pNestJob; }

private:
	// init the style of the list ctrl
	void InitListStyle();

	// init the data
	void InitListData();

private: // for controls
	// for scheme group.
	CString m_strExpLibName;
	CString m_strParamConfig;

	InvCellGrid m_nestPartList;
	InvCellGrid m_nestMatList;

	CString m_strUtilization;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data member
	// 这个排版作业中的板材只有基本信息，其他数据都加载了。
	NestJobPtr m_pNestJob;
};

END_CUTLEADER_NAMESPACE()
