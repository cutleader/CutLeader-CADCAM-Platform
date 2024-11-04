#include "stdafx.h"
#include "MicroJointAssignmentPage.h"

#include "baseConst.h"
#include "CLOptionInfo.h"
#include "ProductParam.h"

#include "clExpertLibResource.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNCREATE(MicroJointAssignmentPage, CResizableDialog)

MicroJointAssignmentPage::MicroJointAssignmentPage() : CLResizableDialog(MicroJointAssignmentPage::IDD)
{
	m_bDisKillFocusEnt = FALSE;
}

MicroJointAssignmentPage::~MicroJointAssignmentPage()
{
}

void MicroJointAssignmentPage::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);

	/************************************************************************/
	// ���Զ����䡱���顣

	DDX_Control(pDX, IDC_TAB_RANGE, m_rangeTab);
	DDX_Control(pDX, IDC_EDIT_HOLES_SIZE, m_dHoleJointPtSize);
	DDX_Control(pDX, IDC_COMBO_HOLES_QUANTITY, m_holeJointPtCount);
	DDX_Control(pDX, IDC_EDIT_HOLES_MAXSIZEX, m_dHoleXSize);
	DDX_Control(pDX, IDC_EDIT_HOLES_MAXSIZEY, m_dHoleYSize);
	DDX_Control(pDX, IDC_STATIC_HOLE, m_holeImg);

	DDX_Control(pDX, IDC_EDIT_PERIMETER_SIZE, m_dPeriJointPtSize);
	DDX_Control(pDX, IDC_COMBO_PERIMETER_QUANTITY, m_periJointPtCount);
	DDX_Control(pDX, IDC_EDIT_PERIMETER_MAXSIZEX, m_dPeriXSize);
	DDX_Control(pDX, IDC_EDIT_PERIMETER_MAXSIZEY, m_dPeriYSize);
	DDX_Control(pDX, IDC_STATIC_PERI, m_periImg);
	/************************************************************************/

	/************************************************************************/
	// lead in/out info

	DDX_Check(pDX, IDC_CHECK_ENABLELEADIN, m_enableLeadIn);
	DDX_Control(pDX, IDC_EDIT_LEADIN_LEN, m_dLeanInLen);
	DDX_Control(pDX, IDC_EDIT_LEADIN_ANGLE, m_dLeadInAngle);

	DDX_Check(pDX, IDC_CHECK_ENABLELEADOUT, m_enableLeadOut);
	DDX_Control(pDX, IDC_EDIT_LEADOUT_LEN, m_dLeanOutLen);
	DDX_Control(pDX, IDC_EDIT_LEADOUT_ANGLE, m_dLeadOutAngle);
	/************************************************************************/

	DDX_Control(pDX, IDC_EDIT_M_MJ_SIZE, m_dManualMJWid);
}

BEGIN_MESSAGE_MAP(MicroJointAssignmentPage, CResizableDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()

	/************************************************************************/
	// ���Զ����䡱���顣

	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_RANGE, OnSelchangeTab)

	ON_CBN_SELCHANGE(IDC_COMBO_HOLES_QUANTITY, OnSelChangeHoleQuantity)
	ON_EN_KILLFOCUS(IDC_EDIT_HOLES_SIZE, OnKillFocusHolesSize)
	ON_EN_KILLFOCUS(IDC_EDIT_HOLES_MAXSIZEX, OnKillFocusHolesSizeX)
	ON_EN_KILLFOCUS(IDC_EDIT_HOLES_MAXSIZEY, OnKillFocusHolesSizeY)

	ON_CBN_SELCHANGE(IDC_COMBO_PERIMETER_QUANTITY, OnSelchangePeriQuantity)
	ON_EN_KILLFOCUS(IDC_EDIT_PERIMETER_SIZE, OnKillFocusPerimeterSize)
	ON_EN_KILLFOCUS(IDC_EDIT_PERIMETER_MAXSIZEX, OnKillFocusPerimeterSizeX)
	ON_EN_KILLFOCUS(IDC_EDIT_PERIMETER_MAXSIZEY, OnKillFocusPerimeterSizeY)

	ON_WM_LBUTTONDOWN()
	/************************************************************************/

	// "Lead In/out" group.
	ON_BN_CLICKED(IDC_CHECK_ENABLELEADIN, OnCheckEnableLeadIn)
	ON_EN_KILLFOCUS(IDC_EDIT_LEADIN_LEN, OnKillFocusLeadInLen)
	ON_EN_KILLFOCUS(IDC_EDIT_LEADIN_ANGLE, OnKillFocusLeadInAngle)
	ON_BN_CLICKED(IDC_CHECK_ENABLELEADOUT, OnCheckEnableLeadOut)
	ON_EN_KILLFOCUS(IDC_EDIT_LEADOUT_LEN, OnKillFocusLeadOutLen)
	ON_EN_KILLFOCUS(IDC_EDIT_LEADOUT_ANGLE, OnKillFocusLeadOutAngle)

	ON_EN_KILLFOCUS(IDC_EDIT_M_MJ_SIZE, OnKillFocusManualWid)
