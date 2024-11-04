#pragma once

#include "clUILibCommon.h"
#include "clBaseDataUICommon.h"
#include "taoresource.h"
#include "CGridListCtrlGroups.h"
#include <vector>
#include "CLDialog.h"

DECLARE_CUTLEADER_CLASS(HelpVideoCategory)

BEGIN_CUTLEADER_NAMESPACE()

#define WM_DATA_AUTO_POPUP_VIDEO	WM_USER + 0x05C1

// this dialog will display the help video.
class ClBaseDataUI_Export HelpVideoDlg : public CLDialog
{
public:
	HelpVideoDlg(LANG_TYPE iLang, BOOL bAutoShow);

	enum { IDD = IDD_clBaseDataUI_HelpVideo };

protected: // inherit from the super class.
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	virtual void OnCancel();

protected: // message mapping.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSelchangedCategory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkVideo(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnCheckAtStart();

	afx_msg void OnPlay();
	afx_msg void OnMinimize();

	DECLARE_MESSAGE_MAP()

private:
	// display video info.
	void DisplayVideoInfo(HelpVideoCategoryPtr pHelpVideoCategory);

	void InitGridStyle();

	// play the video.
	void PlayVideo(CString strVideoFileName);

private: // for control
	CListCtrl m_categoryList;
	CImageList m_imageList;

	CString m_strMsg;
	CGridListCtrlGroups m_videoList;

	BOOL m_bAutoShow;

	// use this brush to paint the background.
	CBrush m_brush;

private: // for data.
	LANG_TYPE m_iLang;

	// all video info.
	std::vector<HelpVideoCategoryPtr> m_videoCategoryList;
};

END_CUTLEADER_NAMESPACE()
