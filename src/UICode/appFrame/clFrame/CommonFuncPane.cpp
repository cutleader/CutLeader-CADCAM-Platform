#include "stdafx.h"
#include "CommonFuncPane.h"

#include "clFrameResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_CUTLEADER_NAMESPACE()

CommonFuncPane* CommonFuncPane::m_pCommonFuncPane = NULL;

BEGIN_MESSAGE_MAP(CommonFuncPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

CommonFuncPane::CommonFuncPane()
{
	m_bPartMode = FALSE;
	m_bSheetMode = FALSE;
}

CommonFuncPane::~CommonFuncPane()
{
}

CommonFuncPane* CommonFuncPane::GetInstance(CWnd* pParent)
{
	if (m_pCommonFuncPane == NULL)
	{
		CString str;
		str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMON_FUNC_BAR);

		m_pCommonFuncPane = new CommonFuncPane();
		m_pCommonFuncPane->m_pMainWnd = pParent;
		m_pCommonFuncPane->Create(str, (CWnd*)pParent, CRect(0, 0, 150, 200), TRUE, IDC_COMMON_FUNC_BAR, 
								  WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|CBRS_LEFT|CBRS_FLOAT_MULTI);
		m_pCommonFuncPane->ShowBar(FALSE);
	}

	return m_pCommonFuncPane;
}

int CommonFuncPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create the sub dialog.
	m_partComFuncDlg.Create(IDD_clFrame_PartCommonFunction, this);
	m_sheetComFuncDlg.Create(IDD_clFrame_SheetCommonFunction, this);

	return 0;
}

void CommonFuncPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// position the sub dialog
	CRect rectClient;
	GetClientRect(rectClient);
	m_partComFuncDlg.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), 
								  SWP_NOACTIVATE | SWP_NOZORDER);
	m_sheetComFuncDlg.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), 
								   SWP_NOACTIVATE | SWP_NOZORDER);

	if (m_bPartMode)
		m_partComFuncDlg.ShowWindow(SW_SHOW);
	if (m_bSheetMode)
		m_sheetComFuncDlg.ShowWindow(SW_SHOW);
}

void CommonFuncPane::DisplayPartPane()
{
	if (!m_bPartMode)
	{
		m_partComFuncDlg.ShowWindow(SW_SHOW);
		m_sheetComFuncDlg.ShowWindow(SW_HIDE);

		m_bPartMode = TRUE;
		m_bSheetMode = FALSE;
	}
}

void CommonFuncPane::DisplaySheetPane()
{
	if (!m_bSheetMode)
	{
		m_partComFuncDlg.ShowWindow(SW_HIDE);
		m_sheetComFuncDlg.ShowWindow(SW_SHOW);

		m_bPartMode = FALSE;
		m_bSheetMode = TRUE;
	}
}

END_CUTLEADER_NAMESPACE()
