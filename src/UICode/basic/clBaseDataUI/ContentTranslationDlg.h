#pragma once

#include "clBaseDataUICommon.h"
#include "taoresource.h"
#include "CGridListCtrlGroups.h"
#include "CLDialog.h"


BEGIN_CUTLEADER_NAMESPACE()


class ClBaseDataUI_Export ContentTranslationDlg : public CLDialog
{
	DECLARE_DYNCREATE(ContentTranslationDlg)

	// ÿ�й�������Ϣ
	class GridData
	{
	public:
		GridData(CString strKey)
		{
			m_iStringType = 1;
			m_strKey = strKey;
		}

		GridData(int iStringID)
		{
			m_iStringType = 2;
			m_iStringID = iStringID;
		}

		int m_iStringType; // 1:��һ���ַ�����2:�ڶ����ַ���
		CString m_strKey; // ��һ���ַ����õ�key
		int m_iStringID; // �ڶ����ַ����õ�ID
	};

public:
	ContentTranslationDlg(CWnd* pParent = NULL);
	~ContentTranslationDlg();

	enum { IDD = IDD_clBaseDataUI_Translate };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnDestroy();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnClickOk();
	afx_msg void OnClickCancel();

    afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnGrid_beginLabelEdit_1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGrid_endLabelEdit_1(NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg void OnGrid_beginLabelEdit_2(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnGrid_endLabelEdit_2(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

public:
	void Init(LANG_TYPE iCurrentLanguage) { m_iCurrentLanguage = iCurrentLanguage; }

private: // for controls
    CTabCtrl m_tabCtrl;
    CString m_strMsg_1;
    CString m_strMsg_2;
	CGridListCtrlGroups m_translationList_1;
    CGridListCtrlGroups m_translationList_2;

    // use this brush to paint the background.
    CBrush m_brush;

private:
	LANG_TYPE m_iCurrentLanguage;

	// �ڱ༭���֮ǰ��¼�µ�Ԫ������ݣ����ڱ༭��֮��Ƚϡ�
	CString m_strCellText_beforeEdit;

	// �޸Ĺ��ĵ�һ��͵ڶ����ַ���
	std::map<CString, CString> m_translationData_1_modified;
	std::map<int, CString> m_translationData_2_modified;
};

END_CUTLEADER_NAMESPACE()
