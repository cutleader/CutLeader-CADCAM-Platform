#pragma once

#include "util.h"
#include "nestTypedecl.h"
#include "NumEdit.h"
#include "taoresource.h"

BEGIN_NAMESPACE_3(CL, Nest, UI)

// ���������Ű�������
class NestCountDlg : public CDialogEx
{
	DECLARE_DYNAMIC(NestCountDlg)

public:
	NestCountDlg(CWnd* pParent = NULL);
	virtual ~NestCountDlg(void);

	enum { IDD = IDD_NESTMGR_NEST_COUNT };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()

public:
	int GetNestCount() { return m_iNestCount; }

private: // controls.
	// �Ű�������
	CNumEdit m_ctrlNestNum;

private: // data
	int m_iNestCount;
};

END_NAMESPACE_3()
