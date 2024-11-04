#include "stdafx.h"
#include "ContentTranslationDlg.h"

#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitText.h"
#include <map>

#include "MultiLanguageMgr.h"
#include "clUtilityResource.h"
#include "clBaseDataResource.h"
#include "ProductFileManager.h"
#include "baseConst.h"

using namespace std;


BEGIN_CUTLEADER_NAMESPACE()


IMPLEMENT_DYNCREATE(ContentTranslationDlg, CDialogEx)

ContentTranslationDlg::ContentTranslationDlg(CWnd* pParent)
						 : CLDialog(ContentTranslationDlg::IDD, pParent)
{
    m_strMsg_1 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STRING_CATEGORY_1_EXPLAIN);
    m_strMsg_2 = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STRING_CATEGORY_2_EXPLAIN);
}

ContentTranslationDlg::~ContentTranslationDlg()
{
}

void ContentTranslationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_TAB_TRANSLATE, m_tabCtrl);
    DDX_Text(pDX, IDC_EDIT_MSG_1, m_strMsg_1);
    DDX_Text(pDX, IDC_EDIT_MSG_2, m_strMsg_2);
	DDX_Control(pDX, IDC_LIST_TRANSLATION_1, m_translationList_1);
    DDX_Control(pDX, IDC_LIST_TRANSLATION_2, m_translationList_2);
}

BEGIN_MESSAGE_MAP(ContentTranslationDlg, CDialogEx)
	ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()

    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_TRANSLATE, OnSelchangeTab)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_TRANSLATION_1, OnGrid_beginLabelEdit_1)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_TRANSLATION_1, OnGrid_endLabelEdit_1)
    ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_TRANSLATION_2, OnGrid_beginLabelEdit_2)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_TRANSLATION_2, OnGrid_endLabelEdit_2)
	ON_BN_CLICKED(IDC_BTN_OK, OnClickOk)
	ON_BN_CLICKED(IDC_BTN_CANCEL, OnClickCancel)
END_MESSAGE_MAP()

