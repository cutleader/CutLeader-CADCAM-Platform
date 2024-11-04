#pragma once

#include "clFrameCommon.h"
#include "ResizableFormView.h"
#include "taoresource.h"

BEGIN_CUTLEADER_NAMESPACE()

#define WM_GUIDE_MODE_1		WM_USER + 0x4D08
#define WM_GUIDE_MODE_2		WM_USER + 0x18A8
class CLFRAME_Export StartPageView : public CResizableFormView
{
	DECLARE_DYNCREATE(StartPageView)

protected:
	StartPageView();
	virtual ~StartPageView();

	enum{ IDD = IDD_clFrame_StartPage };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnInitialUpdate();

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnGuideMode1();
	afx_msg void OnGuideMode2();

	DECLARE_MESSAGE_MAP()

private: // for controls
	CMFCButton m_ctrlMode1Btn;
	CMFCButton m_ctrlMode2Btn;

	CFont m_buttonFont;

private: // for data member.
	// use this brush to paint the background.
	CBrush m_brush;
};

END_CUTLEADER_NAMESPACE()
