#pragma once

#include "clBaseDataUICommon.h"
#include "taoresource.h"
#include "XHTMLStatic.h"
#include "XHyperLink.h"
#include "CLDialog.h"

BEGIN_CUTLEADER_NAMESPACE()

// ���ڶԻ���
class ClBaseDataUI_Export AboutDlg : public CLDialog
{
public:
	AboutDlg();

	enum { IDD = IDD_clBaseDataUI_About };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

private:
	CBitmapButton m_btnProductName;

	/************************************************************************/
	// ��Ȩ��Ϣ��

	// ��ƷID��
	CString m_strProductID;

	// �ͻ�ID���롣
	CString m_strCustomerID;

	// ��Ч�ڡ�
	CString m_strStartDate;
	CString m_strEndDate;
	/************************************************************************/

	// app version.
	XHTMLStatic	m_appVersion;

	// �����ݿ�汾��
	XHTMLStatic	m_dbVersion;

	// ����汾��
	XHTMLStatic	m_partVersion;

	// ��İ汾��
	XHTMLStatic	m_shtVersion;

	// driver version.
	XHTMLStatic	m_driVersion;

	// ר�ҿ�汾��
	XHTMLStatic	m_expLibVersion;

	// ���Ͽ�汾��
	XHTMLStatic	m_matLibVersion;

	// �����汾��
	XHTMLStatic	m_rptLibVersion;

	// the web site link.
	XHyperLink m_websiteLink;

	// use this brush to paint the background.
	CBrush m_brush;
};

END_CUTLEADER_NAMESPACE()
