#pragma once

#include "clFrameCommon.h"
#include "taoresource.h"
#include "RibbonBar.h"

BEGIN_CUTLEADER_NAMESPACE()

// 引导模式状态。
typedef enum tagSheetGrideState
{
	SHEET_GUIDE_CREATE_SHEET	= 0,
	SHEET_GUIDE_ADD_DXFDWG		= 1,
	SHEET_GUIDE_AUTO_SEQUENCE	= 2,
	SHEET_GUIDE_VIEW_SEQUENCE	= 3,
	SHEET_GUIDE_GENERATE_NC		= 4,
} SHEET_GUIDE_STATE;

#define WM_SHT_GUIDE_QUIT			WM_USER + 0x1F57

// 指引板材模式下面的工作流程。
class SheetGuideDlg : public CLDialog
{
public:
	SheetGuideDlg(CWnd* pParent = NULL);
	~SheetGuideDlg(void);

	enum { IDD = IDD_clFrame_SheetGuide };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

protected: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPrevious();
	afx_msg void OnNext();
	afx_msg void OnQuitGuide();

	DECLARE_MESSAGE_MAP()

private: // for control.
	CMFCButton m_ctrlPreBtn;
	CMFCButton m_ctrlPostBtn;
	CMFCButton m_ctrlQuitBtn;

private:
	SHEET_GUIDE_STATE m_iSheetGuideState;

	// use this brush to paint the background.
	CBrush m_brush;
};

END_CUTLEADER_NAMESPACE()
