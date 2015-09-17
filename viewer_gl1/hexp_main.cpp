#include "planet_gui.h"
#include <GL/glui.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>

//=========================================
// globals (mostly for dealing with glut/gui)
//=========================================
int g_glutMainWin = 0;
float g_aspect = 0.;
PlanetGui *g_planetGui = NULL;

/// callback to draw the frame
void glut_Display(void)
{  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0f, g_aspect, 1.0f, 5000.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  g_planetGui->redraw();

  glutSwapBuffers(); 
}

/// callback when window changes shape
void glut_Reshape(int x, int y)
{
  int tx, ty, tw, th;
  GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
  glViewport(tx, ty, tw, th);

  g_aspect = (float)tw / (float)th;

  glutPostRedisplay();
}

/// callback on key press
void glut_Keyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 27: // escape
  case 'q':
    exit(0);
    break;

#if 0
	// for debugging cursor thinnggy
  case '>':
  case '.':
	g_planetGui->m_cursorHex++;
	break;
   case '<':
   case ',':
	g_planetGui->m_cursorHex--;
	break;
#endif
  };  
  
  glutPostRedisplay();
}

void glut_Menu(int value)
{
  glut_Keyboard(value, 0, 0);
}

/// callback when nothing is happening
void glut_Idle(void)
{
  // According to the GLUT specification, the current window is 
  // undefined during an idle callback.  So we need to explicitly change
  // it if necessary
  if (glutGetWindow() != g_glutMainWin)
  {
	glutSetWindow(g_glutMainWin);  
  }

  // Sync widgets to live variables
  GLUI_Master.sync_live_all();  

  // update "sim"
  // No sim

  // redraw
  glutPostRedisplay();
}

/// callback on mouse actions
void glut_Mouse(int button, int button_state, int x, int y)
{
	g_planetGui->m_mouseX = x;
	g_planetGui->m_mouseY = y;    
	g_planetGui->paintTile();
}

/// callback on mouse move
void glut_Motion(int x, int y)
{
	g_planetGui->m_mouseX = x;
	g_planetGui->m_mouseY = y;    
}

/// callback on mouse move
void glut_MotionDraw(int x, int y)
{
	g_planetGui->m_mouseX = x;
	g_planetGui->m_mouseY = y;    
	g_planetGui->paintTile();
}

/// Main program
int main( int argc, char *argv[])
{
	// Todo: check args for fullscreen
	
	// Initialize glut
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(800, 600);
 
	glutInit(&argc, argv);
	g_glutMainWin = glutCreateWindow("Hex Planet Demo");

	// Initialize glew
	glewInit();

	glutDisplayFunc(glut_Display);
	GLUI_Master.set_glutReshapeFunc(glut_Reshape);
	GLUI_Master.set_glutKeyboardFunc(glut_Keyboard);
	GLUI_Master.set_glutSpecialFunc(NULL);
	GLUI_Master.set_glutMouseFunc(glut_Mouse);
	glutMotionFunc(glut_MotionDraw);
	glutPassiveMotionFunc(glut_Motion);

	// Register idle func with glui
	GLUI_Master.set_glutIdleFunc(glut_Idle);

	// Build GLUI interface
	g_planetGui = new PlanetGui(g_glutMainWin);
	g_planetGui->buildInterface();	
	
	// opengl setup
	//--- depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//--- cull backfaces
	glEnable(GL_CULL_FACE);

	// Call glut main loop  
	glutMainLoop();

	// clean up
	delete g_planetGui;

	return 0;
}
