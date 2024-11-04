#pragma once

#include "clCutSequenceCommon.h"

DECLARE_CUTLEADER_CLASS(DBConnect)
DECLARE_CUTLEADER_CLASS(IntegerList)
DECLARE_CUTLEADER_CLASS(Point2DList)
DECLARE_CUTLEADER_CLASS(CutSequenceList)
DECLARE_CUTLEADER_CLASS(LoopSequence)
DECLARE_CUTLEADER_CLASS(GridLoopSequence)
DECLARE_CUTLEADER_CLASS(CommonCutSequence)
DECLARE_CUTLEADER_CLASS(BridgeNode)
DECLARE_CUTLEADER_CLASS(BridgeSequence)
DECLARE_CUTLEADER_CLASS(ChainCutSequence)
DECLARE_CUTLEADER_CLASS(GridCutSequence)
DECLARE_CUTLEADER_CLASS(FlyCutSequence)
DECLARE_CUTLEADER_CLASS(RemnantLineSeq)

BEGIN_CUTLEADER_NAMESPACE()

// use this class to load sequence data from database.
class ClCutSequence_Export SequenceDataWriter
{
public:
	// save sequence units of the sheet to the database.
	static void SaveSeqData(DBConnectPtr pDBConnect, LONGLONG iSheetID, CutSequenceListPtr pCutSequences);

	// delete all sequence of sheet.
	static void DeleteSheetSeq(DBConnectPtr pDBConnect, BOOL bTransaction = TRUE);

private:
	/************************************************************************/
	// about loop sequence.

	// save the loop sequence.
	static void SaveLoopSeq(DBConnectPtr pDBConnect, LoopSequencePtr pLoopSequence);

	// save the loop grid sequence.
	static void SaveLoopGridSeq(DBConnectPtr pDBConnect, GridLoopSequencePtr pLoopGridSeq);
	/************************************************************************/


	// save comcut sequence.
	static void SaveComCutSeq(DBConnectPtr pDBConnect, CommonCutSequencePtr pComCutSeq, IntegerListPtr pSavedComGroups);
	static void SaveComcutGroup(DBConnectPtr pDBConnect, CommonCutSequencePtr pComCutSeq);

	// 保存桥接工序。
	static void SaveBdgeCutSeq(DBConnectPtr pDBConnect, BridgeSequencePtr pBridgeSeq);
	static void SaveSubBdgeNode(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, BridgeNodePtr pBridgeNode);

	// 保存连割工序。
	static void SaveChainSeq(DBConnectPtr pDBConnect, ChainCutSequencePtr pChainSeq);

	// 保存田字工序。
	static void SaveGridSeq(DBConnectPtr pDBConnect, GridCutSequencePtr pGridSeq);

	// 保存飞行切割工序。
	static void SaveFlyCutSeq(DBConnectPtr pDBConnect, FlyCutSequencePtr pFlyCutSeq);

	// save trace data.
	static void SaveTraceData(DBConnectPtr pDBConnect, LONGLONG iCutSequenceID, Point2DListPtr pPtList);

	// 保存“余料切割线”
	static void SaveRemnantLineSeq(DBConnectPtr pDBConnect, RemnantLineSeqPtr pRemnantLineSeq);
};

END_CUTLEADER_NAMESPACE()
