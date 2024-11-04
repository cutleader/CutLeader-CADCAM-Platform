#include "StdAfx.h"
#include "RemnantMatInfoDlg.h"

#include "baseConst.h"
#include "LineArc2DList.h"
#include "DataBaseManager.h"
#include "Polygon2DList.h"

#include "Sheet.h"
#include "SheetProcessor.h"
#include "clSheetResource.h"

#include "PolygonMaterial.h"
#include "MaterialLibLoader.h"
#include "MaterialLibWriter.h"
#include "MaterialProcessor.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(RemnantMatInfoDlg, CDialogEx)

RemnantMatInfoDlg::RemnantMatInfoDlg(CWnd* pParent)
		: CLDialog(RemnantMatInfoDlg::IDD, pParent)
{
	m_strMsg = MultiLanguageMgr::GetInstance()->TranslateString(IDS_MSG_INVALID_REGION);
	m_bEnableMerge = FALSE;
}

RemnantMatInfoDlg::~RemnantMatInfoDlg(void)
{
}

void RemnantMatInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_MSG, m_strMsg);
	DDX_Control(pDX, IDC_STATIC_PREVIEW_MAT, m_ctrlMatPreview);
	DDX_Control(pDX, IDC_EDIT_INV_COUNT, m_ctrlRegionCount);

	DDX_Check(pDX, IDC_CHECK_MERGE, m_bEnableMerge);
	DDX_Control(pDX, IDC_EDIT_MERGE_DIS, m_ctrlMergeDis);
}

BOOL RemnantMatInfoDlg::OnInitDialog()
{
	CLDialog::OnInitDialog();

	// 有关合并。
	m_ctrlMergeDis.Init(10.0, 4, false);
	m_ctrlMergeDis.EnableWindow(FALSE);
	((CWnd*)GetDlgItem(IDC_BTN_MERGE))->EnableWindow(FALSE);

	// 材料的边框。
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, m_pSheet->GetMaterialSizeID());
	m_pOuterLoop = MaterialProcessor::GetBorderLineArcs(pMaterialSize.get());

	// 板材的未合并的无效区域
	m_uselessRegion = SheetProcessor::CalcUselessRegionsA(m_pSheet);

	// 得到经过合并后的无效区域。
	m_mergeUselessRegion = SheetProcessor::MergedUselessRegion(m_pSheet, m_ctrlMergeDis.Get_Number());

	// 无效区域个数。
	m_ctrlRegionCount.Init(m_uselessRegion.size(), 0, false);

	PreviewMat();

	SetBackgroundColor(UI_COLOR);
	return TRUE;
}

BEGIN_MESSAGE_MAP(RemnantMatInfoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()

	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()

	ON_BN_CLICKED(IDC_CHECK_MERGE, OnCheckMerge)
	ON_BN_CLICKED(IDC_BTN_MERGE, OnMerge)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnSaveRemnant)
END_MESSAGE_MAP()

void RemnantMatInfoDlg::OnPaint()
{
	m_ctrlMatPreview.Invalidate(TRUE);

	CDialogEx::OnPaint();
}

void RemnantMatInfoDlg::OnDestroy()
{
	// 释放视口。
	m_ctrlMatPreview.ReleaseViewPort();

	CDialogEx::OnDestroy();
}

HBRUSH RemnantMatInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	TCHAR classname[MAX_PATH];
	if (::GetClassName(pWnd->m_hWnd, classname, MAX_PATH) == 0)
		return hbr;

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

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW_MAT)
		return hbr;

	pDC->SetBkColor(UI_COLOR);

	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);

	return (HBRUSH) m_brush;
}

BOOL RemnantMatInfoDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect r;
	GetClientRect(&r);
	if ((HBRUSH)m_brush == NULL)
		m_brush.CreateSolidBrush(UI_COLOR);
	pDC->FillRect(&r, &m_brush);

	return TRUE;
}

BOOL RemnantMatInfoDlg::OnMouseWheel(UINT nFlags, short iDelta, CPoint pt)
{
	// the rect of the polygon view window.
	CRect polyWndRect;
	m_ctrlMatPreview.GetWindowRect(&polyWndRect);

	// check whether the current cursor is on the material view window.
	if (polyWndRect.PtInRect(pt))
		m_ctrlMatPreview.ZoomViewPort(iDelta, pt);

	return TRUE;
}

void RemnantMatInfoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// check whether the current cursor is on the material view window.
	CRect polyWndRect;
	m_ctrlMatPreview.GetWindowRect(&polyWndRect);
	ScreenToClient(&polyWndRect);
	if (polyWndRect.PtInRect(point))
	{
		m_ctrlMatPreview.SetFocus();

		if (nFlags & MK_MBUTTON)
		{
			CPoint ptOffSet = point - m_ctrlMatPreview.GetPanReferPt();
			m_ctrlMatPreview.PanViewPort(ptOffSet.x, ptOffSet.y);
			m_ctrlMatPreview.SetPanReferPt(point);
		}
	}
}

void RemnantMatInfoDlg::OnMButtonDown(UINT nFlags, CPoint point)
{
	m_ctrlMatPreview.SetPanReferPt(point);
}

void RemnantMatInfoDlg::OnCheckMerge()
{
	UpdateData(TRUE);

	if (m_bEnableMerge)
	{
		// 得到经过合并后的无效区域。
		m_mergeUselessRegion = SheetProcessor::MergedUselessRegion(m_pSheet, m_ctrlMergeDis.Get_Number());

		// 无效区域个数。
		m_ctrlRegionCount.Init(m_mergeUselessRegion.size(), 0, false);

		m_ctrlMergeDis.EnableWindow(TRUE);
		((CWnd*)GetDlgItem(IDC_BTN_MERGE))->EnableWindow(TRUE);
	}
	else
	{
		// 无效区域个数。
		m_ctrlRegionCount.Init(m_uselessRegion.size(), 0, false);

		m_ctrlMergeDis.EnableWindow(FALSE);
		((CWnd*)GetDlgItem(IDC_BTN_MERGE))->EnableWindow(FALSE);
	}

	PreviewMat();
}

void RemnantMatInfoDlg::OnMerge()
{
	// 得到经过合并后的无效区域。
	m_mergeUselessRegion = SheetProcessor::MergedUselessRegion(m_pSheet, m_ctrlMergeDis.Get_Number());

	// 无效区域个数。
	m_ctrlRegionCount.Init(m_mergeUselessRegion.size(), 0, false);

	PreviewMat();
}

void RemnantMatInfoDlg::OnSaveRemnant()
{
	// 创建余料。
	DBConnectPtr pMatLibDBCon = DataBaseManager::GetInstance()->GetMatLibDBConnect();
	IMaterialSizePtr pMaterialSize = MaterialLibLoader::LoadMatSize(pMatLibDBCon, m_pSheet->GetMaterialSizeID());
	PolygonMaterialPtr pPolyMat(new PolygonMaterial(pMaterialSize->GetParentID(), pMaterialSize->GetName(), TRUE, pMaterialSize->GetWidth(), pMaterialSize->GetHeight(), 1, 1));
	pPolyMat->SetOuterLoop(m_pOuterLoop);
	if (m_bEnableMerge)
		pPolyMat->SetUselessRegion(m_mergeUselessRegion);
	else
		pPolyMat->SetUselessRegion(m_uselessRegion);

	// 保存余料。
	MaterialLibWriter::SaveMatSize(pMatLibDBCon, pPolyMat, TRUE);

	EndDialog(IDC_BTN_SAVE);
}

void RemnantMatInfoDlg::Init(SheetPtr pSheet)
{
	m_pSheet = pSheet;
}

void RemnantMatInfoDlg::PreviewMat()
{
	// 初始化视口。
	Rect2D geomRect = m_pOuterLoop->GetRectBox();
	m_ctrlMatPreview.InitViewPort(geomRect);

	// 材料外边框。
	m_ctrlMatPreview.SetSolidGeom2DList(m_pOuterLoop);

	// 材料的无效区域。
	LineArc2DListPtr pUselessRegion(new LineArc2DList);
	if (!m_bEnableMerge)
	{
		for (unsigned int i = 0; i < m_uselessRegion.size(); i++)
		{
			LineArc2DListPtr pGeom2DList = m_uselessRegion.at(i);
			pUselessRegion->insert(pUselessRegion->end(), pGeom2DList->begin(), pGeom2DList->end());
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_mergeUselessRegion.size(); i++)
		{
			LineArc2DListPtr pGeom2DList = m_mergeUselessRegion.at(i);
			pUselessRegion->insert(pUselessRegion->end(), pGeom2DList->begin(), pGeom2DList->end());
		}
	}
	m_ctrlMatPreview.SetDashGeom2DList(pUselessRegion);

	// 绘制。
	m_ctrlMatPreview.Invalidate();
}

END_CUTLEADER_NAMESPACE()
