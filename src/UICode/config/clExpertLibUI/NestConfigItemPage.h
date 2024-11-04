#pragma once

#include "clExpertLibUICommon.h"
#include "NumEdit.h"
#include "CLResizableDialog.h"
#include "taoresource.h"
#include "clExpertLibResource.h"

DECLARE_CUTLEADER_CLASS(NestConfigItem)

BEGIN_CUTLEADER_NAMESPACE()

class ClExpertLibUI_Export NestConfigItemPage : public CLResizableDialog
{
	DECLARE_DYNCREATE(NestConfigItemPage)

public:
	NestConfigItemPage(void);
	~NestConfigItemPage(void);

	enum { IDD = IDD_clExpertLibUI_Nest };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual void OnOK() { return; }
	virtual void OnCancel() { return; }

protected:
	afx_msg void OnKillLeft();
	afx_msg void OnKillRight();
	afx_msg void OnKillTop();
	afx_msg void OnKillBottom();
	afx_msg void OnKillPolyMatMargin();

	afx_msg void OnTopLeft();
	afx_msg void OnTopRight();
	afx_msg void OnBottomLeft();
	afx_msg void OnBottomRight();

	afx_msg void OnDirY();
	afx_msg void OnDirX();

	afx_msg void OnCheckComcut();
	afx_msg void OnKillPartDis();
	afx_msg void OnCheckPartInPart();
	afx_msg void OnCheckExcludeCAM();
	afx_msg void OnAdvSetting();

	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	void DisplayNestConfigItem(NestConfigItemPtr pNestConfigItem);
	NestConfigItemPtr GetNestConfigItem();

private:
	// init the layout of the controls.
	void InitLayout();

	// init the button's bitmap.
	void InitButtonBMP();

	// draw the border of the BMP.
	void SelectBitmap(int iBtnID);

private: // controls
	// use this brush to paint the background.
	CBrush m_brush;

	// “矩形材料边距”分组。
	CBitmapButton m_borderSpace;
	CNumEdit m_dToLeft;
	CNumEdit m_dToRight;
	CNumEdit m_dToTop;
	CNumEdit m_dToBottom;

	// “不规则材料边距”分组。
	CBitmapButton m_polyBorderSpace;
	CNumEdit m_dPolyMargin;

	// for "Start Corner" group.
	CBitmapButton m_bmpTopLeft;
	CBitmapButton m_bmpTopRight;
	CBitmapButton m_bmpBottomLeft;
	CBitmapButton m_bmpBottomRight;

	// for "Nest Direction" group.
	CBitmapButton m_bmpYDir;
	CBitmapButton m_bmpXDir;

	// “其他设置”分组。
	CNumEdit m_dPartDis;
	BOOL m_bEnableComcut;
	BOOL m_bPartInPart;
	BOOL m_bExcludeCAM;

private:
	// use this to draw the border of the BMP.
	CDC* m_pDC;
	int m_nTimer;

	NestConfigItemPtr m_pNestConfigItem;
};

END_CUTLEADER_NAMESPACE()
