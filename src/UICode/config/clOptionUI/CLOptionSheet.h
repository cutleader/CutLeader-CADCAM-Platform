#pragma once

#include "clOptionUICommon.h"
#include "taoresource.h"
#include "CLResizableDialog.h"
#include "ProductParamPage.h"
#include "ImpExpConfigPage.h"
#include "DisplayParamPage.h"
#include "PartEditParamPage.h"
#include "SheetEditParamPage.h"

#include "CLOptionInfo.h"

BEGIN_CUTLEADER_NAMESPACE()

typedef enum tagConfigType
{
	CONFIG_NOT_INIT		= -1,

	CONFIG_GENERAL		= 1,
	CONFIG_IMPEXP		= 2,
	CONFIG_DISPLAY		= 3,
	CONFIG_PART_EDIT	= 4,
	CONFIG_SHEET_EDIT	= 5,
} CONFIG_TYPE;

// ��ϵͳ���������ý��档
class CLOPTIONUI_Export CLOptionSheet : public CLResizableDialog
{
public:
	CLOptionSheet(CWnd* pParent = NULL);
	~CLOptionSheet();

	enum { IDD = IDD_clOptionUI_Main };

protected: // inherit the interface
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

public: // message mapping.
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnGeneralConfig();
	afx_msg void OnImpExpConfig();
	afx_msg void OnColorConfig();
	afx_msg void OnDisplayPartParam();
	afx_msg void OnDisplaySheetParam();

	afx_msg void OnSave();
	afx_msg void OnQuit();

	DECLARE_MESSAGE_MAP()

public:
	// ���ó�ʼҳ��
	void InitStartPage(CONFIG_TYPE iConfigType) { m_iConfigType = iConfigType; }

private:
	void InitSubDialog();

	// hide all sub-dialogs.
	void HideAllSubDialog();

	void RepositionSubDlg();

	void CheckModification();

	// ����ѡ������ܵ�Ӱ���һЩ������
	void SaveAndUpdate();

private: // for UI.
	ProductParamPage m_productParamPage;
	ImpExpConfigPage m_impExpConfigPage;
	DisplayParamPage m_colorParamPage;
	PartEditParamPage m_partParamPage;
	SheetEditParamPage m_sheetParamPage;

private: // for data
	CLOptionInfoPtr m_pCLOptionInfo;

	// the current page type.
	CONFIG_TYPE m_iConfigType;
};

END_CUTLEADER_NAMESPACE()
