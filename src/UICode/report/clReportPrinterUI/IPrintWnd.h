#pragma once

#include "clReportPrinterUICommon.h"

BEGIN_CUTLEADER_NAMESPACE()

// ֧�ִ�ӡ�Ĵ��ڡ�
class ClReportPrinterUI_Export IPrintWnd
{
public:
	/************************************************************************/	
	// ��ӡ��ء�

	// ׼����ӡ��
	virtual void PreparePrinting(CPrintInfo* pInfo) = 0;

	// ��ʼ��ӡ��
	virtual void BeginPrinting(CDC* pDC, CPrintInfo* pInfo) = 0;

	// ׼���豸������
	virtual void PrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL) = 0;

	// ��ӡһҳ��
	virtual void Print(CDC* pDC, CPrintInfo* pInfo) = 0;

	// ������ӡ��
	virtual void EndPrinting(CDC* pDC, CPrintInfo* pInfo) = 0;

	// Ԥ���ʹ�ӡ���ݡ�
	virtual void PreviewData() = 0;
	virtual void PrintData() = 0;
	/************************************************************************/

	// ������ء�
	virtual CString GetWndText() = 0;
	virtual void SetWndFocus() = 0;
};

END_CUTLEADER_NAMESPACE()
