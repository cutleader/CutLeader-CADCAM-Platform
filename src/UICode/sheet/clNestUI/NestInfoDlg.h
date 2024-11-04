#pragma once

#include "clNestJobCommon.h"
#include "clNestUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// ���������Ű���Ϣ��
class NestInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(NestInfoDlg)

public:
	NestInfoDlg(CWnd* pParent = NULL);
	virtual ~NestInfoDlg(void);

	enum { IDD = IDD_clNestUI_NestPartInfo };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckNestCount();
	afx_msg void OnCheckRotAng();
	afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	BOOL IsEnablePartCount() { return m_bEnableNestCount; }
	int GetNestCount() { return m_iNestCount; }

	BOOL IsEnableRotAng() { return m_bEnableRotAng; }
	PART_ROT_STYLE GetRotAng() { return m_iRotAng; }

private: // controls.
	BOOL m_bEnableNestCount;
	CNumEdit m_ctrlNestNum;
	BOOL m_bEnableRotAng;
	CComboBox m_ctrlRotateAng;

	CBrush m_brush;

private: // data
	int m_iNestCount;
	PART_ROT_STYLE m_iRotAng;
};

END_CUTLEADER_NAMESPACE()
