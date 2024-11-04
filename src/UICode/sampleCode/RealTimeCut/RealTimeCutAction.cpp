#include "StdAfx.h"
#include "RealTimeCutAction.h"

#include "DBConnect.h"
#include "DataBaseManager.h"
#include "GlViewPort.h"
#include "Line2D.h"
#include "LineArc2DList.h"
#include "Rect2D.h"
#include "ValueInputManager.h"
#include "DummyAction.h"
#include "CursorDrawer.h"
#include "ProgressDlg.h"
#include "Arc2D.h"
#include "LoopDrawer.h"
#include "PartPlacementList.h"
#include "PartInstanceList.h"
#include "LoopInstanceList.h"
#include "LoopInstance.h"
#include "PartPlacementManager.h"

#include "clCutSequenceResource.h"
#include "ICutPartSequence.h"
#include "CutSequenceList.h"
#include "SequenceSimulateShapeList.h"
#include "CutNodeManager.h"
#include "CutNodeList.h"
#include "CutNodeManager.h"
#include "RealTimeCutSimulator.h"
#include "PartLayoutData.h"
#include "CutSequenceDrawer.h"
#include "CutSequenceSelectAction.h"
#include "CutSequenceManager.h"
#include "GenerateCutNodeTask.h"
#include "ConfigData.h"
#include "TraceLineSimulateShape.h"
#include "CutSimulateShape.h"

#include "SequenceConfigItem.h"
#include "IMaterialSize.h"
#include "CLOptionInfo.h"
#include "DisplayParam.h"
#include "LeadConfigItem.h"
#include "ParamConfig.h"
#include "ParamConfigLoader.h"

// 把切割路径打断成如下一段段的。
const double g_dSplitInterval = 1.0;

BEGIN_CUTLEADER_NAMESPACE()

RealTimeCutAction::RealTimeCutAction(PartLayoutDataPtr pPartLayoutData, CutSequenceListPtr pCutSequences, ConfigDataPtr pConfigData, GlViewPortPtr pViewPort)
	: SequenceEditActionBase(pPartLayoutData, pCutSequences, pConfigData, pViewPort)
{
	m_bDisplayValInput = FALSE;
	m_pSequenceSimulateShapes.reset();
}

RealTimeCutAction::~RealTimeCutAction(void)
{
}

void RealTimeCutAction::Start()
{
	const IMaterialSize* pMaterialSize = m_pConfigData->GetMatSize();
	const SequenceConfigItem* pSeqConfigItem = m_pConfigData->GetSeqConfigItem();

	// 把“切割工序”转化为“切割节点”。
	CutNodeListPtr pCutNodeList;
	Point2D startSeqPt = pMaterialSize->GetOuterRect().GetCornerPt(pSeqConfigItem->GetStartCorner());
	if (pSeqConfigItem->IsSoftwareOffset())
	{
		// 得到一些专家库规则。
		DBConnectPtr pExpLibDBCon = DataBaseManager::GetInstance()->GetExpLibDBConnect();
		ParamConfigPtr pParamConfig = ParamConfigLoader::LoadParamConfig(pExpLibDBCon, pSeqConfigItem->GetParentID());
		LeadConfigItemPtr pLeadConfigItem = boost::dynamic_pointer_cast<LeadConfigItem>(pParamConfig->GetItemByType(PARAMCONFIG_ITEM_LEAD_ASSIGN));

		// 创建一个任务生成“切割节点”。
        {
            // init the task.
            PartPlacementListPtr pPartPlacements = m_pPartLayoutData->GetPartPlacements();
            PartInstanceListPtr pPartInstances = m_pPartLayoutData->GetPartInstances();
            GenerateCutNodeTaskPtr pGenCutNodeTask(new GenerateCutNodeTask(startSeqPt, pPartPlacements, pPartInstances, m_pCutSequences, pSeqConfigItem->IsCheckIntf(), pLeadConfigItem));

            // display the progress of the task.
            CString str;
            str = MultiLanguageMgr::GetInstance()->TranslateString(IDS_GEN_CUTNODE_TASK);
            ProgressDlg progressDlg;
            progressDlg.Init(str, pGenCutNodeTask);
            progressDlg.DoModal();

            // 得到结果。
            pCutNodeList = pGenCutNodeTask->GetCutNodes();
        }
	}
	else
	{
		pCutNodeList = CutNodeManager::CutSequences2CutNodes(startSeqPt, m_pCutSequences.get());
	}

	// 生成“工序模拟形状”集合。
    m_pSequenceSimulateShapes = CutNodeManager::CutNode2SimShape(pCutNodeList.get());

	// 初始化并启动“实时切割模拟器”，为了模拟实时切割的场景，这里把“模拟形状”打断成一段段很短的图形。
	m_pRealTimeCutSimulator = RealTimeCutSimulator::GetInstance();
    SequenceSimulateShapeListPtr pSequenceSimulateShapes_afterSplit = SplitSequenceSimulateShapes(m_pSequenceSimulateShapes.get());
	m_pRealTimeCutSimulator->Init(m_pViewPort, pCutNodeList, pSequenceSimulateShapes_afterSplit);
	m_pRealTimeCutSimulator->StartSimulation();
}

