#pragma once

#include "resource.h"
#include "SplashThread.h"
#include "cutLeaderCommon.h"

DECLARE_CUTLEADER_CLASS(SplashThread)

class ClApp : public CWinAppEx
{
public:
	ClApp();

protected: // inherit from the super class.
	virtual BOOL InitInstance();
	virtual int ExitInstance();

public: // message mapping.
	afx_msg void OnDataMgr();
	afx_msg void OnNewPart();
	afx_msg void OnOpenDxf();
	afx_msg void OnNewSheet();
	afx_msg void OnNewNestJob();
	afx_msg void OnImport();
	afx_msg void OnSplitImport();

	afx_msg void OnOpenDataItem(WPARAM wParam, LPARAM lParam);
	afx_msg void OnOpenPartItem(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAutoPopupVideo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGuideMode1(WPARAM wParam, LPARAM lParam);
	afx_msg void OnGuideMode2(WPARAM wParam, LPARAM lParam);

	afx_msg BOOL OnOpenRecentData(UINT nID);
	afx_msg void OnRecentData();

	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();

	// Configuration Category.
	afx_msg void OnCLOption();
	afx_msg void OnMacLib();
	afx_msg void OnMatLib();
	afx_msg void OnExpertLib();
	afx_msg void OnDflReportTpl();
	afx_msg void OnDesignReportTpl();

	// "Help" Category.
	afx_msg void OnHelpVideo();
	afx_msg void OnHelpTopics();
	afx_msg void OnWebSite();
	afx_msg void OnAbout();

	DECLARE_MESSAGE_MAP()

private:
	// do some initializations when the application starts.
	void Init();

	// do some un-initializations when the application ends.
	void UnInit();

	// about displaying splash.
	void BeginSplash();
	void EndSplash();

private:
	CMultiDocTemplate* m_pPartDocTemplate;
	CMultiDocTemplate* m_pSheetDocTemplate;
	CMultiDocTemplate* m_pStartPageDocTpl;

	SplashThread* m_pSplashThread;

	// whether this app will not start.
	BOOL m_bNotLaunch;
};

extern _declspec (dllexport) ClApp cLeaderApp;
