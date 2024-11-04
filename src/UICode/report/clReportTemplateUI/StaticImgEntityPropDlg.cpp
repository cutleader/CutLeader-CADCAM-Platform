#include "stdafx.h"
#include "StaticImgEntityPropDlg.h"

#include "baseConst.h"
#include "baseConst.h"
#include "FileHelper.h"
#include "RandomGen.h"
#include "ProductFolderManager.h"

#include "ImgData.h"
#include "StaticImgEntity.h"
#include "clReportResource.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(StaticImgEntityPropDlg, CDialogEx)

StaticImgEntityPropDlg::StaticImgEntityPropDlg()
					: CLDialog(StaticImgEntityPropDlg::IDD)
{
	m_strImgFilePath = _T("");
}

StaticImgEntityPropDlg::~StaticImgEntityPropDlg(void)
{
}

void StaticImgEntityPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Control(pDX, IDC_STATIC_PIC_PREVIEW, m_ctrlImgPreview);

	DDX_Check(pDX, IDC_CHECK_BORDER, m_bBorder);
	DDX_Text(pDX, IDC_EDIT_BORDER_THICKNESS, m_iBorderThickness);
	DDX_Control(pDX, IDC_COMBO_BORDER_STYLE, m_ctrlBorderStyle);
	DDX_Control(pDX, IDC_STATIC_BORDER_COLOR, m_ctrlBorderColor);
}

BOOL StaticImgEntityPropDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	/************************************************************************/
	// init some style.

	CString text;
	text = MultiLanguageMgr::GetInstance()->TranslateString(IDS_LINE_STYLE_1);
	m_ctrlBorderStyle.SetItemData(m_ctrlBorderStyle.AddString(text), PS_SOLID);
	text = MultiLanguageMgr::GetInstance()->TranslateString(IDS_LINE_STYLE_2);
	m_ctrlBorderStyle.SetItemData(m_ctrlBorderStyle.AddString(text), PS_DASH);
	text = MultiLanguageMgr::GetInstance()->TranslateString(IDS_LINE_STYLE_3);
	m_ctrlBorderStyle.SetItemData(m_ctrlBorderStyle.AddString(text), PS_DOT);
	/************************************************************************/

	/************************************************************************/
	// init the dialog content.

	m_strName = m_pStaticImgEntity->GetName();

	// about border.
	m_bBorder = m_pStaticImgEntity->IsShowBorder();
	m_iBorderThickness = m_pStaticImgEntity->GetBorderThickness();
	m_ctrlBorderColor.SetColor( m_pStaticImgEntity->GetBorderColor());
	for (int t = 0; t < m_ctrlBorderStyle.GetCount(); t++)
		if (m_ctrlBorderStyle.GetItemData(t) == static_cast<DWORD>(m_pStaticImgEntity->GetBorderStyle()))
			m_ctrlBorderStyle.SetCurSel(t);
	/************************************************************************/

	UpdateData(FALSE);

	SetBackgroundColor(UI_COLOR);

	return TRUE;
}

BEGIN_MESSAGE_MAP(StaticImgEntityPropDlg, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_PAINT()

	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_BTN_BORDER_COLOR, OnBorderColor)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)

END_MESSAGE_MAP()

BOOL StaticImgEntityPropDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH StaticImgEntityPropDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	// here, message edit box will display the bk color same as the dlg.
	if (_tcsicmp(classname, _T("EDIT")) == 0 && pWnd->GetDlgCtrlID() != IDC_EDIT_MSG)
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

