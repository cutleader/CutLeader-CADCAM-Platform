#pragma once

#include "clFrameCommon.h"

BEGIN_CUTLEADER_NAMESPACE()

#define RECENT_DATA_COUNT	10

// ribbon bar��
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

	// ����ʾ���ֵķ�ʽ������ť��
	/* ������
	*  strCategoryName: category�����ơ�
	*  iBtnID: ��ťID��
	*/
	void HighlightBtn(CString strCategoryName, int iBtnID, CString strTips);

private:
	// �������˵���
	void CreateMainCategory();

	// ������ݹ�������
	void CreateQuickAccess();

	// ����ribbonҳ��
	void CreateQuickStartCategory();
	void CreateGeomeCategory();
	void CreateCutFeatCategory();
	void CreatePartLayoutCategory();
	void CreateSequenceCategory();
	void CreateConfigCategory();
	void CreateHelpCategory();

	// ����������Ƶõ�������š�
	int GetCategoryIndex(CString strName);

	// �õ���ǰ������ơ�
	CString GetCurCategoryName();

private:
	CMFCRibbonApplicationButton m_mainButton;
	CMFCRibbonBar m_ribbonBar;
};

END_CUTLEADER_NAMESPACE()