END_MESSAGE_MAP()

BOOL MicroJointAssignmentPage::OnInitDialog()
{
	CLResizableDialog::OnInitDialog();

	// init something
	Init();

	SetBackgroundColor(UI_COLOR);

	UpdateData(FALSE);

	return TRUE;
}

BOOL MicroJointAssignmentPage::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

HBRUSH MicroJointAssignmentPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

	if (_tcsicmp(classname, _T("EDIT")) == 0)
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

void MicroJointAssignmentPage::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_bDisKillFocusEnt = TRUE;

	// init the micro joint rules for specified range.
	int iSel = m_rangeTab.GetCurSel();
	MicroJointRangePtr pRange = m_pMicroJointSetting->GetRangeList()->at(iSel);
	InitRange(pRange);

	m_bDisKillFocusEnt = FALSE;

	*pResult = 0;
}

void MicroJointAssignmentPage::OnSelChangeHoleQuantity()
{
	// get the current range info.
	int iRangeIndex = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr pRangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIndex, TRUE);

	// update "MicroJointRangeInfo::m_iQuantity".
	pRangeInfo->SetQuantity(m_holeJointPtCount.GetCurSel());
	m_pMicroJointSetting->SetModified(TRUE);

	// then we update the preview window.
	InitPosPreview(pRangeInfo->GetQuantity(), pRangeInfo->GetMicroJointPos(), TRUE);
}

void MicroJointAssignmentPage::OnSelchangePeriQuantity()
{
	// get the current range info.
	int iRangeIndex = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIndex, FALSE);

	// update "MicroJointRangeInfo::m_iQuantity".
	rangeInfo->SetQuantity(m_periJointPtCount.GetCurSel()*2);
	m_pMicroJointSetting->SetModified(TRUE);

	// then we update the preview window.
	InitPosPreview(rangeInfo->GetQuantity(), rangeInfo->GetMicroJointPos(), FALSE);
}

