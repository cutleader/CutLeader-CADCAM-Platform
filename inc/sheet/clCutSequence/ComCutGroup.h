#pragma once

#include "clGeometryBaseCommon.h"
#include "clCutSequenceCommon.h"
#include "ClData.h"

DECLARE_CUTLEADER_CLASS(PartInstanceList)
DECLARE_CUTLEADER_CLASS(AvoidData)
DECLARE_CUTLEADER_CLASS(ComCutGroup)

BEGIN_CUTLEADER_NAMESPACE()

// �����顣
//  1) ����������˿��������ߵ�һ�������
class ComCutGroup : public ClData
{
public:
	ComCutGroup(void);
	ComCutGroup(PartInstanceListPtr pPartInstances, AvoidDataPtr pAvoidData, DIRECTION cutDir, double dOverCut, double dToolWidth);
	virtual ~ComCutGroup(void);

public: // implement IData interface.
	// ��¡�����顣
	// ע:
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
	// ���������ߵ�һ�������
	PartInstanceListPtr m_pPartInstList;

	// ���߹����ڲ�������ʱ��Ҫ�ο������ݡ�
	AvoidDataPtr m_pAvoidData;

	// ���߲�����
	DIRECTION m_cutDir;
	double m_dOverCut;

	double m_dToolWidth;
};

END_CUTLEADER_NAMESPACE()
