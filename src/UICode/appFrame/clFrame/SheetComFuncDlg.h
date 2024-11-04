#pragma once

#include "clFrameCommon.h"
#include "CLResizableDialog.h"
#include "taoResource.h"
#include "SheetView.h"

BEGIN_CUTLEADER_NAMESPACE()

// 板材常用功能界面。
class SheetComFuncDlg : public CLResizableDialog
{
	DECLARE_DYNCREATE(SheetComFuncDlg)

public:
	SheetComFuncDlg();
	~SheetComFuncDlg();

	enum { IDD = IDD_clFrame_SheetCommonFunction };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel() { return; }

protected: // message mapping.
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	// 零件布局功能。
	afx_msg void OnAddPartFromDB();
	afx_msg void OnAddPartFromDxf();
	afx_msg void OnAddPartFromAsmDxf();
	afx_msg void OnSelectPart();
	afx_msg void OnCreateGrid();
	afx_msg void OnDeletePart();
	afx_msg void OnMeasureDis();

	// 编辑工艺。
	afx_msg void OnAssignCam();
	afx_msg void OnResetCam();
	afx_msg void OnCheckIntf();
	afx_msg void OnEditLoopTool();
	afx_msg void OnLoopLead();
	afx_msg void OnStartPt();
	afx_msg void OnCutDir();
	afx_msg void OnCutSide();

	// 编辑工序。
	afx_msg void OnAutoSeq();
	afx_msg void OnClearSeq();
	afx_msg void OnDeleteSeq();
	afx_msg void OnOrderSeq();
	afx_msg void OnAddLoopSeq();
	afx_msg void OnGenerateRemCutLine();
	afx_msg void OnSimulateSeq();
	afx_msg void OnGenerateNC();

	// 设置。
	afx_msg void OnParamConfig();
	afx_msg void OnSystemConfig();
	afx_msg void OnMatLib();

	DECLARE_MESSAGE_MAP()

public:
	void Init(SheetView* pSheetView) { m_pSheetView = pSheetView; }

private: // controls
	// 零件布局功能。
	CMFCButton m_btnPartFromDB;
	CMFCButton m_btnPartFromDxf;
	CMFCButton m_btnPartFromAsmDxf;
	CMFCButton m_btnSelectPart;
	CMFCButton m_btnCreateGrid;
	CMFCButton m_btnDeletePart;
	CMFCButton m_btnMeasureDis;

	// 编辑工艺。
	CMFCButton m_ctrlAssignCAMBtn;
	CMFCButton m_ctrlResetCAMBtn;
	CMFCButton m_ctrlCheckIntfBtn;
	CMFCButton m_ctrlLoopToolBtn;
	CMFCButton m_ctrlLoopLeadBtn;
	CMFCButton m_ctrlStartPtBtn;
	CMFCButton m_ctrlCutDirBtn;
	CMFCButton m_ctrlCutSideBtn;

	// 编辑工序。
	CMFCButton m_btnAutoSeq;
	CMFCButton m_btnClearSeq;
	CMFCButton m_btnSelectSeq;
	CMFCButton m_btnDeleteSeq;
	CMFCButton m_btnOrderSeq;
	CMFCButton m_btnAddLoopSeq;
	CMFCButton m_btnInsertLoopSeq;
	CMFCButton m_btnGenerateRemCutLine;
	CMFCButton m_btnSimulateSeq;
	CMFCButton m_btnGenerateNC;

	// 设置。
	CMFCButton m_ctrlParamBtn;
	CMFCButton m_ctrlSysSetBtn;
	CMFCButton m_ctrlMatLibBtn;

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
	SheetView* m_pSheetView;
};

END_CUTLEADER_NAMESPACE()