void MicroJointAssignmentPage::OnKillFocusManualWid()
{
	if (m_pMicroJointSetting->GetManualWidth() != m_dManualMJWid.Get_Number())
	{
		m_pMicroJointSetting->SetManualWidth(m_dManualMJWid.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect holeStaticRect, periStaticRect;
	m_holeImg.GetWindowRect(&holeStaticRect);
	m_periImg.GetWindowRect(&periStaticRect);

	ScreenToClient(holeStaticRect);
	ScreenToClient(periStaticRect);

	BOOL bHole;
	int jointCount;

	if (holeStaticRect.PtInRect(point))
	{
		m_holeImg.OnLButtonDown(nFlags, point);

		bHole = TRUE;
		jointCount = m_holeJointPtCount.GetCurSel();

		//
		m_pMicroJointSetting->SetModified(TRUE);
	}
	else if (periStaticRect.PtInRect(point))
	{
		m_periImg.OnLButtonDown(nFlags, point);

		bHole = FALSE;
		jointCount = m_holeJointPtCount.GetCurSel()*2;

		//
		m_pMicroJointSetting->SetModified(TRUE);
	}
	else
		return;

	// get range info
	int iRangeIdx = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIdx, bHole);

	// get current image index.
	int iImgIdx = m_holeImg.GetCurrentImage();
	MJ_POSITION bmjPos = ImgIdxToMicroJointPos(iImgIdx, jointCount);

	// set micro joint position
	rangeInfo->SetMicroJointPos(bmjPos);

	CDialogEx::OnLButtonDown(nFlags, point);
}

void MicroJointAssignmentPage::OnKillFocusHolesSize()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	// get rangeinfo
	int iRangeIdx = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIdx, TRUE);

	// set the joint size
	if (rangeInfo->GetJointWidth() != m_dHoleJointPtSize.Get_Number())
	{
		rangeInfo->SetJointWidth(m_dHoleJointPtSize.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnKillFocusHolesSizeX()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	// get rangeinfo
	int iRangeIdx = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIdx, TRUE);

	if (rangeInfo->GetRangeX() != m_dHoleXSize.Get_Number())
	{
		rangeInfo->SetRangeX(m_dHoleXSize.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnKillFocusHolesSizeY()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	// get rangeinfo
	int iRangeIdx = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIdx, TRUE);

	if (rangeInfo->GetRangeY() != m_dHoleYSize.Get_Number())
	{
		rangeInfo->SetRangeY(m_dHoleYSize.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnKillFocusPerimeterSize()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	// get rangeinfo
	int iRangeIdx = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIdx, FALSE);

	// set the joint size.
	if (rangeInfo->GetJointWidth() != m_dPeriJointPtSize.Get_Number())
	{
		rangeInfo->SetJointWidth(m_dPeriJointPtSize.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnKillFocusPerimeterSizeX()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	// get rangeinfo
	int iRangeIdx = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIdx, FALSE);

	if (rangeInfo->GetRangeX() != m_dPeriXSize.Get_Number())
	{
		rangeInfo->SetRangeX(m_dPeriXSize.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnKillFocusPerimeterSizeY()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	// get rangeinfo
	int iRangeIdx = m_rangeTab.GetCurSel();
	MicroJointRangeInfoPtr rangeInfo = m_pMicroJointSetting->GetRangeInfo(iRangeIdx, FALSE);

	if (rangeInfo->GetRangeY() != m_dPeriYSize.Get_Number())
	{
		rangeInfo->SetRangeY(m_dPeriYSize.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnCheckEnableLeadIn()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	m_pMicroJointSetting->IsLeadIn(m_enableLeadIn);
	m_pMicroJointSetting->SetModified(TRUE);

	if (m_enableLeadIn)
	{
		m_dLeanInLen.EnableWindow(TRUE);
		m_dLeadInAngle.EnableWindow(TRUE);
	}
	else
	{
		m_dLeanInLen.EnableWindow(FALSE);
		m_dLeadInAngle.EnableWindow(FALSE);
	}
}

void MicroJointAssignmentPage::OnKillFocusLeadInLen()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	if (m_pMicroJointSetting->GetLeadInLen() != m_dLeanInLen.Get_Number())
	{
		m_pMicroJointSetting->SetLeadInLen(m_dLeanInLen.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnKillFocusLeadInAngle()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	if (m_pMicroJointSetting->GetLeadInAngle() != m_dLeadInAngle.Get_Number())
	{
		m_pMicroJointSetting->SetLeadInAngle(m_dLeadInAngle.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnCheckEnableLeadOut()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	m_pMicroJointSetting->IsLeadOut(m_enableLeadOut);
	m_pMicroJointSetting->SetModified(TRUE);

	if (m_enableLeadOut)
	{
		m_dLeanOutLen.EnableWindow(TRUE);
		m_dLeadOutAngle.EnableWindow(TRUE);
	}
	else
	{
		m_dLeanOutLen.EnableWindow(FALSE);
		m_dLeadOutAngle.EnableWindow(FALSE);
	}
}

void MicroJointAssignmentPage::OnKillFocusLeadOutLen()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	if (m_pMicroJointSetting->GetLeadOutLen() != m_dLeanOutLen.Get_Number())
	{
		m_pMicroJointSetting->SetLeadOutLen(m_dLeanOutLen.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::OnKillFocusLeadOutAngle()
{
	if (m_bDisKillFocusEnt)
		return;

	UpdateData(TRUE);

	if (m_pMicroJointSetting->GetLeadOutAngle() != m_dLeadOutAngle.Get_Number())
	{
		m_pMicroJointSetting->SetLeadOutAngle(m_dLeadOutAngle.Get_Number());
		m_pMicroJointSetting->SetModified(TRUE);
	}
}

void MicroJointAssignmentPage::DisplayMicroJointSetting(MicroJointConfigItemPtr pMicroJointConfigItem)
{
	m_pMicroJointSetting = pMicroJointConfigItem;

	/************************************************************************/
	//  init the content in the "range" group.

	// get the current MicroJointRange object.
	int iRangeIdx = m_rangeTab.GetCurSel();
	vector<MicroJointRangePtr>* pRangeList = pMicroJointConfigItem->GetRangeList();
	MicroJointRangePtr pMJRange = pRangeList->at(iRangeIdx);

	// init this micro joint range info.
	InitRange(pMJRange);
	/************************************************************************/

	m_dManualMJWid.Init(pMicroJointConfigItem->GetManualWidth(), DIGITAL_NUM, FALSE);

	/************************************************************************/
	// init lead in/out info.

	m_dLeanInLen.Init(pMicroJointConfigItem->GetLeadInLen(), DIGITAL_NUM, FALSE);
	m_dLeadInAngle.Init(pMicroJointConfigItem->GetLeadInAngle(), DIGITAL_NUM, FALSE);
	if (pMicroJointConfigItem->IsLeadIn())
	{
		m_dLeanInLen.EnableWindow(TRUE);
		m_dLeadInAngle.EnableWindow(TRUE);
	}
	else
	{
		m_dLeanInLen.EnableWindow(FALSE);
		m_dLeadInAngle.EnableWindow(FALSE);
	}

	m_dLeanOutLen.Init(pMicroJointConfigItem->GetLeadOutLen(), DIGITAL_NUM, FALSE);
	m_dLeadOutAngle.Init(pMicroJointConfigItem->GetLeadOutAngle(), DIGITAL_NUM, FALSE);
	if (pMicroJointConfigItem->IsLeadOut())
	{
		m_dLeanOutLen.EnableWindow(TRUE);
		m_dLeadOutAngle.EnableWindow(TRUE);
	}
	else
	{
		m_dLeanOutLen.EnableWindow(FALSE);
		m_dLeadOutAngle.EnableWindow(FALSE);
	}

	m_enableLeadIn = pMicroJointConfigItem->IsLeadIn();
	m_enableLeadOut = pMicroJointConfigItem->IsLeadOut();
	/************************************************************************/

	UpdateData(FALSE);
}

void MicroJointAssignmentPage::Init()
{
	/************************************************************************/
	// init the tab control.

	CString str;
	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RANGEONE);
	m_rangeTab.InsertItem(0, str);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RANGETWO);
	m_rangeTab.InsertItem(1, str);

	str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_RANGETHREE);
	m_rangeTab.InsertItem(2, str);
	/************************************************************************/

	LayoutControl();
}

void MicroJointAssignmentPage::LayoutControl()
{
	/************************************************************************/
	// ���Զ����䡱���顣

	AddAnchor(IDC_STATIC_AUTO_MJ, TOP_LEFT);
	AddAnchor(IDC_TAB_RANGE, TOP_LEFT);

	// �ڲ��������顣
	AddAnchor(IDC_GROUP_HOLE, TOP_LEFT);
	AddAnchor(IDC_STATIC_LOOP_RECT1, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_QTY, TOP_LEFT);
	AddAnchor(IDC_COMBO_HOLES_QUANTITY, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_SIZE, TOP_LEFT);
	AddAnchor(IDC_EDIT_HOLES_SIZE, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_X1, TOP_LEFT);
	AddAnchor(IDC_EDIT_HOLES_MAXSIZEX, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_Y1, TOP_LEFT);
	AddAnchor(IDC_EDIT_HOLES_MAXSIZEY, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_HOLE, TOP_LEFT);

	// �ⲿ�������顣
	AddAnchor(IDC_STATIC_BOUNDARY, TOP_LEFT);
	AddAnchor(IDC_STATIC_LOOP_RECT2, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_QTY2, TOP_LEFT);
	AddAnchor(IDC_COMBO_PERIMETER_QUANTITY, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_SIZE2, TOP_LEFT);
	AddAnchor(IDC_EDIT_PERIMETER_SIZE, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_X2, TOP_LEFT);
	AddAnchor(IDC_EDIT_PERIMETER_MAXSIZEX, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_Y2, TOP_LEFT);
	AddAnchor(IDC_EDIT_PERIMETER_MAXSIZEY, TOP_LEFT);
	//
	AddAnchor(IDC_STATIC_PERI, TOP_LEFT);
	/************************************************************************/

	// ���ֶ����䡱���顣
	AddAnchor(IDC_STATIC_M_ASSIGN, TOP_LEFT);
	AddAnchor(IDC_STATIC_M_MJ_SIZE, TOP_LEFT);
	AddAnchor(IDC_EDIT_M_MJ_SIZE, TOP_LEFT);

	/************************************************************************/
	// �����������ߡ����顣

	AddAnchor(IDC_STATIC_MJ_LEAD, TOP_LEFT);

	AddAnchor(IDC_CHECK_ENABLELEADIN, TOP_LEFT);
	AddAnchor(IDC_STATIC_LENGTH1, TOP_LEFT);
	AddAnchor(IDC_EDIT_LEADIN_LEN, TOP_LEFT);
	AddAnchor(IDC_STATIC_ANGLE1, TOP_LEFT);
	AddAnchor(IDC_EDIT_LEADIN_ANGLE, TOP_LEFT);

	AddAnchor(IDC_CHECK_ENABLELEADOUT, TOP_LEFT);
	AddAnchor(IDC_STATIC_LENGTH2, TOP_LEFT);
	AddAnchor(IDC_EDIT_LEADOUT_LEN, TOP_LEFT);
	AddAnchor(IDC_STATIC_ANGLE2, TOP_LEFT);
	AddAnchor(IDC_EDIT_LEADOUT_ANGLE, TOP_LEFT);
	/************************************************************************/
}

void MicroJointAssignmentPage::InitRange(MicroJointRangePtr range)
{
	/************************************************************************/
	// init "Hole" group

	// get joint point count
	MicroJointRangeInfoPtr rangeInfo = range->GetHoleRange();
	int iCount = rangeInfo->GetQuantity();

	// init the "Quantity" combo box.
	CString str;
	str.Format(_T("%d"), iCount);
	for (int i = 0; i < m_holeJointPtCount.GetCount(); i++)
	{
		CString tmpStr;
		m_holeJointPtCount.GetLBText(i, tmpStr);
		if (tmpStr.Compare(str) == 0)
		{
			m_holeJointPtCount.SetCurSel(i);
			break;
		}
	}

	// init joint size.
	m_dHoleJointPtSize.Init(rangeInfo->GetJointWidth(), DIGITAL_NUM, FALSE);

	// init "Maximum Overall" values
	m_dHoleXSize.Init(rangeInfo->GetRangeX(), DIGITAL_NUM, FALSE);
	m_dHoleYSize.Init(rangeInfo->GetRangeY(), DIGITAL_NUM, FALSE);

	// display the image in ImgStatic control.
	InitPosPreview(iCount, rangeInfo->GetMicroJointPos(), TRUE);
	/************************************************************************/

	/************************************************************************/
	// init "Perimeter" group

	// get joint point count
	MicroJointRangeInfoPtr rangeInfo1 = range->GetPerimeterRange();
	int iCount1 = rangeInfo1->GetQuantity();

	// init the "Quantity" combo box.
	CString str1;
	str1.Format(_T("%d"), iCount1);
	for (int j = 0; j < m_periJointPtCount.GetCount(); j++)
	{
		CString tmpStr1;
		m_periJointPtCount.GetLBText(j, tmpStr1);
		if (tmpStr1.Compare(str1) == 0)
		{
			m_periJointPtCount.SetCurSel(j);
			break;
		}
	}

	// init joint size.
	m_dPeriJointPtSize.Init(rangeInfo1->GetJointWidth(), DIGITAL_NUM, FALSE);

	// init "Maximum Overall" values
	m_dPeriXSize.Init(rangeInfo1->GetRangeX(), DIGITAL_NUM, FALSE);
	m_dPeriYSize.Init(rangeInfo1->GetRangeY(), DIGITAL_NUM, FALSE);

	// display the image in ImgStatic control.
	InitPosPreview(iCount1, rangeInfo1->GetMicroJointPos(), FALSE);
	/************************************************************************/
}

int MicroJointAssignmentPage::MicroJointPosToImgIdx(MJ_POSITION bmjPos)
{
	switch(bmjPos)
	{
	case ONE_BOTTOM:
		return 3;

	case ONE_RIGHT:
		return 0;

	case ONE_TOP:
		return 1;

	case ONE_LEFT:
		return 2;

	case TWO_VERTICAL:
		return 0;

	case TWO_HORIZONTAL:
		return 1;

	case THREE_BOTTOM:
		return 3;

	case THREE_RIGHT:
		return 0;

	case THREE_TOP:
		return 1;

	case THREE_LEFT:
		return 2;

	case FOUR_FULL:
		return 0;

	case SIX_DOWN:
		return 0;

	case SIX_UP:
		return 1;

	default:
		break;
	}

	// i think we cannot reach here.
	return 3;
}

MJ_POSITION MicroJointAssignmentPage::ImgIdxToMicroJointPos(int imgIdx, int iJointCount)
{
	switch(iJointCount)
	{
	case 1:
		{
			switch(imgIdx)
			{
			case 0:
				return ONE_RIGHT;
			case 1:
				return ONE_TOP;
			case 2:
				return ONE_LEFT;
			case 3:
				return ONE_BOTTOM;
			default:
				break;
			}
			break;
		}
	case 2:
		{
			switch(imgIdx)
			{
			case 0:
				return TWO_VERTICAL;
			case 1:
				return TWO_HORIZONTAL;
			default:
				break;
			}
			break;
		}
	case 3:
		{
			switch(imgIdx)
			{
			case 0:
				return THREE_RIGHT;
			case 1:
				return THREE_TOP;
			case 2:
				return THREE_LEFT;
			case 3:
				return THREE_BOTTOM;
			default:
				break;
			}
			break;
		}
	case 4:
		{
			switch(imgIdx)
			{
			case 0:
				return FOUR_FULL;
			default:
				break;
			}
			break;
		}
	case 6:
		{
			switch(imgIdx)
			{
			case 0:
				return SIX_DOWN;
			case 1:
				return SIX_UP;
			default:
				break;
			}
			break;
		}
	default:
		break;
	}

	// i think we cannot reach here.
	return ONE_RIGHT;
}

void MicroJointAssignmentPage::InitPosPreview(int iJointCount, MJ_POSITION bmjPos, BOOL bHole)
{
	// get the image index.
	int iImgIdx = MicroJointPosToImgIdx(bmjPos);

	int iImgCount = 0;
	// create the proper image list
	CImageList* tmpImgList;
	if (iJointCount == 0)
	{
		m_MJ0ImgList.DeleteImageList();
		m_MJ0ImgList.Create(IDB_MJ_QUANTITY0, 64, 1, RGB(0,0,0));
		tmpImgList = &m_MJ0ImgList;

		iImgCount = 1;
	}
	else if (iJointCount == 1)
	{
		m_MJ1ImgList.DeleteImageList();
		m_MJ1ImgList.Create(IDB_MJ_QUANTITY1, 64, 4, RGB(0,0,0));
		tmpImgList = &m_MJ1ImgList;

		iImgCount = 4;
	}
	else if (iJointCount == 2)
	{
		m_MJ2ImgList.DeleteImageList();
		m_MJ2ImgList.Create(IDB_MJ_QUANTITY2, 64, 2, RGB(0,0,0));
		tmpImgList = &m_MJ2ImgList;

		iImgCount = 2;
	}
	else if (iJointCount == 3)
	{
		m_MJ3ImgList.DeleteImageList();
		m_MJ3ImgList.Create(IDB_MJ_QUANTITY3, 64, 4, RGB(0,0,0));
		tmpImgList = &m_MJ3ImgList;

		iImgCount = 4;
	}
	else if (iJointCount == 4)
	{
		m_MJ4ImgList.DeleteImageList();
		m_MJ4ImgList.Create(IDB_MJ_QUANTITY4, 64, 1, RGB(0,0,0));
		tmpImgList = &m_MJ4ImgList;

		iImgCount = 1;

	}
	else if (iJointCount == 6)
	{
		m_MJ6ImgList.DeleteImageList();
		m_MJ6ImgList.Create(IDB_MJ_QUANTITY6, 64, 2, RGB(0,0,0));
		tmpImgList = &m_MJ6ImgList;

		iImgCount = 2;
	}

	// display the image in ImgStatic control.
	if (bHole)
	{
		m_holeImg.SetPicImageList(tmpImgList, iImgCount);
		m_holeImg.SetCurrentImage(iImgIdx);
	}
	else
	{
		m_periImg.SetPicImageList(tmpImgList, iImgCount);
		m_periImg.SetCurrentImage(iImgIdx);
	}
}

END_CUTLEADER_NAMESPACE()
