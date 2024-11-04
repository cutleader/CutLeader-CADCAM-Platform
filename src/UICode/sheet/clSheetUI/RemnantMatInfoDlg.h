#pragma once

#include "clSheetUICommon.h"
#include "taoresource.h"
#include "NumEdit.h"
#include "LineArc2DStaticControl.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(Sheet)

BEGIN_CUTLEADER_NAMESPACE()

class ClSheetUI_Export RemnantMatInfoDlg : public CLDialog
{
	DECLARE_DYNAMIC(RemnantMatInfoDlg)

public:
	RemnantMatInfoDlg(CWnd* pParent = NULL);
	~RemnantMatInfoDlg(void);

	enum { IDD = IDD_clSheetUI_RemnantMaterial };

protected: // inherit from the base class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short iDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnCheckMerge();
	afx_msg void OnMerge();
	afx_msg void OnSaveRemnant();

	DECLARE_MESSAGE_MAP()

public:
	void Init(SheetPtr pSheet);

private:
	void PreviewMat();

private: // for controls.
	CString m_strMsg;
	LineArc2DStaticControl m_ctrlMatPreview;
	CNumEdit m_ctrlRegionCount;

	BOOL m_bEnableMerge;
	CNumEdit m_ctrlMergeDis;

	CBrush m_brush;

private: // for data.
	SheetPtr m_pSheet;

	// ��ĵı߿�
	LineArc2DListPtr m_pOuterLoop;

	// ��ĵ�δ�ϲ�����Ч����
	std::vector<LineArc2DListPtr> m_uselessRegion;

	// ��ĵĺϲ������Ч����
	std::vector<LineArc2DListPtr> m_mergeUselessRegion;
};

END_CUTLEADER_NAMESPACE()