void StaticImgEntityPropDlg::OnPaint()
{
	CDialogEx::OnPaint();

	// whether user selected a image file from disk.
	if (m_strImgFilePath != _T(""))
	{
		// load the image.
		CImage imgObj;
		imgObj.Load(m_strImgFilePath);

		// display image.
		CRect clientRect;
		m_ctrlImgPreview.GetClientRect(&clientRect);
		imgObj.Draw(m_ctrlImgPreview.GetDC()->m_hDC, 0, 0, clientRect.Width(), clientRect.Height(), 
					0, 0, imgObj.GetWidth(), imgObj.GetHeight());
	}
	else
	{
		// the image data.
		ImgDataPtr pImgData = m_pStaticImgEntity->GetImageData();
		if (pImgData)
		{
			// create a tmp folder to hold image.
			CString strImgPath = ProductFolderManager::GetTempPath();
			CString strFolderName;
			strFolderName.Format(_T("%I64d"), RandomGen::genrand64_int63());
			strImgPath += strFolderName;
			CreateDirectory(strImgPath, NULL);

			// write the image file.
			CString strImgFullPath = strImgPath;
			CString strFileName;
			strFileName.Format(_T("%I64d"), RandomGen::genrand64_int63());
			strImgFullPath += _T("\\");
			strImgFullPath += strFileName;
			strImgFullPath += _T(".");
			strImgFullPath += pImgData->GetExt();
			HANDLE hFile = CreateFile(strImgFullPath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			DWORD dwBytesWrite = 0;
			::WriteFile(hFile, pImgData->GetData(), pImgData->GetSize(), &dwBytesWrite, NULL);
			CloseHandle(hFile);

			// load the image.
			CImage imgObj;
			imgObj.Load(strImgFullPath);

			// display image.
			CRect clientRect;
			m_ctrlImgPreview.GetClientRect(&clientRect);
			imgObj.Draw(m_ctrlImgPreview.GetDC()->m_hDC, 0, 0, clientRect.Width(), clientRect.Height(), 
						0, 0, imgObj.GetWidth(), imgObj.GetHeight());

			// delete the tmp folder.
			FileHelper::DeleteDirectory(strImgPath);
		}
	}
}

void StaticImgEntityPropDlg::OnBrowse()
{
	CString strFilter, strDflExt;
	strFilter = MultiLanguageMgr::GetInstance()->TranslateString(IDS_IMG_FILTER);
	strDflExt = MultiLanguageMgr::GetInstance()->TranslateString(IDS_BMP_EXT);
	CFileDialog impFileDlg(TRUE, strDflExt, NULL, OFN_FILEMUSTEXIST, strFilter);
	if (impFileDlg.DoModal() == IDOK)
	{
		m_strImgFilePath = impFileDlg.GetPathName();
		m_strImgFileExt = impFileDlg.GetFileExt();

		RedrawWindow();
	}
}

void StaticImgEntityPropDlg::OnBorderColor()
{
	CColorDialog dlg(m_pStaticImgEntity->GetBorderColor());
	if (dlg.DoModal() == IDOK)
	{
		m_ctrlBorderColor.SetColor(dlg.GetColor());
	}
}

void StaticImgEntityPropDlg::OnOK() 
{
	UpdateData(TRUE);

	// the picture name.
	m_pStaticImgEntity->SetName(m_strName);

	// for border.
	m_pStaticImgEntity->IsShowBorder(m_bBorder);
	m_pStaticImgEntity->SetBorderThickness(m_iBorderThickness);
	LINE_STYLE_TYPE borderLineStyle = (LINE_STYLE_TYPE)m_ctrlBorderStyle.GetItemData( m_ctrlBorderStyle.GetCurSel() );
	m_pStaticImgEntity->SetBorderStyle(borderLineStyle);
	m_pStaticImgEntity->SetBorderColor(m_ctrlBorderColor.GetColor());

	// read image data from file.
	if (m_strImgFilePath != _T(""))
	{
		// read data from file.
		HANDLE hFile = CreateFile(m_strImgFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		void* pBuf = malloc(MAXLENGTH_STR_1M*4);
		DWORD dwBytesRead = 0;
		::ReadFile(hFile, pBuf, MAXLENGTH_STR_1M*4, &dwBytesRead, NULL);

		// init.
		ImgDataPtr pImgData(new ImgData);
		pImgData->SetData(pBuf);
		pImgData->SetSize(dwBytesRead);
		pImgData->SetExt(m_strImgFileExt);
		m_pStaticImgEntity->SetImageData(pImgData);

		// close file.
		::CloseHandle(hFile);
	}

	CDialogEx::OnOK();
}

void StaticImgEntityPropDlg::OnCancel() 
{
	CDialogEx::OnCancel();
}

END_CUTLEADER_NAMESPACE()
