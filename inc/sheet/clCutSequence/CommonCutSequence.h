#pragma once

#include "PatternPosition.h"
#include "CutPartSequenceBase.h"


DECLARE_CUTLEADER_CLASS(ILineArc2D)
DECLARE_CUTLEADER_CLASS(ComCutGroup)
DECLARE_CUTLEADER_CLASS(CommonCutSequence)

// ���߱ȹ�����Сʱ���������������
#define OVERCUT_FACTOR	1/3.0


BEGIN_CUTLEADER_NAMESPACE()


// �����߹��򡱡�
class CommonCutSequence : public CutPartSequenceBase
{
public:
	CommonCutSequence(void);
	CommonCutSequence(ComCutGroupPtr pComCutGroup, LONGLONG iPartInstID);
	virtual ~CommonCutSequence(void);

public: // implement IData interface.
	// ��¡���߹���
	// ע:
	//  1) ����¡�������顱����Ҫ�Ļ�����������
	virtual IDataPtr Clone() const override;

public: // implement interface ICutSequence.
	virtual CutSequenceType GetCutSequenceType() const override { return CutSequence_CommonCut; }
	virtual Point2D GetFirstCutPt() const override;
	virtual Point2D GetLastCutPt() const override; // ��������õ��ĵ���Ҫ����У�������ӹ��е�����ʱ�����ݹ����д��λ����Ϣ�������������ʵ��������Щƫ�

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
	// �ڡ�pGeom���Ϲ���ʱ���õ����е㡣
	// ע:
	//  1) ��pGeom���ķ�������ѭ�и��ġ�
	static Point2D GetOverCutPt(ILineArc2DPtr pGeom, double dOverCut);

private:
	// ������߹������ڵġ������顱��
	ComCutGroupPtr m_pComCutGroup;

	// ���ʵ��ID��
	LONGLONG m_iPartInstID;

	/************************************************************************/
	// �ӡ��߶���������ʱ�õ��ı�����

	// �Ƿ�ӡ��߶������µ���
	BOOL m_bStartOnEdge;

	// �����λ�á�
	LONGLONG m_iStartPatID;
	PatternPosition m_startPatPos;

	// �г���λ�á�
	LONGLONG m_iEndPatID;
	PatternPosition m_endPatPos;
	/************************************************************************/
};

END_CUTLEADER_NAMESPACE()