BOOL ContentTranslationDlg::OnInitDialog()
{
	__super::OnInitDialog();

    // init the tab control
    CString str;
    str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STRING_CATEGORY_1);
    m_tabCtrl.InsertItem(0, str);
    str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STRING_CATEGORY_2);
    m_tabCtrl.InsertItem(1, str);

    // ��һ���ַ���
    {
        // ������ʽ
        {
            CString str;

            // Requires one never uses column 0
            m_translationList_1.InsertHiddenLabelColumn();

            // Ӣ������
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ENG);
            CGridColumnTrait* pEnglishCol = new CGridColumnTraitText;
            m_translationList_1.InsertColumnTrait(1, str, LVCFMT_LEFT, 420, 0, pEnglishCol);

            // ��ǰ����
            map<LANG_TYPE, int> allSupportedLangs = MultiLanguageMgr::GetInstance()->GetAllSupportedLangs();
            str = MultiLanguageMgr::GetInstance()->TranslateString(allSupportedLangs[m_iCurrentLanguage]);
            CGridColumnTrait* pCurrentLanguageCol = new CGridColumnTraitEdit;
            m_translationList_1.InsertColumnTrait(2, str, LVCFMT_LEFT, 420, 0, pCurrentLanguageCol);

            m_translationList_1.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        }

        // �����
        {
            int iRowCount = 0;
            const map<CString, CString>& translationData_1 = MultiLanguageMgr::GetInstance()->GetTranslationData().Get_TranslationData_1();
            for (map<CString, CString>::const_iterator iter = translationData_1.begin(); iter != translationData_1.end(); iter++)
            {
                GridData* pGridData = new GridData(iter->first);
                m_translationList_1.InsertItem(iRowCount, _T(""));
                m_translationList_1.SetItemText(iRowCount, 1, iter->first);
                m_translationList_1.SetItemText(iRowCount, 2, iter->second);
                m_translationList_1.SetItemData(iRowCount, (DWORD_PTR)pGridData);
                iRowCount++;
            }
        }
    }

    // �ڶ����ַ���
    {
        // ������ʽ
        {
            CString str;

            // Requires one never uses column 0
            m_translationList_2.InsertHiddenLabelColumn();

            // �ַ���ID
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_STRING_ID);
            CGridColumnTrait* pIDCol = new CGridColumnTraitText;
            m_translationList_2.InsertColumnTrait(1, str, LVCFMT_LEFT, 100, 0, pIDCol);

            // Ӣ������
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_ENG);
            CGridColumnTrait* pEnglishCol = new CGridColumnTraitText;
            m_translationList_2.InsertColumnTrait(2, str, LVCFMT_LEFT, 420, 0, pEnglishCol);

            // ��ǰ����
            map<LANG_TYPE, int> allSupportedLangs = MultiLanguageMgr::GetInstance()->GetAllSupportedLangs();
            str = MultiLanguageMgr::GetInstance()->TranslateString(allSupportedLangs[m_iCurrentLanguage]);
            CGridColumnTrait* pCurrentLanguageCol = new CGridColumnTraitEdit;
            m_translationList_2.InsertColumnTrait(3, str, LVCFMT_LEFT, 420, 0, pCurrentLanguageCol);

            m_translationList_2.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
        }

        // �����
        {
            int iRowCount = 0;
            const map<int, CString>& translationData_2 = MultiLanguageMgr::GetInstance()->GetTranslationData().Get_TranslationData_2();
            for (map<int, CString>::const_iterator iter = translationData_2.begin(); iter != translationData_2.end(); iter++)
            {
            	CString strEnglishString;
            	strEnglishString.LoadString(iter->first);
            	GridData* pGridData = new GridData(iter->first);
            	m_translationList_2.InsertItem(iRowCount, _T(""));
                CString strStringID;
                strStringID.Format(_T("%d"), iter->first);
                m_translationList_2.SetItemText(iRowCount, 1, strStringID);
            	m_translationList_2.SetItemText(iRowCount, 2, strEnglishString);
            	m_translationList_2.SetItemText(iRowCount, 3, iter->second);
            	m_translationList_2.SetItemData(iRowCount, (DWORD_PTR)pGridData);
            	iRowCount++;
            }
        }
    }

    ((CWnd*)GetDlgItem(IDC_EDIT_MSG_1))->ShowWindow(SW_SHOW);
    ((CWnd*)GetDlgItem(IDC_LIST_TRANSLATION_1))->ShowWindow(SW_SHOW);
    ((CWnd*)GetDlgItem(IDC_EDIT_MSG_2))->ShowWindow(SW_HIDE);
    ((CWnd*)GetDlgItem(IDC_LIST_TRANSLATION_2))->ShowWindow(SW_HIDE);

	return TRUE;
}

void ContentTranslationDlg::OnDestroy()
{
	for (int i = 0; i < m_translationList_1.GetItemCount(); i++)
	{
		GridData* pGridData = (GridData*)m_translationList_1.GetItemData(i);
		delete pGridData;
	}

    for (int i = 0; i < m_translationList_2.GetItemCount(); i++)
    {
        GridData* pGridData = (GridData*)m_translationList_2.GetItemData(i);
        delete pGridData;
    }

	CLDialog::OnDestroy();
}

BOOL ContentTranslationDlg::OnEraseBkgnd(CDC* pDC)
{
    CRect r;
    GetClientRect(&r);
    if ((HBRUSH)m_brush == NULL)
        m_brush.CreateSolidBrush(UI_COLOR);
    pDC->FillRect(&r, &m_brush);

    return TRUE;
}

HBRUSH ContentTranslationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    TCHAR classname[MAX_PATH];
    if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
        return hbr;

    // message edit box will display the bk color same as the dlg.
    if (_tcsicmp(classname, _T("EDIT")) == 0 && (pWnd->GetDlgCtrlID() != IDC_EDIT_MSG_1 && pWnd->GetDlgCtrlID() != IDC_EDIT_MSG_2))
    {
        return hbr;
    }

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

