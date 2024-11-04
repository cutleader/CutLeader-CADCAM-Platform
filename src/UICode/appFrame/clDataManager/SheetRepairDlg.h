#pragma once

#include "clDataManagerCommon.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

// ����޸��Ի���
// ע��
//  1) �����ĵġ��������á���ʧ����Ҫ�޸�����ʵ��ʱ����и���������յĲ�������Ҳ��ʧ�ˡ�
//  2) �����ĵĲ��϶�ʧ����Ҫ�޸���
class ClDataManager_Export SheetRepairDlg : public CLDialog
{
	DECLARE_DYNCREATE(SheetRepairDlg)

public:
	SheetRepairDlg();
	~SheetRepairDlg(void);

	enum { IDD = IDD_clDataManagerUI_SheetRepair };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnRepairScheme();
	afx_msg void OnRepairMat();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	void SetSheet(LONGLONG iSheetID) { m_iSheetID = iSheetID; }

private:
	// init the material group.
	void InitMatData();

	// init the scheme data.
	void InitSchemeData();

private: // controls
	CString m_strMsg;

	CString m_strExpLibItem;
	CString m_strConfig;

	// for "material info" group.
	CString m_strMat;
	CString m_strMatThick;
	CString m_strMatSize;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data member.
	LONGLONG m_iSheetID;
};

END_CUTLEADER_NAMESPACE()
