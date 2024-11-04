#pragma once

#include "clGeometryBaseCommon.h"

DECLARE_CUTLEADER_CLASS(GlViewPort)

BEGIN_CUTLEADER_NAMESPACE()

// this class can draw some kinds of cursor.
// notes:
//   1) all line width of the cursor should be 1.
//   2) the cursor color should be set outside this class.
class ClGeometryBase_Export CursorDrawer
{
public:
	// draw the default cursor.
	/*     
		        CursorPt (pt1)            
					**                   
					* **                 
					*   *                
					*    *               
					*     **             
					*       *            
					*        **          
					*          *         
					*           *  pt7      
					*            **      
					*          **        
					*   pt3  **  pt6        
					*  **     *          
			pt2		**   *     *         
						  *    *         
						  *   **  pt5       
						   ****           
                         pt4
	*/
	// notes:
	//   1) draw the cursor within a (30*30) rect. the name on the illustration
	//      are similar with those in the implementation.
	static void DrawDefaultCursor(GlViewPortPtr pViewPort);

	// draw the aim select cursor.
	/*       
					*         
					*         
				+-------+         
				|   *   |     
			****|*******|*****
				|   *   |     
				+-------+     
					*         
					*         
	*/
	// notes:
	//   1) the center point is the cursor position.
	//   2) draw the cursor within a (40*40) rect.
	static void DrawAimSelCursor(GlViewPortPtr pViewPort);
	
	// draw the rect select cursor.
	/*
			   +-------+   
			   |       |   
			   |       |   
			   |       |   
			   +-------+   
	*/
	// notes:
	//   1) the center point is the cursor position.
	//   2) draw the cursor within a (10*10) rect.
	static void DrawRectSelCursor(GlViewPortPtr pViewPort);

	// draw the cross line cursor.
	/*             
					 *        
					 *        
					 *        
			  ***************
					 *        
					 *        
					 * 
	*/
	// notes:
	//   1) the center point is the cursor position.
	//   2) draw the cursor within a (40*40) rect.
	static void DrawCrossLineCursor(GlViewPortPtr pViewPort);
};

END_CUTLEADER_NAMESPACE()
