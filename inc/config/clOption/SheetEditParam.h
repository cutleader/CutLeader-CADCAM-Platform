#pragma once

#include "clOptionCommon.h"
#include "ClData.h"

BEGIN_CUTLEADER_NAMESPACE()

// 板材编辑参数。
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
	// 步进。
	double m_dMoveStep;
	double m_dRotateStep; // 角度单位。

	// 零件距离检测。
	BOOL m_bDetectPartDis;
	BOOL m_bDetectCommon; // 一键靠碰时检测共边。
	double m_dSheetMargin;
	double m_dPartDis;
	double m_dStickDis;
	double m_dEscapeDis;

	// 布局零件时的轮廓光顺控制。
	BOOL m_bSleekCtrl; // 是否采取光顺控制。
	double m_dSleekRate; // 零件大于轮廓矩形的一定比例，不做光顺处理。

	// 工序操作时，捕获轮廓的距离。
	// 注：
	//  1) 是像素单位。
	int m_iLoopDetectDis;
};

END_CUTLEADER_NAMESPACE()
