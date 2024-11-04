#pragma once

#include "clPartCommon.h"
#include "baseConst.h"
#include "ProductParam.h"
#include <vector>

DECLARE_CUTLEADER_CLASS(PatternList)
DECLARE_CUTLEADER_CLASS(PolygonPattern)
DECLARE_CUTLEADER_CLASS(PartCadData)
DECLARE_CUTLEADER_CLASS(ParamConfig)
DECLARE_CUTLEADER_CLASS(PartCamData)
DECLARE_CUTLEADER_CLASS(PartItem)
DECLARE_CUTLEADER_CLASS(Part)
DECLARE_CUTLEADER_CLASS(PartTopologyItem)
DECLARE_CUTLEADER_CLASS(PartTopologyItemList)
DECLARE_CUTLEADER_CLASS(LoopTopologyItem)
DECLARE_CUTLEADER_CLASS(LoopTopologyItemList)

BEGIN_CUTLEADER_NAMESPACE()

// this class will build the part.
class ClPart_Export PartBuilder
{
public:
	/************************************************************************/
	// 有关构建。

	// 利用从外部导入的图形集合来构建零件对象。
	// notes:
	//   1) we should use default part scheme to generate the cam data.
	//   2) cache data in part should be updated.
	static PartPtr BuildExtPart(CString strName, PatternListPtr pPatList_notPolygon, PatternListPtr pPatList_polygon, LONGLONG iParamConfigID = INVALID_ID);

	// 利用“零件cad数据”和零件方案来构建零件对象。
	/* 参数：
	*  bCheckAssignAll: 检查如果图形量大于一个值时，省略一些特征，这一般用于导入复杂零件时。
	*/
	static PartPtr BuildPart(CString strName, PartCadDataPtr pPartCadData, LONGLONG iParamConfigID, BOOL bCheckAssignAll = FALSE);
    static PartPtr BuildPart(CString strName, PartCadDataPtr pPartCadData, const ParamConfig* pParamConfig, BOOL bCheckAssignAll = FALSE);

	// 利用从外部导入的图形集合来构建PartCadData对象。
	/* 参数：
	*  bProcessStd: 合并为标准图形。
	*  bProcessGrid: 合并为阵列图形。
	*/
	// 注：
	//   1) 需要根据系统设置执行图形处理，优化等动作。
	//   2) 送进来的pattern的缓存是更新过的，这个函数结束时需要更新缓冲数据。
	static PartCadDataPtr BuildPartCadData(PatternListPtr pPatList_notPolygon, PatternListPtr pPatList_polygon, BOOL bProcessStd, BOOL bProcessGrid);

	// 利用“零件cad数据”和零件方案来构建“零件cam数据”。
	/* 参数：
	*  bCheckAssignAll: 检查如果图形量大于一个值时，省略一些特征，这一般用于导入复杂零件时。
	*/
	static PartCamDataPtr BuildPartCamData(PartCadDataPtr pPartCadData, LONGLONG iParamConfigID, BOOL bCheckAssignAll = FALSE);
    static PartCamDataPtr BuildPartCamData(PartCadDataPtr pPartCadData, const ParamConfig* pParamConfig, BOOL bCheckAssignAll = FALSE);

	// 创建矩形/圆形零件。
	static PartPtr BuildRectPart(CString strName, double dWidth, double dHeight, LONGLONG iParamConfigID);
	static PartPtr BuildRoundPart(CString strName, double dRad, LONGLONG iParamConfigID);
	/************************************************************************/

	/************************************************************************/
	// 有关克隆。

	// 克隆零件。
	// 注:
	//   1) 新零件的内存和老零件是独立的。
	//   2) 需要处理好数据之间的引用关系。
	//   3) 这个函数不更新缓存。
	static PartPtr ClonePart(const Part* pOldPart);

	// 克隆“零件项”。
	// 注:
	//   1) 新“零件项”的内存和老“零件项”是独立的。
	//   2) the new PartItem will belong to a new part, so we should generate the Part ID for the new PartItem in this function.
	//   3) 这个函数不更新缓存。
	static PartItemPtr ClonePartItem(const PartItem* pOldPartItem);

	// 克隆CAD数据。
	// 注:
	//   1) 新CAD数据的内存和老CAD数据是独立的。
	//   2) 需要处理好数据之间的引用关系。
	//   3) 这个函数不更新缓存。
	static PartCadDataPtr CloneCadData(const PartCadData* pOldCadData);

	// 克隆CAM数据。
	// 注:
	//   1) 新CAM数据的内存和老CAM数据是独立的。
	//   2) 只有当同时也克隆CAD数据时才调用这个函数, pOldCadData/pNewCadData是克隆前/后的CAD数据。
	//   3) 需要处理好数据之间的引用关系。
	//   4) 这个函数不更新缓存。
	static PartCamDataPtr CloneCamData(const PartCamData* pOldCamData, const PartCadData* pOldCadData, PartCadDataPtr pNewCadData);
	/************************************************************************/

	// 从回路拓扑结构整理出零件拓扑结构。
	// 注：
	//   1) 零件必须有一个封闭的外围回路。
	static PartTopologyItemListPtr CalcPartTopStruct(const LoopTopologyItemList* pLoopTopItemList);

private:
	// 从一个“回路拓扑项”往下递归，求出所有“零件拓扑项”。
	// 注：
	//   1) "pPartTopItem"所代表零件的外围回路已经被"pLoopTopItem"所代表回路初始化了。
	static void SearchLoopTopItem(LoopTopologyItemPtr pLoopTopItem, PartTopologyItemPtr pPartTopItem);

    // 是否需要对图形做优化
	static bool NeedOptimization(const std::vector<PolygonPatternPtr>& polygonPatList, UINT iOptimizeLimit);
};

END_CUTLEADER_NAMESPACE()