SequenceSimulateShapeListPtr RealTimeCutAction::SplitSequenceSimulateShapes(const SequenceSimulateShapeList* pSequenceSimulateShapes)
{
    SequenceSimulateShapeListPtr pSequenceSimulateShapes_afterSplit(new SequenceSimulateShapeList);

    for (unsigned int i = 0; i < pSequenceSimulateShapes->size(); i++)
    {
        const ISequenceSimulateShape* pSequenceSimulateShape = pSequenceSimulateShapes->operator [](i).get();
        if (pSequenceSimulateShape->GetSimShapeType() == SIM_SHAPE_CUT)
        {
            const CutSimulateShape* pCutSimulateShape = (const CutSimulateShape*)pSequenceSimulateShape;
            const ILineArc2D* pLineArc2D = pCutSimulateShape->GetLineArc2D();
            if (pLineArc2D->GetType() == LineArc2D_Line)
            {
                const Line2D* pLine2D = (const Line2D*)pLineArc2D;

                // 把线段打碎。
                Point2D splitPosition = pLine2D->GetStartPoint();
                while (true)
                {
                    if (splitPosition.DistanceTo(pLine2D->GetEndPoint()) < g_dSplitInterval) // 线段长度已经小于g_dSplitInterval，不需再分割了。
                    {
                        Line2DPtr pLine2D_new(new Line2D(splitPosition, pLine2D->GetEndPoint()));
                        CutSimulateShapePtr pCutSimulateShape_new(new CutSimulateShape(pLine2D_new, pLine2D_new->GetRectBox(), pCutSimulateShape->IsCutPart()));
                        pSequenceSimulateShapes_afterSplit->push_back(pCutSimulateShape_new);
                        break;
                    }
                    else // 继续分割。
                    {
                        Point2D splitPosition_old = splitPosition;

                        // 确定分割点新位置
                        Vector2D vect(pLine2D->GetStartPoint(), pLine2D->GetEndPoint());
                        vect.Magnitude(g_dSplitInterval);
                        splitPosition += vect;

                        //
                        Line2DPtr pLine2D_new(new Line2D(splitPosition_old, splitPosition));
                        CutSimulateShapePtr pCutSimulateShape_new(new CutSimulateShape(pLine2D_new, pLine2D_new->GetRectBox(), pCutSimulateShape->IsCutPart()));
                        pSequenceSimulateShapes_afterSplit->push_back(pCutSimulateShape_new);
                    }
                }
            }
            else if (pLineArc2D->GetType() == LineArc2D_Arc)
            {
                const Arc2D* pArc2D = (const Arc2D*)pLineArc2D;

                // 把圆弧打碎
                if (pArc2D->GetArcLen() < g_dSplitInterval) // 圆弧太短，不需打碎。
                {
                    Arc2DPtr pArc2D_new = boost::dynamic_pointer_cast<Arc2D>(pArc2D->Clone());
                    CutSimulateShapePtr pCutSimulateShape_new(new CutSimulateShape(pArc2D_new, pArc2D_new->GetRectBox(), pCutSimulateShape->IsCutPart()));
                    pSequenceSimulateShapes_afterSplit->push_back(pCutSimulateShape_new);
                }
                else
                {
                    int iNumber = pArc2D->GetArcLen() / g_dSplitInterval + 1; // 打碎后圆弧的个数。
                    double dPercent = g_dSplitInterval / pArc2D->GetArcLen(); // 除了最后一段圆弧，其他圆弧长度的百分比。
                    for (unsigned int j = 0; j < iNumber; j++)
                    {
                        if (j < iNumber - 1)
                        {
                            Arc2DPtr pArc2D_new = pArc2D->GetPercentSegment(dPercent * j, dPercent * ( j + 1));
                            CutSimulateShapePtr pCutSimulateShape_new(new CutSimulateShape(pArc2D_new, pArc2D_new->GetRectBox(), pCutSimulateShape->IsCutPart()));
                            pSequenceSimulateShapes_afterSplit->push_back(pCutSimulateShape_new);
                        }
                        else
                        {
                            Arc2DPtr pArc2D_new = pArc2D->GetPercentSegment(dPercent * ( j + 1), 1.0);
                            CutSimulateShapePtr pCutSimulateShape_new(new CutSimulateShape(pArc2D_new, pArc2D_new->GetRectBox(), pCutSimulateShape->IsCutPart()));
                            pSequenceSimulateShapes_afterSplit->push_back(pCutSimulateShape_new);
                        }
                    }
                }
            }
        }
        else if (pSequenceSimulateShape->GetSimShapeType() == SIM_SHAPE_TRACE)
        {
            const TraceLineSimulateShape* pTraceSimShape = (const TraceLineSimulateShape*)pSequenceSimulateShape;
            const Line2D* pLine2D = pTraceSimShape->GetLine2D();

            // 把线段打碎。
            Point2D splitPosition = pLine2D->GetStartPoint();
            while (true)
            {
                if (splitPosition.DistanceTo(pLine2D->GetEndPoint()) < g_dSplitInterval) // 线段长度已经小于g_dSplitInterval，不需再分割了。
                {
                    Line2DPtr pLine2D_new(new Line2D(splitPosition, pLine2D->GetEndPoint()));
                    TraceLineSimulateShapePtr pTraceLineSimulateShape_new(new TraceLineSimulateShape(pLine2D_new, pLine2D_new->GetRectBox()));
                    pSequenceSimulateShapes_afterSplit->push_back(pTraceLineSimulateShape_new);
                    break;
                }
                else // 继续分割。
                {
                    Point2D splitPosition_old = splitPosition;

                    // 确定分割点新位置
                    Vector2D vect(pLine2D->GetStartPoint(), pLine2D->GetEndPoint());
                    vect.Magnitude(g_dSplitInterval);
                    splitPosition += vect;

                    //
                    Line2DPtr pLine2D_new(new Line2D(splitPosition_old, splitPosition));
                    TraceLineSimulateShapePtr pTraceLineSimulateShape_new(new TraceLineSimulateShape(pLine2D_new, pLine2D_new->GetRectBox()));
                    pSequenceSimulateShapes_afterSplit->push_back(pTraceLineSimulateShape_new);
                }
            }
        }
    }

    return pSequenceSimulateShapes_afterSplit;
}

