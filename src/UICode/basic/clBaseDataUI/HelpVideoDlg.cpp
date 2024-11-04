#include "StdAfx.h"
#include "HelpVideoDlg.h"

#include "clUtilityResource.h"
#include "CGridColumnTraitText.h"

#include "baseConst.h"
#include "clBaseDataResource.h"
#include "ProductFolderManager.h"
#include "ProductFileDefine.h"
#include "VideoInfoLoader.h"
#include "HelpVideo.h"
#include "HelpVideoCategory.h"

BEGIN_CUTLEADER_NAMESPACE()

HelpVideoDlg::HelpVideoDlg(LANG_TYPE iLang, BOOL bAutoShow) : CLDialog(HelpVideoDlg::IDD)
{
	m_iLang = iLang;
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_PLAY_VIDEO);

	m_bAutoShow = bAutoShow;
}

void HelpVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_CATEGORY, m_categoryList);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
	DDX_Control(pDX, IDC_LIST_VIDEO, m_videoList);

	DDX_Check(pDX, IDC_CHECK_AT_START, m_bAutoShow);
}

BEGIN_MESSAGE_MAP(HelpVideoDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CATEGORY, OnSelchangedCategory)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_VIDEO, OnDblclkVideo)

	ON_BN_CLICKED(IDC_CHECK_AT_START, OnCheckAtStart)

	ON_BN_CLICKED(IDC_BTN_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_BTN_MINIMIZE, OnMinimize)
END_MESSAGE_MAP()

