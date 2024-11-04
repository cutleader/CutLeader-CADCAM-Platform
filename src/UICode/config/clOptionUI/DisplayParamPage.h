#pragma once

#include "clOptionUICommon.h"
#include "taoresource.h"
#include "ColorComboBox.h"
#include "CLResizableDialog.h"

DECLARE_CUTLEADER_CLASS(DisplayParam)

BEGIN_CUTLEADER_NAMESPACE()

class DisplayParamPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(DisplayParamPage)

public:
	DisplayParamPage();
	~DisplayParamPage();

	enum { IDD = IDD_clOptionUI_Display };

protected: // inherit the interface
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnBnClickedPickcolor();
	afx_msg void OnSelchangeColorItem();
	afx_msg void OnPartNoFont();

	DECLARE_MESSAGE_MAP()

public:
	void DisplayDisplayParam(DisplayParamPtr colorStyleInfoPtr);
	DisplayParamPtr GetDisplayParam();

private: // for control
	CString m_strMsg;

	// ÑÕÉ«
	ColorComboBox m_itemColorList;
	CMFCColorButton	m_ctrlColorPicker;

	// ÏÔÊ¾/Òþ²Ø
	BOOL m_bShowStartCut;
	BOOL m_bShowEndCut;
	BOOL m_bShowLead;
	BOOL m_bShowMicroJoint;
	BOOL m_bShowCorner;
	BOOL m_bShowOT;
	BOOL m_bFlagOpenPath;
	BOOL m_bShowIncorrectTool;

	CSliderCtrl m_speedSlider;

	// use this brush to paint the background.
	CBrush m_brush;

private: // for data
	DisplayParamPtr m_pDisplayParam;

	LOGFONT m_logicFont;
	COLORREF m_fontColor;
};

END_CUTLEADER_NAMESPACE()