BOOL RealTimeCutAction::LButtonDown(double& dPtX, double& dPtY)
{
	// check whether we need to pause/resume the simulation.
	if (m_pRealTimeCutSimulator->IsPaused())
    {
		m_pRealTimeCutSimulator->ResumeSimulation();
    }
	else
    {
		m_pRealTimeCutSimulator->PauseSimulation();
    }

	return TRUE;
}

BOOL RealTimeCutAction::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (ValueInputManager::GetInstance()->IsInputCancel(nChar))
	{
		// send "stop" message to simulator.
		if (!m_pRealTimeCutSimulator->IsFinished())
        {
			m_pRealTimeCutSimulator->StopSimulation();
        }

		// figure out the next action.
		m_pNextAction.reset(new CutSequenceSelectAction(m_pPartLayoutData, m_pCutSequences, m_pConfigData, m_pViewPort));
	}

	return TRUE;
}

BOOL RealTimeCutAction::DrawBeforeFinish()
{
    const DisplayParam* pColorInfo = CLOptionInfo::GetInstance()->GetDisplayParam().get();

	// set the prompt message.
	if (m_pRealTimeCutSimulator->IsPaused())
    {
		m_iMsgID = IDS_SEQ_VIEW_PAUSE;
    }
	else if (m_pRealTimeCutSimulator->IsFinished())
    {
		m_iMsgID = IDS_SEQ_VIEW_FINISH;
    }

	// 老的设置。
	DWORD dwOldColor;
	m_pViewPort->GetDrawColor(dwOldColor);

	// 绘制模拟图形。
	m_pViewPort->SetDrawColor(*pColorInfo->GetSeqSimColor());
    const SequenceSimulateShapeList* pSequenceSimulateShapes_simulated = m_pRealTimeCutSimulator->GetSimulatedShapes();
    DrawSequenceSimulateShapes(pSequenceSimulateShapes_simulated); // 动态的切割过程没有用CCP core提供的函数。

	// 恢复绘图设置。
	m_pViewPort->SetDrawColor(dwOldColor);

	// draw cursor.
	CursorDrawer::DrawAimSelCursor(m_pViewPort);

	return TRUE;
}

