#pragma once

#include "clGeometryBaseCommon.h"
#include "clCutSequenceCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(AvoidData)
DECLARE_CUTLEADER_CLASS(ComCutGroup)

BEGIN_CUTLEADER_NAMESPACE()

// 共边组。
//  1) 这个对象定义了可以做共边的一组零件。
class ComCutGroup : public ClData
{
public:
	ComCutGroup(void);
	ComCutGroup(PartInstanceListPtr pPartInstances, AvoidDataPtr pAvoidData, DIRECTION cutDir, double dOverCut, double dToolWidth);
	virtual ~ComCutGroup(void);

public: // implement IData interface.
	// 克隆共边组。
	// 注:
	//   1) do not need to clone "m_pPartInstList", so if we want to change it, we should do this outside.
	virtual IDataPtr Clone() const override;

public:
	void SetPartInsts(PartInstanceListPtr pPartInstances) { m_pPartInstList = pPartInstances; }
	PartInstanceListPtr GetPartInstances() const { return m_pPartInstList; }

	void SetAvoidData(AvoidDataPtr pAvoidData) { m_pAvoidData = pAvoidData; }
	AvoidDataPtr GetAvoidData() const { return m_pAvoidData; }

	void SetCutDirection(DIRECTION cutDir) { m_cutDir = cutDir; }
	DIRECTION GetCutDirection() const { return m_cutDir; }

	void SetOverCut(double dOverCut) { m_dOverCut = dOverCut; }
	double GetOverCut() const { return m_dOverCut; }

	void SetToolWidth(double dToolWidth) { m_dToolWidth = dToolWidth; }
	double GetToolWidth() const { return m_dToolWidth; }

protected:
	// 可以做共边的一组零件。
	PartInstanceListPtr m_pPartInstList;

	// 共边工序内部做避让时需要参考的数据。
	AvoidDataPtr m_pAvoidData;

	// 共边参数。
	DIRECTION m_cutDir;
	double m_dOverCut;

	double m_dToolWidth;
};

END_CUTLEADER_NAMESPACE()
