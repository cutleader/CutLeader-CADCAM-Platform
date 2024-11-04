#pragma once

#include "clSheetCommon.h"
#include "DataProperty.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(DataItem)
DECLARE_CUTLEADER_CLASS(FolderNode)
DECLARE_CUTLEADER_CLASS(ImgData)
DECLARE_CUTLEADER_CLASS(Sheet)

BEGIN_CUTLEADER_NAMESPACE()

// use this class to manager the sheet.
class ClSheet_Export SheetManager
{
public:
	// create a sheet.
	// notes:
	//   1) we should not consume matSize in this function, do it when save the sheet into database.
	static SheetPtr CreateSheet(CString strSheetName, LONGLONG iMaterialSizeID, LONGLONG iSchemeInfoID);

	// delete a sheet.
	// notes:
	//   1) when delete a sheet, caller should check whether the sheet related to a nest job, if YES, "iSheetCount" will be set correctly.
	//   2) when deleting sheet, we should update mat db and main db.
	/* params:
	*  iSheetCount: the sheet count.
	*/
	static BOOL DeleteSheet(LONGLONG iSheetID, int iSheetCount = 1, BOOL bTransaction = TRUE);


	/************************************************************************/
	// �����ġ�

	// save the new sheet.
	/* params:
	*  bTransaction: whatever this value is, the sheet will always be saved.
	*/
	// notes:
	//   1) this function also save data item and return it.
	//   2) this function should update "pParentFolder".
	//   3) this function should update the matLib if the matLib of "pSheet" is not lost.
	//   4) this function should update the used part in main db.
	static DataItemPtr SaveNewSheet(SheetPtr pSheet, int iSheetCount, FolderNodePtr pParentFolder, BOOL bTransaction = TRUE);

	// save the existing sheet.
	// notes:
	//   1) this function should update the modify date.
	//   2) this function should update the used part in main db.
	static void SaveExistSheet(SheetPtr pSheet);

	// import "pSheet" into database and append it to its parent FolderNode object.
	/* params:
	*  prtExpInfoList: the info of the involved part items.
	*/
	// notes:
	//   1) we will create a folder to hold the sheet and the relative parts.
	//   2) ����ǰ�ȿ�¡һ�ݰ�ĺ������
	static FolderNodePtr ImportSheet(const Sheet* pSheet, FolderNodePtr pParentFolder, std::vector<DataProperty> prtExpInfoList);
	/************************************************************************/


	/************************************************************************/
	// ��¡��ġ�

	// clone sheet.
	// notes:
	//   1) before clone the sheet, we should make sure material count is enough, you know, the cloned sheet will consume same material with old one. 
	//   2) we should not consume matSize in this function, do it when save the sheet into database.
	//   3) do not clone the part which is used by the sheet, sequence data will also be cloned. do not clone "Sheet::m_pSkeleMatSize".
	//   4) cache data of returned sheet is not updated.
	static SheetPtr CloneSheet(const Sheet* pSheet);

	// ��¡��ġ�
	// ע:
	//   1) the difference with "CloneSheet" is that this function will also
	//      clone the PartItems within the sheet, so we should update many relative data.
	static SheetPtr CloneSheetEx(const Sheet* pSheet);
	/************************************************************************/


	// ��֤����Ƿ���Ч��
	// ע��:
	//   1) ��������һ���������Ϊ�����Ч����Ҫ�޸���
	//      a) û�ж�Ӧ�Ĳ��Ͽ⡣
	//      b) û�ж�Ӧ��ר�ҿ⡣
	static BOOL VerifySheet(LONGLONG iSheetID);

	// �������ĵ�Ԥ�����ݡ�
	// ע��
	//   1) ��������û��ͼ�Σ����ؿ�ֵ��
	static ImgDataPtr CalcPreviewData(SheetPtr pSheet);
};

END_CUTLEADER_NAMESPACE()
