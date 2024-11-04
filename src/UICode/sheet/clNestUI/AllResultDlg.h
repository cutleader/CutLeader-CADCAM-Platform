#pragma once

#include "clNestUICommon.h"
#include "taoResource.h"

BEGIN_CUTLEADER_NAMESPACE()

class AllResultDlg : public CLDialog
{
	DECLARE_DYNCREATE(AllResultDlg)

public:
	AllResultDlg();
	~AllResultDlg(void);

	enum { IDD = IDD_clNestUI_AllNestResults };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void Init(unsigned int iNestResultCount) { m_iNestResultCount = iNestResultCount; }
	unsigned int GetSelectedResultIndex() const { return m_iSelectedResultIndex; }

private: // controls.
	CListBox m_resultList;

	CBrush m_brush;

private: // data.
	unsigned int m_iNestResultCount;
	unsigned int m_iSelectedResultIndex;
};

END_CUTLEADER_NAMESPACE()