#include "StdAfx.h"
#include "DflTplDlg.h"

#include "baseConst.h"
#include "ProductFileDefine.h"
#include "ProductFolderManager.h"

#include "clReportResource.h"
#include "ReportTpl.h"
#include "ReportTplList.h"
#include "ReportTplLoader.h"

#include "CLOptionInfo.h"
#include "ImpExpConfig.h"
#include "CLOptionWriter.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(DflTplDlg, CDialogEx)

DflTplDlg::DflTplDlg(CWnd* pParent)
	: CLDialog(DflTplDlg::IDD, pParent)
{
}

DflTplDlg::~DflTplDlg(void)
{
}

void DflTplDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_SHT_TPL, m_ctrlShtTpl);
	DDX_Control(pDX, IDC_COMBO_NEST_TPL, m_ctrlNestTpl);
	DDX_Control(pDX, IDC_COMBO_LABEL_TPL, m_ctrlLabelTpl);
}

BEGIN_MESSAGE_MAP(DflTplDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL DflTplDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	// Ĭ�ϱ���ģ�塣
	ImpExpConfigPtr pImpExpConfig = CLOptionInfo::GetInstance()->GetImpExpConfig();
	LONGLONG iShtRptTplID = pImpExpConfig->GetShtRptTplID();
	LONGLONG iNestRptTplID = pImpExpConfig->GetNestRptTplID();
	LONGLONG iLabelRptTplID = pImpExpConfig->GetLabelRptTplID();

	// ���б���ģ�塣
	ReportTplListPtr pAllReportTpls = ReportTplLoader::LoadAllReportTpl(FALSE);

	// ��ġ�
	ReportTplListPtr pShtReportTpls = pAllReportTpls->GetTplByType(SHEET_REPORT);
	if (pShtReportTpls->size() > 0)
	{
		int iSelIndex = 0;
		for (unsigned int i = 0; i < pShtReportTpls->size(); i++)
		{
			ReportTplPtr pReportTpl = pShtReportTpls->at(i);
			m_ctrlShtTpl.AddString(pReportTpl->GetName());
			m_ctrlShtTpl.SetItemData(i, (DWORD_PTR)(new LONGLONG(pReportTpl->GetID())));
			if (pReportTpl->GetID() == iShtRptTplID)
			{
				iSelIndex = i;
			}
		}
		m_ctrlShtTpl.SetCurSel(iSelIndex);
	}

	// �Ű�����
	ReportTplListPtr pNestReportTpls = pAllReportTpls->GetTplByType(NESTJOB_REPORT);
	if (pNestReportTpls->size() > 0)
	{
		int iSelIndex = 0;
		for (unsigned int i = 0; i < pNestReportTpls->size(); i++)
		{
			ReportTplPtr pReportTpl = pNestReportTpls->at(i);
			m_ctrlNestTpl.AddString(pReportTpl->GetName());
			m_ctrlNestTpl.SetItemData(i, (DWORD_PTR)(new LONGLONG(pReportTpl->GetID())));
			if (pReportTpl->GetID() == iNestRptTplID)
			{
				iSelIndex = i;
			}
		}
		m_ctrlNestTpl.SetCurSel(iSelIndex);
	}

	// ��ǩ��
	ReportTplListPtr pLabelReportTpls = pAllReportTpls->GetTplByType(LABEL_REPORT);
	if (pLabelReportTpls->size() > 0)
	{
		int iSelIndex = 0;
		for (unsigned int i = 0; i < pLabelReportTpls->size(); i++)
		{
			ReportTplPtr pReportTpl = pLabelReportTpls->at(i);
			m_ctrlLabelTpl.AddString(pReportTpl->GetName());
			m_ctrlLabelTpl.SetItemData(i, (DWORD_PTR)(new LONGLONG(pReportTpl->GetID())));
			if (pReportTpl->GetID() == iLabelRptTplID)
			{
				iSelIndex = i;
			}
		}
		m_ctrlLabelTpl.SetCurSel(iSelIndex);
	}

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

void DflTplDlg::OnBnClickedOk()
{
	CLOptionInfoPtr pCLOptionInfo = CLOptionInfo::GetInstance();
	ImpExpConfigPtr pImpExpConfig = pCLOptionInfo->GetImpExpConfig();

	// �õ��µ�Ĭ��ģ�塣
	int iIndex = m_ctrlShtTpl.GetCurSel();
	if (iIndex >= 0)
	{
		LONGLONG* pTplID = (LONGLONG*)m_ctrlShtTpl.GetItemData(iIndex);
		pImpExpConfig->SetShtRptTplID(*pTplID);
	}
	iIndex = m_ctrlNestTpl.GetCurSel();
	if (iIndex >= 0)
	{
		LONGLONG* pTplID = (LONGLONG*)m_ctrlNestTpl.GetItemData(iIndex);
		pImpExpConfig->SetNestRptTplID(*pTplID);
	}
	iIndex = m_ctrlLabelTpl.GetCurSel();
	if (iIndex >= 0)
	{
		LONGLONG* pTplID = (LONGLONG*)m_ctrlLabelTpl.GetItemData(iIndex);
		pImpExpConfig->SetLabelRptTplID(*pTplID);
	}

	// ���档
	CString strPath = ProductFolderManager::GetOptionPath();
	strPath += USER_OPTION_FILE;
	CLOptionWriter::SaveCLOption(pCLOptionInfo, strPath);

	OnOK();
}

END_CUTLEADER_NAMESPACE()
