#pragma once

#include "PatternPosition.h"
#include "CutPartSequenceBase.h"


DECLARE_CUTLEADER_CLASS(ILineArc2D)
DECLARE_CUTLEADER_CLASS(ComCutGroup)
DECLARE_CUTLEADER_CLASS(CommonCutSequence)

// 当边比过切量小时，用这个过切量。
#define OVERCUT_FACTOR	1/3.0


BEGIN_CUTLEADER_NAMESPACE()


// “共边工序”。
class CommonCutSequence : public CutPartSequenceBase
{
public:
	CommonCutSequence(void);
	CommonCutSequence(ComCutGroupPtr pComCutGroup, LONGLONG iPartInstID);
	virtual ~CommonCutSequence(void);

public: // implement IData interface.
	// 克隆共边工序。
	// 注:
	//  1) 不克隆“共边组”，需要的话在外面做。
	virtual IDataPtr Clone() const override;

public: // implement interface ICutSequence.
	virtual CutSequenceType GetCutSequenceType() const override { return CutSequence_CommonCut; }
	virtual Point2D GetFirstCutPt() const override;
	virtual Point2D GetLastCutPt() const override; // 这个函数得到的点需要经过校正，当从过切点起切时，根据工序中存的位置信息计算出的坐标会和实际坐标有些偏差。

public: // implement interface ICutPartSequence.
	virtual BOOL LoopHasBeenCut(const LoopInstance* pLoopInstance) const override;
	virtual LoopInstanceListPtr GetLoopInstanceList() const override;
	virtual LoopInstancePtr GetLoopHasLead() const override;
	virtual PartInstanceListPtr GetPartInstances() const override;

public: // get/set functions.
	void SetComCutGroup(ComCutGroupPtr pComCutGroup) { m_pComCutGroup = pComCutGroup; }
	ComCutGroupPtr GetComCutGroup() { return m_pComCutGroup; }

	void SetPartInstID(LONGLONG iPartInstID) { m_iPartInstID = iPartInstID; }
	LONGLONG GetPartInstID() { return m_iPartInstID; }

	void IsStartOnEdge(BOOL bStartOnEdge) { m_bStartOnEdge = bStartOnEdge; }
	BOOL IsStartOnEdge() { return m_bStartOnEdge; }

	void SetStartPatID(LONGLONG iStartPatID) { m_iStartPatID = iStartPatID; }
	LONGLONG GetStartPatID() { return m_iStartPatID; }

	void SetStartPatPos(PatternPosition startPatPos) { m_startPatPos = startPatPos; }
	PatternPosition GetStartPatPos() { return m_startPatPos; }

	void SetEndPatID(LONGLONG iEndPatID) { m_iEndPatID = iEndPatID; }
	LONGLONG GetEndPatID() { return m_iEndPatID; }

	void SetEndPatPos(PatternPosition endPatPos) { m_endPatPos = endPatPos; }
	PatternPosition GetEndPatPos() { return m_endPatPos; }

public:
	// 在“pGeom”上过切时，得到过切点。
	// 注:
	//  1) “pGeom”的方向是遵循切割方向的。
	static Point2D GetOverCutPt(ILineArc2DPtr pGeom, double dOverCut);

private:
	// 这个共边工序所在的“共边组”。
	ComCutGroupPtr m_pComCutGroup;

	// 零件实例ID。
	LONGLONG m_iPartInstID;

	/************************************************************************/
	// 从“边对象”上起切时用到的变量。

	// 是否从“边对象”上下刀。
	BOOL m_bStartOnEdge;

	// 切入点位置。
	LONGLONG m_iStartPatID;
	PatternPosition m_startPatPos;

	// 切出点位置。
	LONGLONG m_iEndPatID;
	PatternPosition m_endPatPos;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
