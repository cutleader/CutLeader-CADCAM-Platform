#pragma once

#include "clSheetCommon.h"
#include "BusinessData.h"
#include "Point2D.h"
#include "boost/enable_shared_from_this.hpp"


DECLARE_CUTLEADER_CLASS(LineArc2DList)
DECLARE_CUTLEADER_CLASS(ImgData)
DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(PartPlacementList)
DECLARE_CUTLEADER_CLASS(SheetSequenceData)
DECLARE_CUTLEADER_CLASS(SheetCache)
DECLARE_CUTLEADER_CLASS(PartGroupList)


BEGIN_CUTLEADER_NAMESPACE()

// this class define the Sheet.
// notes:
//   1) useless region: the useless region of the sheet.
//      please refer to the following illustration, after the nesting1, you can see we got sheet1 and also created a skeleton matsize: matsize2,
//      after nesting2, we got sheet2, you know, sheet2 are relative to "matsize2". So, when display sheet2, we should highlight region:r1, so user will
//      be clear that region r1 in sheet2 is useless.
/*
        after nesting1:
		  +--------------------+             +--------------------+   
		  |                    |             |                +-+ |   
		  |                    |             |                | | |   
		  |                    |             |                +-+ |   
		  |                    |             |                    |   
		  |                    |             |                    |   
		  |                    |             |                    |   
		  |                    |             |                    |   
		  |                    |             |                    |   
		  +--------------------+             +--------------------+   
		        matsize1                            sheet1                                                                              
		        
		after nesting2:
		  +--------------------+             +--------------------+   
		  |                +-+ |             |       +------+ +-+ |   
		  |                | | |             |       |      | | | |   
		  |                +-+ |             |       +------+ +-+ |   
		  |                 r1 |             |                 r1 |   
		  |                    |             |                    |   
		  |                    |             |                    |   
		  |                    |             |                    |   
		  |                    |             |                    |   
		  +--------------------+             +--------------------+   
		        matsize2                            sheet2  
*/
//   2) there are two ways to get a sheet:
//        a) create a sheet.
//        b) build sheet from nesting.
//   3) how to understand sheet/material size: 
//        a) material size and sheet are "physical" objects, not like part, part is an abstract object.
//        b) material size is a material that we can generate sheet from it. their relation is similar with "stock and product".
//   4) all PartItems which will be added into a sheet will reference the same Expert Item, so we cannot add "brother" PartItems into the same sheet.

class ClSheet_Export Sheet : public BusinessData,
							 public boost::enable_shared_from_this<Sheet>
{
public:
	Sheet(void);
	Sheet(CString strName, CString strComment);
	Sheet(LONGLONG iSheetID, CString strName, CString strComment);
	virtual ~Sheet(void);

public: // implement IData interface.
	virtual BOOL IsModified() const override;
    virtual LONGLONG GetLatestModifyTimeStamp() const override;

public: // implement IBusinessData interface
	virtual BUSINESS_DATA_TYPE GetBusinessDataType() const override { return BUSINESS_DATA_SHEET; }

public: // get/set functions.
	PartPlacementListPtr GetPartPlacements() const { return m_pPartPmtList; }
	void SetPartPlacements(PartPlacementListPtr pPartPlacements) { m_pPartPmtList = pPartPlacements; }

	PartInstanceListPtr GetPartInstances() const { return m_pPartInstList; }
	void SetPartInsts(PartInstanceListPtr pPartInstances) { m_pPartInstList = pPartInstances; }

    PartGroupListPtr GetPartGroupList() const { return m_pPartGroupList; }
    void SetPartGroupList(PartGroupListPtr pPartGroupList) { m_pPartGroupList = pPartGroupList; }

	void SetMaterialSizeID(LONGLONG iMaterialSizeID) { m_iMaterialSizeID = iMaterialSizeID; }
	LONGLONG GetMaterialSizeID() const { return m_iMaterialSizeID; }

	// about the sequence.
	SheetSequenceDataPtr GetSheetSequenceData() const { return m_pSheetSequenceData; }
	void SetSheetSequenceData(SheetSequenceDataPtr pSheetSequenceData) { m_pSheetSequenceData = pSheetSequenceData; }

	void SetParamConfigID(LONGLONG iParamConfigID) { m_iParamConfigID = iParamConfigID; }
	LONGLONG GetParamConfigID() const { return m_iParamConfigID; }

	void SetSheetCache(SheetCachePtr pSheetCache) { m_pSheetCache = pSheetCache; }
	SheetCachePtr GetSheetCache() const { return m_pSheetCache; }

	void SetPreviewData(ImgDataPtr pPreviewData) { m_pPreviewData = pPreviewData; }
	ImgDataPtr GetPreviewData() const { return m_pPreviewData; }

public: // other functions.
	// calc the base geoms of the sheet.
	// notes: 
	//   1) only return the geometry data, do not include lead in/out, useless Region,....
	LineArc2DListPtr Calculate2DLineArcs() const;

	// get the inst count of the part in this sheet.
	int GetPartInstanceCount(LONGLONG iPartID) const;

	// 得到材料利用率。
	double CalcMaterialUtilization() const;

private:
	void CommonConstruct();

private:
	/************************************************************************/
	// part involved in sheet.

	// all part placements in the sheet.
	PartPlacementListPtr m_pPartPmtList;

	// all part instances in the sheet.
	PartInstanceListPtr m_pPartInstList;
	/************************************************************************/


	/************************************************************************/
	// configuration info.

	// the material size which produce this sheet.
	// notes:
	//   1) after deleting the sheet, we should return the material size.
	//   2) should not be INVALID_ID.
	LONGLONG m_iMaterialSizeID;

	// the scheme info used by this sheet.
	LONGLONG m_iParamConfigID;
	/************************************************************************/


    // 零件分组
    PartGroupListPtr m_pPartGroupList;

	// record the sequence units of the sheet.
	// notes: 
	//   1) these data should be saved into database.
	SheetSequenceDataPtr m_pSheetSequenceData;

	// the cache data of sheet.
	SheetCachePtr m_pSheetCache;

	// 零件的预览图片。
	// 注：
	//   1) 这个变量只在从数据库加载板材时初始化，不会实时更新。
	//   2) 在保存板材时，计算出预览数据存入数据库。
	ImgDataPtr m_pPreviewData;
};

END_CUTLEADER_NAMESPACE()
