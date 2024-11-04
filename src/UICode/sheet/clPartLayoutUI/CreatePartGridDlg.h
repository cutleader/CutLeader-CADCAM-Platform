#pragma once

#include "clPartLayoutUICommon.h"
#include "NumEdit.h"
#include "taoresource.h"

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(PartPlacement)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(PartInstanceList)

BEGIN_CUTLEADER_NAMESPACE()

// ����Ի�����������������С�
class ClPartLayoutUI_Export CreatePartGridDlg : public CLDialog
{
	DECLARE_DYNCREATE(CreatePartGridDlg)

public:
	CreatePartGridDlg(CWnd* pParent = NULL);
	~CreatePartGridDlg(void);

	enum { IDD = IDD_clPartLayoutUI_CreatePartGrid };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnCheckComcut();
	afx_msg void OnPreview();
	afx_msg void OnOK();
	afx_msg void OnCancel();

	DECLARE_MESSAGE_MAP()

public: // get/set functions.
	BOOL IsCanceled() const { return m_bCanceled; }
	BOOL IsCreateGrid() const { return m_bCreateGrid; }

public:
	// ��ʾ�Ի���
	// ע��
	//  1) �ڶ����а������ʱ���������������ʾ�Ի���
	void ShowPartGridDlg(GlViewPortPtr pViewPort, PartPlacementListPtr pPartPlacements, PartInstanceListPtr pPartInstances, PartPlacementPtr pBasePartPmt, PartPlacementListPtr pGridPartPmts);

private:
	// ����������С�
	// ע��
	//  1) ���������Ҫ�����������ݡ�
	PartPlacementPtr CreatePartGrid();

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	// �����ࡣ
	CNumEdit m_ctrlPartDis;
	BOOL m_bComCut;

	// ��������
	CNumEdit m_ctrlRowNum;
	CNumEdit m_ctrlColumnNum;

	// ���з���
	CComboBox m_ctrlGridDir;

	CMFCButton m_ctrlPreview;
	CMFCButton m_ctrlOK;
	CMFCButton m_ctrlCancel;

private: // data
	GlViewPortPtr m_pViewPort;

	// ����������ú����ʵ����
	PartPlacementListPtr m_pPartPmts;
	PartInstanceListPtr m_pPartInsts;

	// ��׼�����
	PartPlacementPtr m_pBasePartPmt;

	// ���������С�
	// ע:
	//  1) ��Ȼ�Ǽ��϶����������ֻ��һ��Ԫ�ء�
	PartPlacementListPtr m_pGridPartPmts;

	// �Ƿ�ȡ���ˡ�
	// ע��
	//  1) ��������Ҫ������ֵ���˳�������
	BOOL m_bCanceled;

	// �Ƿ�ȷ���������С�
	// ע��
	//  1) ��������Ҫ������ֵ��Ȼ�󴴽�������˳�������
	BOOL m_bCreateGrid;
};

END_CUTLEADER_NAMESPACE()
