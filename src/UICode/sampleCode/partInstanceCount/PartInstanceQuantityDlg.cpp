#include "StdAfx.h"
#include "PartInstanceQuantityDlg.h"

#include "Sheet.h"
#include "PartPlacementList.h"

using namespace std;

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(PartInstanceQuantityDlg, CDialogEx)

BEGIN_MESSAGE_MAP(PartInstanceQuantityDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

PartInstanceQuantityDlg::PartInstanceQuantityDlg(SheetPtr pSheet, CWnd* pParent /*=NULL*/)
					: CLDialog(PartInstanceQuantityDlg::IDD, pParent)
{
    m_pSheet = pSheet;
}

PartInstanceQuantityDlg::~PartInstanceQuantityDlg(void)
{
}

void PartInstanceQuantityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);


	DDX_Text(pDX, IDC_EDIT_PartInstanceQuantity, m_strPartInstanceQuantity);
}

BOOL PartInstanceQuantityDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

    PartPlacementListPtr pPartPlacements = m_pSheet->GetPartPlacements();
	m_strPartInstanceQuantity.Format(_T("%d"), pPartPlacements->GetPartInstanceCount());

	SetBackgroundColor(UI_COLOR);
	UpdateData(FALSE);

	return TRUE;
}

void PartInstanceQuantityDlg::OnBnClickedOk()
{
	OnOK();
}


END_CUTLEADER_NAMESPACE()
