#pragma once

#include "clCutSequenceUICommon.h"
#include "Point2D.h"

DECLARE_CUTLEADER_CLASS(GlViewPort)
DECLARE_CUTLEADER_CLASS(CutNodeList)
DECLARE_CUTLEADER_CLASS(SequenceSimulateShapeList)

BEGIN_CUTLEADER_NAMESPACE()

//
class ClCutSequenceUI_Export RealTimeCutSimulator
{
public:
	RealTimeCutSimulator(void);
	~RealTimeCutSimulator(void);

public:
	// get the singleton object
	static RealTimeCutSimulator* GetInstance();

public: // get/set functions.
	CutNodeListPtr GetCutNodeList() const { return m_pCutNodeList; }
	const SequenceSimulateShapeList* GetSimulatedShapes() const { return m_pSimulatedShapes.get(); }
	GlViewPortPtr GetDrawer() const { return m_pViewPort; }

public:
	// init the simulator.
	void Init(GlViewPortPtr pViewPort, CutNodeListPtr pCutNodeList, SequenceSimulateShapeListPtr pSequenceSimulateShapes_afterSplit);

	// control the simulation.                                                                      
	void StartSimulation();
	void PauseSimulation();
	void ResumeSimulation();
	void StopSimulation();


	/************************************************************************/
	// get the status of the simulation.

	// check whether the simulation is finished.
	BOOL IsFinished() const { return m_bFinished; }

	// whether the simulation is paused.
	BOOL IsPaused() const { return m_bPaused; }
	/************************************************************************/


	// update the simulation shapes which will be drawn.
	// notes:
	//   1) this function will tell the view to re-draw.
	void UpdateSimShapes();

public:
	// the timer message will be sent to this function.
	static void CALLBACK TimerFunc(HWND, UINT uMsg, UINT dEvent, DWORD dwTime);

private:
	static RealTimeCutSimulator* m_pRealTimeCutSimulator;

private:
	// 需要模拟的“切割节点”。
	CutNodeListPtr m_pCutNodeList;

	// all simulation shapes.
	SequenceSimulateShapeListPtr m_pSequenceSimulateShapes_afterSplit;


	/************************************************************************/
	// use these variables, we can know where we are.

	// the index of simulation shape within "m_pSequenceSimulateShapes_afterSplit".
	int m_iSimShapeIndex;

	// the simulate shapes which have been simulated, these shapes will always be shown on the view during the simulation.
	SequenceSimulateShapeListPtr m_pSimulatedShapes;

	// whether the simulation is finished.
	BOOL m_bFinished;

	// whether the simulation is paused.
	BOOL m_bPaused;
	/************************************************************************/


	// the timer ID.
	UINT m_iTimerID;

	GlViewPortPtr m_pViewPort;
};

END_CUTLEADER_NAMESPACE()
