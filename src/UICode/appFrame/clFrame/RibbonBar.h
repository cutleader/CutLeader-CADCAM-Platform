#pragma once

#include "clFrameCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

#define RECENT_DATA_COUNT	10

// ribbon bar。
class CLFRAME_Export RibbonBar
{
public:
	RibbonBar(void);
	~RibbonBar(void);

public:
	// create ribbon bar and add the main icon.
	// notes:
	//   1) this function will init some categories which exist in all modes.
	void Init(CWnd* pParentWnd);

	// update "recent data" menu.
	void UpdateRecentDataMenu();

	// 用提示文字的方式高亮按钮。
	/* 参数：
	*  strCategoryName: category的名称。
	*  iBtnID: 按钮ID。
	*/
	void HighlightBtn(CString strCategoryName, int iBtnID, CString strTips);

private:
	// 创建主菜单。
	void CreateMainCategory();

	// 创建快捷工具条。
	void CreateQuickAccess();

	// 创建ribbon页。
	void CreateQuickStartCategory();
	void CreateGeomeCategory();
	void CreateCutFeatCategory();
	void CreatePartLayoutCategory();
	void CreateSequenceCategory();
	void CreateConfigCategory();
	void CreateHelpCategory();

	// 根据类别名称得到它的序号。
	int GetCategoryIndex(CString strName);

	// 得到当前类别名称。
	CString GetCurCategoryName();

private:
	CMFCRibbonApplicationButton m_mainButton;
	CMFCRibbonBar m_ribbonBar;
};

END_CUTLEADER_NAMESPACE()
