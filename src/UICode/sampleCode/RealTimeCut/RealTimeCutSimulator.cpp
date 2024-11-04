#include "StdAfx.h"
#include "RealTimeCutSimulator.h"

#include "GlViewPort.h"

#include "LineArc2DList.h"
#include "Triangle2D.h"

#include "ICutPartSequence.h"
#include "ICutSequence.h"
#include "CutSequenceList.h"
#include "CutNode.h"
#include "CutNodeList.h"
#include "SequenceSimulateShapeList.h"
#include "ISequenceSimulateShape.h"
#include "CutNodeViewBar.h"

#include "CLOptionInfo.h"
#include "ConcealConfig.h"
#include "DebugUIInfo.h"
#include "DisplayParam.h"
#include "SimulateInfo.h"

// 定时器的时间间隔。
const int g_iTimeInterval = 1;

// the interval count，用来控制模拟的速度。
const int g_iIntervalNum = 2;


BEGIN_CUTLEADER_NAMESPACE()

RealTimeCutSimulator* RealTimeCutSimulator::m_pRealTimeCutSimulator = NULL;

RealTimeCutSimulator* RealTimeCutSimulator::GetInstance()
{
	if (m_pRealTimeCutSimulator == NULL)
    {
		m_pRealTimeCutSimulator = new RealTimeCutSimulator();
    }

	return m_pRealTimeCutSimulator;
}

RealTimeCutSimulator::RealTimeCutSimulator(void)
{
	m_pSimulatedShapes.reset(new SequenceSimulateShapeList);
	m_bFinished = TRUE;
}

RealTimeCutSimulator::~RealTimeCutSimulator(void)
{
}

void RealTimeCutSimulator::Init(GlViewPortPtr pViewPort, CutNodeListPtr pCutNodeList, SequenceSimulateShapeListPtr pSequenceSimulateShapes_afterSplit)
{
	m_pViewPort = pViewPort;
	m_pCutNodeList = pCutNodeList;
	m_pSequenceSimulateShapes_afterSplit = pSequenceSimulateShapes_afterSplit;

	// init some state.
	m_iSimShapeIndex = 0;
	m_bFinished = FALSE;
	m_bPaused = FALSE;
	m_pSimulatedShapes->clear();
}

void RealTimeCutSimulator::StartSimulation()
{
	m_iTimerID = ::SetTimer(NULL, 100, g_iTimeInterval, (TIMERPROC)RealTimeCutSimulator::TimerFunc); 
}

void RealTimeCutSimulator::StopSimulation()
{
	::KillTimer(NULL, m_iTimerID);

	// reset some state.
	m_pSimulatedShapes->clear(); // 把动态模拟的形状清除。
	m_bFinished = TRUE;
	m_bPaused = FALSE;
}

void RealTimeCutSimulator::PauseSimulation()
{
	::KillTimer(NULL, m_iTimerID);
	m_bPaused = TRUE;
}

void RealTimeCutSimulator::ResumeSimulation()
{
	m_iTimerID = ::SetTimer(NULL, 0, g_iTimeInterval, (TIMERPROC)RealTimeCutSimulator::TimerFunc); 
	m_bPaused = FALSE;
}

void RealTimeCutSimulator::UpdateSimShapes()
{
	if ((unsigned int)m_iSimShapeIndex < m_pSequenceSimulateShapes_afterSplit->size())
	{
		int iLeftCount = m_pSequenceSimulateShapes_afterSplit->size() - m_iSimShapeIndex;
		SequenceSimulateShapeList::iterator beginIter = m_pSequenceSimulateShapes_afterSplit->begin() + m_iSimShapeIndex;
		if (iLeftCount >= g_iIntervalNum)
		{
			SequenceSimulateShapeList::iterator endIter = beginIter + g_iIntervalNum;
			m_pSimulatedShapes->insert(m_pSimulatedShapes->end(), beginIter, endIter);
			m_iSimShapeIndex += g_iIntervalNum;
		}
		else
		{
			SequenceSimulateShapeList::iterator endIter = beginIter + iLeftCount;
			m_pSimulatedShapes->insert(m_pSimulatedShapes->end(), beginIter, endIter);
			m_iSimShapeIndex += iLeftCount;
		}
	}
	else
	{
		// the simulation is finished.
		StopSimulation();
	}

	// told the view to redraw.
	CWnd* pView = CWnd::FromHandle(m_pViewPort->GetWnd());
	pView->Invalidate();
}

void RealTimeCutSimulator::TimerFunc(HWND, UINT uMsg, UINT dEvent, DWORD dwTime)
{
	// figure out the nest simulation shape and told the view to draw.
	GetInstance()->UpdateSimShapes();
}

END_CUTLEADER_NAMESPACE()
