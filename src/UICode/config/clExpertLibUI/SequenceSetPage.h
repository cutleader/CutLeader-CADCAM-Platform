#pragma once

#include "clExpertLibUICommon.h"
#include "taoresource.h"
#include "CLResizableDialog.h"
#include "NumEdit.h"

DECLARE_CUTLEADER_CLASS(SequenceConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

class ClExpertLibUI_Export SequenceSetPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(SequenceSetPage)

public:
	SequenceSetPage(void);
	~SequenceSetPage(void);

	enum { IDD = IDD_clExpertLibUI_Sequence };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected:
	// Sequence Direction Group.
	afx_msg void OnPartXLine();
	afx_msg void OnPartYLine();
	afx_msg void OnLoopShortest();
	afx_msg void OnLoopXLine();
	afx_msg void OnLoopYLine();

	// Start Corner Group.
	afx_msg void OnLeftTop();
	afx_msg void OnRightTop();
	afx_msg void OnLeftBottom();
	afx_msg void OnRightBottom();

	// for common cutting.
	afx_msg void OnCheckComCut();
	afx_msg void OnSetComCut();

	afx_msg void OnSetBdgeCut();
	afx_msg void OnSetGridCut();
	afx_msg void OnSetFlyCut();

	afx_msg void OnSetCollisionAvoid();

	// ����
	afx_msg void OnCheckChainCut();
	afx_msg void OnSetChainCut();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	void DisplaySeqConfigItem(SequenceConfigItemPtr seqSetting);
	SequenceConfigItemPtr GetSeqConfigItem();

private:
	void LayoutCtrl();

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	int m_nPartDir;
	int m_nLoopDir;
	int m_nCorner;

	BOOL m_bCheckIntf;
    BOOL m_bInnerPartCutRule; // �Ƿ����ڿ׵��и�˳�����и���Ƕ���
	BOOL m_bComCut;
	BOOL m_bChainCut;

	BOOL m_bSoftwareOffset;

private:
	SequenceConfigItemPtr m_pSeqSetting;
};

END_CUTLEADER_NAMESPACE()
