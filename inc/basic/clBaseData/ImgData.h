#pragma once

#include "clBaseDataCommon.h"

DECLARE_CUTLEADER_CLASS(ImgData)

BEGIN_CUTLEADER_NAMESPACE()

// ��դͼ����
class ClBaseData_Export ImgData
{
public:
	ImgData(void);
	~ImgData(void);

public: // get/set functions.
	void SetData(LPVOID pData) { m_pData = pData; }
	LPVOID GetData() { return m_pData; }

	void SetSize(int iSize) { m_iSize = iSize; }
	int GetSize() { return m_iSize; }

	void SetExt(CString strImgExt) { m_strImgExt = strImgExt; }
	CString GetExt() { return m_strImgExt; }

public:
	// �ڴ����ϻ���ͼƬ��
	// ע�⣺
	//   1) ͼ������λ��ʱ���ð�ɫ��䡣
	//   2) ͼ��������ڡ�
	static void DrawImgData(ImgDataPtr pImgData, CWnd* pWnd);

private:
	// the data in the image.
	LPVOID m_pData;

	// the image size.
	int m_iSize;

	// the image ext.
	// notes:
	//   1) do not have "."
	CString m_strImgExt;
};

END_CUTLEADER_NAMESPACE()
