#include "StdAfx.h"
#include "LabelDrawStatic.h"

#include "ReportTplPage.h"
#include "LabelTplEntity.h"

BEGIN_CUTLEADER_NAMESPACE()

IMPLEMENT_DYNAMIC(LabelDrawStatic, CStatic)

LabelDrawStatic::LabelDrawStatic(void)
{
}

LabelDrawStatic::~LabelDrawStatic(void)
{
}

BEGIN_MESSAGE_MAP(LabelDrawStatic, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void LabelDrawStatic::OnPaint()
{
	CDC* pDC = GetDC();
	pDC->SelectStockObject(NULL_BRUSH);

	// ҳ���ڻ�ͼ����λ�á�
	double dScale = .0;
	CRect pageRect = GetPageRect(dScale);
	pDC->Rectangle(pageRect);

	/************************************************************************/
	// ����ÿ����ǩ��

	// ҳ��ȥ���ıߵı߾ࡣ
	CRect labelRegion = pageRect;
	int iLeftMargin = (int)(m_pReportTplPage->GetLeftMargin()*dScale);
	int iRightMargin = (int)(m_pReportTplPage->GetRightMargin()*dScale);
	int iTopMargin = (int)(m_pReportTplPage->GetTopMargin()*dScale);
	int iBottomMargin = (int)(m_pReportTplPage->GetTopMargin()*dScale);
	labelRegion.left += iLeftMargin;
	labelRegion.right -= iRightMargin;
	labelRegion.top += iTopMargin;
	labelRegion.bottom -= iBottomMargin;

	// ���Ƹ�����ǩ��
	CRect labelRect = m_pLabelTplEntity->GetBorderRect();
	int iLabelWidth = (int)(labelRect.Width()*dScale);
	int iLabelHeight = (int)(labelRect.Height()*dScale);
	int iRowGap = (int)(m_pLabelTplEntity->GetRowGap()*dScale);
	int iColumnGap = (int)(m_pLabelTplEntity->GetColumnGap()*dScale);
	for (int i = 0; i < m_pLabelTplEntity->GetRowNum(); i++)
	{
		for (int j = 0; j < m_pLabelTplEntity->GetColumnNum(); j++)
		{
			CRect labelRect;
			labelRect.left = labelRegion.left + j*(iLabelWidth+iColumnGap);
			labelRect.right = labelRect.left + iLabelWidth;
			labelRect.top = labelRegion.top + i*(iLabelHeight+iRowGap);
			labelRect.bottom = labelRect.top + iLabelHeight;
			pDC->Rectangle(labelRect);
		}
	}
	/************************************************************************/

	// �ͷ��豸�����ġ�
	ReleaseDC(pDC);
}

void LabelDrawStatic::Init(ReportTplPagePtr pReportTplPage, LabelTplEntityPtr pLabelTplEntity)
{
	m_pReportTplPage = pReportTplPage;
	m_pLabelTplEntity = pLabelTplEntity;
}

CRect LabelDrawStatic::GetPageRect(double& dScale)
{
	CRect pageRect;

	// ��ͼ��λ�á�
	CRect clientRect;
	GetClientRect(&clientRect);
	int iClientWidth = clientRect.Width();
	int iClientHeight = clientRect.Height();

	// ҳ��λ�á�
	int iPageWidth = m_pReportTplPage->GetPaperWidth();
	int iPageHeight = m_pReportTplPage->GetPaperHeight();
	if ((double)iPageWidth/(double)iPageHeight < (double)iClientHeight/(double)iClientWidth) // ���¶�ס��
	{
		dScale = (double)iClientHeight/(double)iPageHeight;
		pageRect.left = (int)((iClientWidth - iPageWidth*dScale)/2);
		pageRect.top = 0;
		pageRect.right = (int)(pageRect.left + iPageWidth*dScale);
		pageRect.bottom = (int)(pageRect.top + iPageHeight*dScale);
	}
	else // ���Ҷ�ס��
	{
		dScale = (double)iClientWidth/(double)iPageWidth;
		pageRect.left = 0;
		pageRect.top = (int)((iClientHeight - iPageHeight*dScale)/2);
		pageRect.right = (int)(pageRect.left + iPageWidth*dScale);
		pageRect.bottom = (int)(pageRect.top + iPageHeight*dScale);
	}

	return pageRect;
}

END_CUTLEADER_NAMESPACE()