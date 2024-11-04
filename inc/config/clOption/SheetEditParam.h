#pragma once

#include "clOptionCommon.h"
#include "ClData.h"

BEGIN_CUTLEADER_NAMESPACE()

// ��ı༭������
class ClOption_Export SheetEditParam : public ClData
{
public:
	SheetEditParam(void);
	~SheetEditParam(void);

public: // get/set functions
	void SetMoveStep(double dMoveStep) { m_dMoveStep = dMoveStep; }
	double GetMoveStep() const { return m_dMoveStep; }

	void SetRotateStep(double dRotateStep) { m_dRotateStep = dRotateStep; }
	double GetRotateStep() const { return m_dRotateStep; }

	void EnableDisDetect(BOOL bDetectPartDis) { m_bDetectPartDis = bDetectPartDis; }
	BOOL EnableDisDetect() const { return m_bDetectPartDis; }

	void EnableComDetect(BOOL bDetectCommon) { m_bDetectCommon = bDetectCommon; }
	BOOL EnableComDetect() const { return m_bDetectCommon; }

	void SetSheetMargin(double dSheetMargin) { m_dSheetMargin = dSheetMargin; }
	double GetSheetMargin() const { return m_dSheetMargin; }

	void SetPartDis(double dPartDis) { m_dPartDis = dPartDis; }
	double GetPartDis() const { return m_dPartDis; }

	void SetStickDis(double dStickDis) { m_dStickDis = dStickDis; }
	double GetStickDis() const { return m_dStickDis; }

	void SetEscapeDis(double dEscapeDis) { m_dEscapeDis = dEscapeDis; }
	double GetEscapeDis() const { return m_dEscapeDis; }

	void EnableSleekCtrl(BOOL bSleekCtrl) { m_bSleekCtrl = bSleekCtrl; }
	BOOL EnableSleekCtrl() const { return m_bSleekCtrl; }

	void SetSleekRate(double dSleekRate) { m_dSleekRate = dSleekRate; }
	double GetSleekRate() const { return m_dSleekRate; }

	void SetLoopDetectDis(int iLoopDetectDis) { m_iLoopDetectDis = iLoopDetectDis; }
	int GetLoopDetectDis() const { return m_iLoopDetectDis; }

private:
	// ������
	double m_dMoveStep;
	double m_dRotateStep; // �Ƕȵ�λ��

	// ��������⡣
	BOOL m_bDetectPartDis;
	BOOL m_bDetectCommon; // һ������ʱ��⹲�ߡ�
	double m_dSheetMargin;
	double m_dPartDis;
	double m_dStickDis;
	double m_dEscapeDis;

	// �������ʱ��������˳���ơ�
	BOOL m_bSleekCtrl; // �Ƿ��ȡ��˳���ơ�
	double m_dSleekRate; // ��������������ε�һ��������������˳����

	// �������ʱ�����������ľ��롣
	// ע��
	//  1) �����ص�λ��
	int m_iLoopDetectDis;
};

END_CUTLEADER_NAMESPACE()
