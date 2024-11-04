#pragma once

#include "clBaseDataCommon.h"

DECLARE_CUTLEADER_CLASS(ImgData)

BEGIN_CUTLEADER_NAMESPACE()

// 光栅图对象。
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
	// 在窗口上画出图片。
	// 注意：
	//   1) 图像数据位空时，用白色填充。
	//   2) 图像充满窗口。
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