BOOL HelpVideoDlg::OnInitDialog()
{
	__super::OnInitDialog();

	// display the style of category list.
	InitGridStyle();

	// load video info.
	m_videoCategoryList = VideoInfoLoader::LoadHelpVideo(m_iLang);

	// fill category list.
	for (unsigned int i = 0; i < m_videoCategoryList.size(); i++)
	{
		HelpVideoCategoryPtr pHelpVideoCategory = m_videoCategoryList.at(i);

		LVITEM categoryItem;
		categoryItem.mask =  LVIF_IMAGE | LVIF_TEXT;
		categoryItem.iItem = i;
		categoryItem.iSubItem = 0;
		categoryItem.pszText = pHelpVideoCategory->GetName().AllocSysString();
		categoryItem.iImage = i;
		m_categoryList.InsertItem(&categoryItem);
	}

	// select the first row.
	m_categoryList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

	CenterWindow();

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

void HelpVideoDlg::PostNcDestroy()
{
	CDialogEx::PostNcDestroy();
	delete this;
}

void HelpVideoDlg::OnCancel()
{
	CDialogEx::OnCancel();
	DestroyWindow();
}

BOOL HelpVideoDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH HelpVideoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("COMBOLBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("LISTBOX")) == 0)
		return hbr;

	if (_tcsicmp(classname, WC_TREEVIEW) == 0)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

void HelpVideoDlg::OnSelchangedCategory(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	int iIndex = -1;
	if (pNMListView->uNewState && (iIndex = m_categoryList.GetNextItem(iIndex, LVNI_SELECTED))!= -1)
		DisplayVideoInfo(m_videoCategoryList.at(iIndex));
}

void HelpVideoDlg::OnDblclkVideo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int iCategoryIndex = m_categoryList.GetSelectionMark();
	if (iCategoryIndex != -1)
	{
		HelpVideoCategoryPtr pHelpVideoCategory = m_videoCategoryList.at(iCategoryIndex);

		int iVideoIndex = pNMLV->iItem;
		if (iVideoIndex != -1)
		{
			HelpVideoPtr pHelpVideo = pHelpVideoCategory->at(iVideoIndex);
			PlayVideo(pHelpVideo->GetFileName());
		}
	}
}

void HelpVideoDlg::OnCheckAtStart()
{
	UpdateData(TRUE);

	// told app to save the change.
	AfxGetApp()->PostThreadMessage(WM_DATA_AUTO_POPUP_VIDEO, (WPARAM)m_bAutoShow, NULL);
}

void HelpVideoDlg::OnPlay()
{
	int iCategoryIndex = m_categoryList.GetSelectionMark();
	if (iCategoryIndex != -1)
	{
		HelpVideoCategoryPtr pHelpVideoCategory = m_videoCategoryList.at(iCategoryIndex);

		int iVideoIndex = m_videoList.GetSelectionMark();
		if (iVideoIndex != -1)
		{
			HelpVideoPtr pHelpVideo = pHelpVideoCategory->at(iVideoIndex);
			PlayVideo(pHelpVideo->GetFileName());
		}
	}
}

void HelpVideoDlg::OnMinimize()
{
	ShowWindow(SW_SHOWMINIMIZED);
}

void HelpVideoDlg::DisplayVideoInfo(HelpVideoCategoryPtr pHelpVideoCategory)
{
	CString str;

	m_videoList.DeleteAllItems();

	// init the video list.
	int nItem = 0;
	for (unsigned int i = 0; i < pHelpVideoCategory->size(); i++)
	{
		HelpVideoPtr pHelpVideo = pHelpVideoCategory->at(i);

		// init a row.
		CString strNum;
		strNum.Format(_T("%d"), i+1);
		nItem = m_videoList.InsertItem(++nItem, strNum);

		// column num is started from 1 because there is a hidden column.
		m_videoList.SetItemText(nItem, 1, strNum);
		m_videoList.SetItemText(nItem, 2, pHelpVideo->GetName());
		m_videoList.SetItemText(nItem, 3, pHelpVideo->GetComment());
	}

	// select the first row.
	if (m_videoList.GetItemCount() > 0)
		m_videoList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
}

void HelpVideoDlg::InitGridStyle()
{
	CString str;

	/************************************************************************/
	// init the image list for category list.

	// Create 256 color image lists
	HIMAGELIST hList = ImageList_Create(32, 32, ILC_COLOR8 |ILC_MASK, 7, 1);
	m_imageList.Attach(hList);

	// Load the large icons
	CBitmap bmp;
	bmp.LoadBitmap(IDB_VIDEO_CATEGORY);
	m_imageList.Add(&bmp, RGB(192, 192, 192));
	bmp.DeleteObject();

	// Attach them
	m_categoryList.SetImageList(&m_imageList, LVSIL_NORMAL);
	/************************************************************************/

	/************************************************************************/
	// init the style of video list.

	// Requires one never uses column 0
	m_videoList.InsertHiddenLabelColumn();

	// "num" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NUM);
	CGridColumnTrait* pNUMCol = new CGridColumnTraitText;
	m_videoList.InsertColumnTrait(1, str, LVCFMT_LEFT, 40, 0, pNUMCol);

	// "name" column
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_NAME);
	CGridColumnTrait* pNameCol = new CGridColumnTraitText;
	m_videoList.InsertColumnTrait(2, str, LVCFMT_LEFT, 120, 0, pNameCol);

	// comment column.
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_COMMENT);
	CGridColumnTrait* pCommentCol = new CGridColumnTraitText;
	m_videoList.InsertColumnTrait(3, str, LVCFMT_LEFT, 420, 0, pCommentCol);

	m_videoList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	/************************************************************************/
}

void HelpVideoDlg::PlayVideo(CString strVideoFileName)
{
	// get the video path.
	CString strPath = ProductFolderManager::GetHelpPath();
	if (m_iLang == LANG_CHS)
		strPath += CHS_VIDEO_FOLDER;
	else
		strPath += ENG_VIDEO_FOLDER;
	strPath += _T("\\");
	strPath += strVideoFileName;
	ShellExecute(NULL, _T("open"), strPath, NULL, NULL, SW_SHOWMAXIMIZED);
}

END_CUTLEADER_NAMESPACE()
