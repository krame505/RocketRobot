/**
 * \author CSci3081 Guru
 * \editor Carl Bahn, Lucas Kramer
 * \file  BaseGfxApp.cpp
 * \brief Implementation of BaseGfxApp
 */

#include "BaseGfxApp.h"

BaseGfxApp* BaseGfxApp::s_currentApp = NULL;
bool BaseGfxApp::s_glutInitialized = false;

BaseGfxApp::BaseGfxApp(int argc, char* argv[],
                       int width, int height,
                       int x, int y,
                       int glutFlags,
                       bool createGLUIWin,
                       int gluiWinX, int gluiWinY) {
  s_currentApp = this;
  m_glui = NULL;
  m_drag = false;
  m_width = width;
  m_height = height;

  // Set window size and position
  glutInitWindowSize(width, height);
  glutInitWindowPosition(x, y);   
  glutInitDisplayMode(glutFlags);

  if (! s_glutInitialized) {
    glutInit(&argc, argv);
    s_glutInitialized = true;
  }

  m_glutWindowHandle = glutCreateWindow("Graphics Window");

  glutReshapeFunc(s_reshape);
  glutKeyboardFunc(s_keyboard);
  glutKeyboardUpFunc(s_keyboardup);
  glutSpecialFunc(s_keyboardspecial);
  glutSpecialUpFunc(s_keyboardspecialup);
  glutMotionFunc(s_mousemotion);
  glutPassiveMotionFunc(s_mousemotion);
  glutMouseFunc(s_mousebtn);
  glutDisplayFunc(s_draw);

  if (createGLUIWin) {
    //    m_glui = GLUI_Master.create_glui("Controls", 0, gluiWinX, gluiWinY);
    m_glui = GLUI_Master.create_glui_subwindow(m_glutWindowHandle, GLUI_SUBWINDOW_RIGHT);
    GLUI_Master.auto_set_viewport();

    m_glui->set_main_gfx_window(m_glutWindowHandle);
    // Note: if using a glut idle func, it may need to be registered with glui rather than glut.
    GLUI_Master.set_glutIdleFunc(NULL);
  }
}

BaseGfxApp::~BaseGfxApp() {
  s_currentApp = NULL;
  glutDestroyWindow(m_glutWindowHandle);
}

void BaseGfxApp::graphicsTimer(int FPS) {
  //Begin the countdown to the next frame
  glutTimerFunc(1000/FPS, BaseGfxApp::graphicsTimer, FPS);

  //process the movement that takes place between frames
  s_currentApp->advance();
  
  //redraw updated frame
  s_currentApp->display();
}

void BaseGfxApp::graphicsBegin(int FPS, int delay) {
  // Set up graphics
  glutTimerFunc(delay, // msec delay before first frame
                BaseGfxApp::graphicsTimer, // See above
                FPS); // framerate
}

void BaseGfxApp::s_draw() {
  //s_currentApp->display();

  //glutPostRedisplay calls the function specified by glutDisplayFunc
  //Which in this case is s_draw
  //glutPostRedisplay();
}

void BaseGfxApp::s_reshape(int width, int height) {
  s_currentApp->reshape(width, height);
}

void BaseGfxApp::s_keyboard(unsigned char c, int x, int y) {
  s_currentApp->keyboard(c, x, y);
  // glutPostRedisplay();

}

void BaseGfxApp::s_keyboardspecial(int key, int x, int y) {
  s_currentApp->keyboardSpecial(key, x, y);
  // glutPostRedisplay();

}

void BaseGfxApp::s_keyboardup(unsigned char c, int x, int y) {
  s_currentApp->keyboardUp(c, x, y);
  // glutPostRedisplay();

}

void BaseGfxApp::s_keyboardspecialup(int key, int x, int y) {
  s_currentApp->keyboardSpecialUp(key, x, y);
  // glutPostRedisplay();
}

void BaseGfxApp::s_mousemotion(int x, int y) {
  if (s_currentApp->m_drag == true) {
    s_currentApp->mouseDragged(x,y);
  } else {
    s_currentApp->mouseMoved(x,y);
  }
  // glutPostRedisplay();

}

void BaseGfxApp::s_mousebtn(int b, int s, int x, int y) {
  if ((b==GLUT_LEFT_BUTTON) && (s==GLUT_UP)) {
    s_currentApp->leftMouseUp(x,y);
    s_currentApp->m_drag = false;

  }
  else if ((b==GLUT_LEFT_BUTTON) && (s==GLUT_DOWN)) {
    s_currentApp->leftMouseDown(x,y);
    s_currentApp->m_drag = true;
  }
  else if ((b==GLUT_RIGHT_BUTTON) && (s==GLUT_UP)) {
    s_currentApp->rightMouseUp(x,y);
  }
  else if ((b==GLUT_RIGHT_BUTTON) && (s==GLUT_DOWN)) {
    s_currentApp->rightMouseDown(x,y);
  }
  else if ((b==GLUT_MIDDLE_BUTTON) && (s==GLUT_UP)) {
    s_currentApp->middleMouseUp(x,y);
  }
  else if ((b==GLUT_MIDDLE_BUTTON) && (s==GLUT_DOWN)) {
    s_currentApp->middleMouseDown(x,y);
  }
  // glutPostRedisplay();

}

void BaseGfxApp::s_gluicallback(int controlID) {
  s_currentApp->gluiControl(controlID);
}

void BaseGfxApp::runMainLoop() {
  glutMainLoop();
}


int BaseGfxApp::width() const {
  return m_width;
}

int BaseGfxApp::height() const {
  return m_height;
}

void BaseGfxApp::setCaption(const std::string& caption) {
  glutSetWindowTitle(caption.c_str());
  glutSetIconTitle(caption.c_str());
}

void BaseGfxApp::reshape(int width, int height) {
  if (s_currentApp->width() != width || s_currentApp->height() != height) {
    glutReshapeWindow(s_currentApp->width(), s_currentApp->height());
  }
}


