#pragma once

#include "clUtilityCommon.h"
#include "clUILibCommon.h"
#include "XGridProperty.h"
#include <vector>

BEGIN_CUTLEADER_NAMESPACE()

typedef boost::shared_ptr<std::vector<CString>> StringListPtr;

// the basic class for the bars.
class ClUILib_Export XBarBase : public CDockablePane
{
public:
	XBarBase(void);
	virtual ~XBarBase(void);

public:
	// get the relative view of this prop bar.
	// notes:
	//   1) actually, the relative view is the current active view.
	CWnd* GetRelativeView() const;

	// only display or hide bar.
	void ShowBar(BOOL bShow);

protected:
	// 向分组添加一个浮点型的属性项，并返回这个属性项。
	XGridProperty* AddDoublePropItem(CMFCPropertyGridProperty* pGroup, CString strName, double dVal,
									 int iPrecision, BOOL bEnable = TRUE, BOOL bHighlight = FALSE);

	// 向分组添加一个字符串型的属性项，并返回这个属性项。
	XGridProperty* AddStringPropItem(CMFCPropertyGridProperty* pGroup, CString strName, CString strVal,
									 BOOL bEnable = TRUE, BOOL bHighlight = FALSE);

	// 向分组添加一个组合框类型的属性项，并返回这个属性项。
	XGridProperty* AddComboPropItem(CMFCPropertyGridProperty* pGroup, CString strName, CString strVal,
									StringListPtr pStrValList, BOOL bHighlight = FALSE);
	// “启用/禁用”组合框。
	XGridProperty* AddComboPropItemA(CMFCPropertyGridProperty* pGroup, CString strName, BOOL bVal, BOOL bHighlight = FALSE);

	// 向分组添加一个单选框类型的属性项，并返回这个属性项。
	XGridProperty* AddCheckPropItem(CMFCPropertyGridProperty* pGroup, CString strName, BOOL bVal,
									DWORD_PTR dwData = 0, BOOL bHighlight = FALSE);

	// 向分组添加一个指定类型的属性项，并返回这个属性项。
	XGridProperty* AddPropItem(CMFCPropertyGridProperty* pGroup, BASIC_DATA_TYPE iDataType, int iPrecision,
							   CString strName, VARIANT var, BOOL bEnable = TRUE, BOOL bHighlight = FALSE);

protected:
	// the main window of the app.
	CWnd* m_pMainWnd;
};

END_CUTLEADER_NAMESPACE()
