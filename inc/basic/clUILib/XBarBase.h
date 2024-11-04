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
	// ��������һ�������͵��������������������
	XGridProperty* AddDoublePropItem(CMFCPropertyGridProperty* pGroup, CString strName, double dVal,
									 int iPrecision, BOOL bEnable = TRUE, BOOL bHighlight = FALSE);

	// ��������һ���ַ����͵��������������������
	XGridProperty* AddStringPropItem(CMFCPropertyGridProperty* pGroup, CString strName, CString strVal,
									 BOOL bEnable = TRUE, BOOL bHighlight = FALSE);

	// ��������һ����Ͽ����͵��������������������
	XGridProperty* AddComboPropItem(CMFCPropertyGridProperty* pGroup, CString strName, CString strVal,
									StringListPtr pStrValList, BOOL bHighlight = FALSE);
	// ������/���á���Ͽ�
	XGridProperty* AddComboPropItemA(CMFCPropertyGridProperty* pGroup, CString strName, BOOL bVal, BOOL bHighlight = FALSE);

	// ��������һ����ѡ�����͵��������������������
	XGridProperty* AddCheckPropItem(CMFCPropertyGridProperty* pGroup, CString strName, BOOL bVal,
									DWORD_PTR dwData = 0, BOOL bHighlight = FALSE);

	// ��������һ��ָ�����͵��������������������
	XGridProperty* AddPropItem(CMFCPropertyGridProperty* pGroup, BASIC_DATA_TYPE iDataType, int iPrecision,
							   CString strName, VARIANT var, BOOL bEnable = TRUE, BOOL bHighlight = FALSE);

protected:
	// the main window of the app.
	CWnd* m_pMainWnd;
};

END_CUTLEADER_NAMESPACE()
