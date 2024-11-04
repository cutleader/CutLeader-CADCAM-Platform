#pragma once

#include "clReportTemplateCommon.h"
#include "taoResource.h"

DECLARE_CUTLEADER_CLASS(ReportTplPage)
DECLARE_CUTLEADER_CLASS(LabelTplEntity)

BEGIN_CUTLEADER_NAMESPACE()

// Ԥ�ϱ�ǩ�Ĵ��ڡ�
class LabelDrawStatic : public CStatic
{
	DECLARE_DYNAMIC(LabelDrawStatic)

public:
	LabelDrawStatic(void);
	~LabelDrawStatic(void);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnPaint();

public:
	void Init(ReportTplPagePtr pReportTplPage, LabelTplEntityPtr pLabelTplEntity);

private:
	// �õ�ҳ���ڻ�ͼ����λ�á�
	/* ������
	*  dScale: Ϊ����ʾ�ڻ�ͼ����ҳ��Ҫ�������š�
	*/
	CRect GetPageRect(double& dScale);

private:
	ReportTplPagePtr m_pReportTplPage;
	LabelTplEntityPtr m_pLabelTplEntity;
};

END_CUTLEADER_NAMESPACE()
