#pragma once

#include "clNestUICommon.h"
#include "taoresource.h"
#include "CGridListCtrlGroups.h"
#include "LineArc2DStaticControl.h"

DECLARE_CUTLEADER_CLASS(MaterialSizeList)
DECLARE_CUTLEADER_CLASS(IMaterialSize)
DECLARE_CUTLEADER_CLASS(MaterialThickness)
DECLARE_CUTLEADER_CLASS(MaterialList)

BEGIN_CUTLEADER_NAMESPACE()

// ѡ����϶Ի���
class ClNestUI_Export NestMatSelectDlg : public CLDialog
{
	DECLARE_DYNAMIC(NestMatSelectDlg)

public:
	NestMatSelectDlg(CWnd* pParent = NULL);
	virtual ~NestMatSelectDlg();

	enum { IDD = IDD_clNestUI_MaterialSelect };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected: // message mapping.
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelchangeMatType();
	afx_msg void OnSelchangeMatName();

	afx_msg void OnMatListchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckHideSkele();

	afx_msg void OnNewMatSize();
	afx_msg void OnImpMatSize();
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

public:
	MaterialSizeListPtr GetSelectedMaterialSizes() const { return m_pMatSizeList; }

private:
	// about release the item data.
	void RlsMatTypeItemData();
	void RlsMatNameItemData();
	void RlsMatSizeItemData();

	// init the style of the list ctrl
	void InitListStyle();

	// init the data.
	void InitData();

	// init the matSize list by the material name object.
	void InitMatSizeList(MaterialThicknessPtr pMatThick, BOOL bCheck);

	// preview the selected matsize.
	void PreviewMatSize();

	// ���Ӳ��ϳߴ絽�б�
	void AddMatSizeToList(IMaterialSizePtr pMaterialSize, BOOL bCheck);

private: // for controls
	CComboBox m_mat;
	CComboBox m_matThick;

	CGridListCtrlGroups m_matList;
	BOOL m_bHideSkele;

	// Ԥ�����ڡ�
	LineArc2DStaticControl m_matPreviewWnd;

	// use this brush to paint the background.
	CBrush m_brush;

private: // data member
	// ѡ�еĲ��ϡ�
	MaterialSizeListPtr m_pMatSizeList;

	// all material in db.
	MaterialListPtr m_pMatList;

	// �ؼ��Ƿ��ʼ����ɡ�
	BOOL m_bListInited;
};

END_CUTLEADER_NAMESPACE()