void RealTimeCutAction::DrawCutHeader(const Point2D& position)
{
    // keep the old config.
    COLORREF dwOldColor;
    m_pViewPort->GetDrawColor(dwOldColor);
    int iOldLineWid = m_pViewPort->GetLineWidth();

    // 绘制切割头
    m_pViewPort->SetLineWidth(1);
    m_pViewPort->DrawCrossPoint2D(position.X(), position.Y(), 20, 0);

    // 绘制切割头坐标
    CString str;
    str.Format(_T("X: %.3f, Y: %.3f"), position.X(), position.Y());
    m_pViewPort->DrawText(str, position.X() + 10.0, position.Y() + 10.0, position.X() + 100.0, position.Y() + 30.0);

    // restore the old config.
    m_pViewPort->SetDrawColor(dwOldColor);
    m_pViewPort->SetLineWidth(iOldLineWid);
}

void RealTimeCutAction::DrawSequenceSimulateShapes(const SequenceSimulateShapeList* pSequenceSimulateShapes)
{
    BasicGeometryDrawer basicGeometryDrawer(m_pViewPort);

    // 绘制动态切割路径。
    Point2D lastCutPt;
    int iOldLineWid = m_pViewPort->GetLineWidth();
    for (unsigned int i = 0; i < pSequenceSimulateShapes->size(); i++)
    {
        const ISequenceSimulateShape* pSequenceSimulateShape = pSequenceSimulateShapes->operator [](i).get();
        if (pSequenceSimulateShape->GetSimShapeType() == SIM_SHAPE_CUT)
        {
            m_pViewPort->SetLineWidth(2);

            const CutSimulateShape* pCutSimulateShape = (const CutSimulateShape*)pSequenceSimulateShape;
            basicGeometryDrawer.Draw2DLineArc(pCutSimulateShape->GetLineArc2D());
            if (i == pSequenceSimulateShapes->size() - 1)
            {
                lastCutPt = pCutSimulateShape->GetLineArc2D()->GetEndPoint();
            }
        }
        else if (pSequenceSimulateShape->GetSimShapeType() == SIM_SHAPE_TRACE)
        {
            m_pViewPort->SetLineWidth(1);

            const TraceLineSimulateShape* pTraceSimShape = (const TraceLineSimulateShape*)pSequenceSimulateShape;
            basicGeometryDrawer.DrawLine2D(pTraceSimShape->GetLine2D());
            if (i == pSequenceSimulateShapes->size() - 1)
            {
                lastCutPt = pTraceSimShape->GetLine2D()->GetEndPoint();
            }
        }
    }
    m_pViewPort->SetLineWidth(iOldLineWid);

    // 绘制切割头
    DrawCutHeader(lastCutPt);
}

END_CUTLEADER_NAMESPACE()