void ContentTranslationDlg::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (m_tabCtrl.GetCurSel() == 0)
    {
        ((CWnd*)GetDlgItem(IDC_EDIT_MSG_1))->ShowWindow(SW_SHOW);
        ((CWnd*)GetDlgItem(IDC_LIST_TRANSLATION_1))->ShowWindow(SW_SHOW);

        ((CWnd*)GetDlgItem(IDC_EDIT_MSG_2))->ShowWindow(SW_HIDE);
        ((CWnd*)GetDlgItem(IDC_LIST_TRANSLATION_2))->ShowWindow(SW_HIDE);
    }
    else if (m_tabCtrl.GetCurSel() == 1)
    {
        ((CWnd*)GetDlgItem(IDC_EDIT_MSG_1))->ShowWindow(SW_HIDE);
        ((CWnd*)GetDlgItem(IDC_LIST_TRANSLATION_1))->ShowWindow(SW_HIDE);

        ((CWnd*)GetDlgItem(IDC_EDIT_MSG_2))->ShowWindow(SW_SHOW);
        ((CWnd*)GetDlgItem(IDC_LIST_TRANSLATION_2))->ShowWindow(SW_SHOW);
    }
}

void ContentTranslationDlg::OnGrid_beginLabelEdit_1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	int iRowIndex = pDispInfo->item.iItem;
	CString str = pDispInfo->item.pszText;

	int iColumnIndex = pDispInfo->item.iSubItem;
	if (iColumnIndex == 2)
	{
		m_strCellText_beforeEdit = m_translationList_1.GetItemText(iRowIndex, 2);
	}
}

void ContentTranslationDlg::OnGrid_endLabelEdit_1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	int iRowIndex = pDispInfo->item.iItem;
	CString str = pDispInfo->item.pszText;

	int iColumnIndex = pDispInfo->item.iSubItem;
	if (iColumnIndex == 2)
	{
		if (str != m_strCellText_beforeEdit)
		{
			GridData* pGridData = (GridData*)m_translationList_1.GetItemData(iRowIndex);
			m_translationData_1_modified[pGridData->m_strKey] = str;
		}
	}
}

void ContentTranslationDlg::OnGrid_beginLabelEdit_2(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
    int iRowIndex = pDispInfo->item.iItem;
    CString str = pDispInfo->item.pszText;

    int iColumnIndex = pDispInfo->item.iSubItem;
    if (iColumnIndex == 3)
    {
        m_strCellText_beforeEdit = m_translationList_2.GetItemText(iRowIndex, 3);
    }
}

void ContentTranslationDlg::OnGrid_endLabelEdit_2(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
    int iRowIndex = pDispInfo->item.iItem;
    CString str = pDispInfo->item.pszText;

    int iColumnIndex = pDispInfo->item.iSubItem;
    if (iColumnIndex == 3)
    {
        if (str != m_strCellText_beforeEdit)
        {
            GridData* pGridData = (GridData*)m_translationList_2.GetItemData(iRowIndex);
            m_translationData_2_modified[pGridData->m_iStringID] = str;
        }
    }
}

void ContentTranslationDlg::OnClickOk()
{
	if (m_translationData_1_modified.size() > 0 || m_translationData_2_modified.size() > 0)
	{
		if (MultiLanguageMgr::GetInstance()->UpdateTranslationData(m_translationData_1_modified, m_translationData_2_modified, m_iCurrentLanguage))
		{
			CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
			CString strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CHANGE_NEEDTO_RESTART);
			MessageBox(strMsg, strProductName, MB_OK | MB_ICONINFORMATION);
		}
	}

	EndDialog(IDC_BTN_OK);
}

void ContentTranslationDlg::OnClickCancel()
{
	if (m_translationData_1_modified.size() > 0 || m_translationData_2_modified.size() > 0)
	{
		CString strProductName = MultiLanguageMgr::GetInstance()->TranslateString(IDS_PRODUCT_NAME);
		CString strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_SAVE_TRANSLATION);
		if (MessageBox(strMsg, strProductName, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			if (MultiLanguageMgr::GetInstance()->UpdateTranslationData(m_translationData_1_modified, m_translationData_2_modified, m_iCurrentLanguage))
			{
				strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_CHANGE_NEEDTO_RESTART);
				MessageBox(strMsg, strProductName, MB_OK | MB_ICONINFORMATION);
			}
		}
	}

	EndDialog(IDC_BTN_CANCEL);
}

END_CUTLEADER_NAMESPACE()
