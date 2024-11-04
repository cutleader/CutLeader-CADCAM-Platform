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
	// �йع�����

	// ���ô��ⲿ�����ͼ�μ����������������
	// notes:
	//   1) we should use default part scheme to generate the cam data.
	//   2) cache data in part should be updated.
	static PartPtr BuildExtPart(CString strName, PatternListPtr pPatList_notPolygon, PatternListPtr pPatList_polygon, LONGLONG iParamConfigID = INVALID_ID);

	// ���á����cad���ݡ�����������������������
	/* ������
	*  bCheckAssignAll: ������ͼ��������һ��ֵʱ��ʡ��һЩ��������һ�����ڵ��븴�����ʱ��
	*/
	static PartPtr BuildPart(CString strName, PartCadDataPtr pPartCadData, LONGLONG iParamConfigID, BOOL bCheckAssignAll = FALSE);
    static PartPtr BuildPart(CString strName, PartCadDataPtr pPartCadData, const ParamConfig* pParamConfig, BOOL bCheckAssignAll = FALSE);

	// ���ô��ⲿ�����ͼ�μ���������PartCadData����
	/* ������
	*  bProcessStd: �ϲ�Ϊ��׼ͼ�Ρ�
	*  bProcessGrid: �ϲ�Ϊ����ͼ�Ρ�
	*/
	// ע��
	//   1) ��Ҫ����ϵͳ����ִ��ͼ�δ����Ż��ȶ�����
	//   2) �ͽ�����pattern�Ļ����Ǹ��¹��ģ������������ʱ��Ҫ���»������ݡ�
	static PartCadDataPtr BuildPartCadData(PatternListPtr pPatList_notPolygon, PatternListPtr pPatList_polygon, BOOL bProcessStd, BOOL bProcessGrid);

	// ���á����cad���ݡ���������������������cam���ݡ���
	/* ������
	*  bCheckAssignAll: ������ͼ��������һ��ֵʱ��ʡ��һЩ��������һ�����ڵ��븴�����ʱ��
	*/
	static PartCamDataPtr BuildPartCamData(PartCadDataPtr pPartCadData, LONGLONG iParamConfigID, BOOL bCheckAssignAll = FALSE);
    static PartCamDataPtr BuildPartCamData(PartCadDataPtr pPartCadData, const ParamConfig* pParamConfig, BOOL bCheckAssignAll = FALSE);

	// ��������/Բ�������
	static PartPtr BuildRectPart(CString strName, double dWidth, double dHeight, LONGLONG iParamConfigID);
	static PartPtr BuildRoundPart(CString strName, double dRad, LONGLONG iParamConfigID);
	/************************************************************************/

	/************************************************************************/
	// �йؿ�¡��

	// ��¡�����
	// ע:
	//   1) ��������ڴ��������Ƕ����ġ�
	//   2) ��Ҫ���������֮������ù�ϵ��
	//   3) ������������»��档
	static PartPtr ClonePart(const Part* pOldPart);

	// ��¡��������
	// ע:
	//   1) �¡��������ڴ���ϡ������Ƕ����ġ�
	//   2) the new PartItem will belong to a new part, so we should generate the Part ID for the new PartItem in this function.
	//   3) ������������»��档
	static PartItemPtr ClonePartItem(const PartItem* pOldPartItem);

	// ��¡CAD���ݡ�
	// ע:
	//   1) ��CAD���ݵ��ڴ����CAD�����Ƕ����ġ�
	//   2) ��Ҫ���������֮������ù�ϵ��
	//   3) ������������»��档
	static PartCadDataPtr CloneCadData(const PartCadData* pOldCadData);

	// ��¡CAM���ݡ�
	// ע:
	//   1) ��CAM���ݵ��ڴ����CAM�����Ƕ����ġ�
	//   2) ֻ�е�ͬʱҲ��¡CAD����ʱ�ŵ����������, pOldCadData/pNewCadData�ǿ�¡ǰ/���CAD���ݡ�
	//   3) ��Ҫ���������֮������ù�ϵ��
	//   4) ������������»��档
	static PartCamDataPtr CloneCamData(const PartCamData* pOldCamData, const PartCadData* pOldCadData, PartCadDataPtr pNewCadData);
	/************************************************************************/

	// �ӻ�·���˽ṹ�����������˽ṹ��
	// ע��
	//   1) ���������һ����յ���Χ��·��
	static PartTopologyItemListPtr CalcPartTopStruct(const LoopTopologyItemList* pLoopTopItemList);

private:
	// ��һ������·��������µݹ飬������С�����������
	// ע��
	//   1) "pPartTopItem"�������������Χ��·�Ѿ���"pLoopTopItem"�������·��ʼ���ˡ�
	static void SearchLoopTopItem(LoopTopologyItemPtr pLoopTopItem, PartTopologyItemPtr pPartTopItem);

    // �Ƿ���Ҫ��ͼ�����Ż�
	static bool NeedOptimization(const std::vector<PolygonPatternPtr>& polygonPatList, UINT iOptimizeLimit);
};

END_CUTLEADER_NAMESPACE()
