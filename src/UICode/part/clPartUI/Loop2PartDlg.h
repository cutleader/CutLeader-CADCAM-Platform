#pragma once

#include "clPartUICommon.h"
#include "CGridListCtrlGroups.h"
#include "taoresource.h"
#include "LineArc2DStaticControl.h"
#include "CLDialog.h"

DECLARE_CUTLEADER_CLASS(PatternLoopList)
DECLARE_CUTLEADER_CLASS(LoopTopologyItemList)

BEGIN_CUTLEADER_NAMESPACE()

// ����Ի������ڴӻ�·ʶ�������
// ע��
//   1) ��ʱ��Щ��·û��һ�������Χ��·��
//   2) ����������˽ṹ��
class ClPartUI_Export Loop2PartDlg : public CLDialog
{
	DECLARE_DYNCREATE(Loop2PartDlg)

public:
	Loop2PartDlg();
	~Loop2PartDlg(void);

	enum { IDD = IDD_clPartUI_Loop2Part };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

protected:
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectAll();
	afx_msg void OnRecognizeSamePart();

	afx_msg void OnAccept();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	// ��ʼ����·���ϡ�
	void Init(LoopTopologyItemListPtr pLoopTopItemList, CString strName, BOOL bDisplay_RecognizeSamePartBtn);

	// �õ�ʶ�����������ϡ�
	std::vector<std::pair<CString, PatternLoopListPtr>> GetRecognizedParts(BOOL& bRecognizeSamePart)
	{
		bRecognizeSamePart = m_bRecognizeSamePart;
		return m_selPartList;
	}

private:
	// ��ʼ�����
	void InitGrid();

private: // controls
	CString m_strMsg_1;
	CString m_strMsg_2;

	// ʶ���������б�
	CGridListCtrlGroups m_partGrid;

	// �Ƿ�ȫѡ��
	BOOL m_bSelectAll;

	// �Ƿ�ʶ����ͬ�����
	BOOL m_bRecognizeSamePart;

	// Ԥ�����ڡ�
	LineArc2DStaticControl m_previewWnd;

	// use this brush to paint the background.
	CBrush m_brush;

	BOOL m_bDisplay_RecognizeSamePartBtn;

private: // ���ݡ�
	// ���л�·�����˽ṹ��Ϣ�� 
	LoopTopologyItemListPtr m_pLoopTopItemList;

	CString m_strName;

	// ʶ�����������ϡ�
	std::vector<std::pair<CString, PatternLoopListPtr>> m_partList;

	// �û���Ҫ�������
	std::vector<std::pair<CString, PatternLoopListPtr>> m_selPartList;

	// ��ʼ����ʱ���ֹһЩ��Ϣ��
	BOOL m_bEnableEvent;
};

END_CUTLEADER_NAMESPACE()
