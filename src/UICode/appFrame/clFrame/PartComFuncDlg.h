#pragma once

#include "clFrameCommon.h"
#include "CLResizableDialog.h"
#include "taoResource.h"
#include "PartView.h"

BEGIN_CUTLEADER_NAMESPACE()

// 零件常用功能界面。
class PartComFuncDlg : public CLResizableDialog
{
	DECLARE_DYNCREATE(PartComFuncDlg)

public:
	PartComFuncDlg();
	~PartComFuncDlg();

	enum { IDD = IDD_clFrame_PartCommonFunction };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel() { return; }

protected: // message mapping.
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// 画图功能。
	afx_msg void OnDrawLine();
	afx_msg void OnDrawPolyLine();
	afx_msg void OnDrawRect();
	afx_msg void OnDrawArc();
	afx_msg void OnDrawCircle();

	// 图形编辑功能。
	afx_msg void OnSelGeom();
	afx_msg void OnDelGeom();

	// 编辑工艺。
	afx_msg void OnAssignCam();
	afx_msg void OnResetCam();
	afx_msg void OnCheckIntf();
	afx_msg void OnEditLoopTool();
	afx_msg void OnLoopLead();
	afx_msg void OnStartPt();
	afx_msg void OnCutDir();
	afx_msg void OnCutSide();

	// 设置。
	afx_msg void OnParamConfig();
	afx_msg void OnSystemConfig();

	DECLARE_MESSAGE_MAP()

public:
	void Init(PartView* pPartView) { m_pPartView = pPartView; }

private: // controls
	// 画图功能。
	CMFCButton m_ctrlDrawLineBtn;
	CMFCButton m_ctrlDrawPolyLineBtn;
	CMFCButton m_ctrlDrawRectBtn;
	CMFCButton m_ctrlDrawArcBtn;
	CMFCButton m_ctrlDrawCircleBtn;

	// 图形编辑功能。
	CMFCButton m_ctrlSelGeomBtn;
	CMFCButton m_ctrlDelGeomBtn;

	// 编辑工艺。
	CMFCButton m_ctrlAssignCAMBtn;
	CMFCButton m_ctrlResetCAMBtn;
	CMFCButton m_ctrlCheckIntfBtn;
	CMFCButton m_ctrlLoopToolBtn;
	CMFCButton m_ctrlLoopLeadBtn;
	CMFCButton m_ctrlStartPtBtn;
	CMFCButton m_ctrlCutDirBtn;
	CMFCButton m_ctrlCutSideBtn;

	// 设置。
	CMFCButton m_ctrlParamBtn;
	CMFCButton m_ctrlSysSetBtn;

	// use this brush to paint the background.
	CBrush m_brush;

	/************************************************************************/
	// about scroll bar.

	// dialog size as you see in the resource view (original size)
	CRect m_rcOriginalRect;

	// actual scroll position
	int m_nScrollPos;

	// actual dialog height
	int m_nCurHeight;
	/************************************************************************/

private: // data member.
	PartView* m_pPartView;
};

END_CUTLEADER_NAMESPACE()
