#pragma once

/**
 * \author CSci3081 Guru
 * \editor Carl Bahn, Lucas Kramer
 * \file  BaseGfxApp.h
 * \brief The basic application class for CSci-3081 project.  Uses GLUT and 
 * GLUI and wraps them in a nice C++ interface.
 */

// Frequently used C++ stuff 
#include <string>
#include <iostream>
#include <assert.h>

// The GLUI library, which in turn will include gl.h and glut.h
#include <GL/glui.h>

/** \brief Graphics driver class */
class BaseGfxApp {
 public:
  BaseGfxApp(int argc, char* argv[], int width, int height, int x, int y, int glutFlags, 
             bool createGLUIWin, int gluiWinX, int gluiWinY);
  virtual ~BaseGfxApp();

  /** This function starts the graphics after a delay.*/
  static void graphicsBegin(int v, int delay);

  void setCaption(const std::string& caption);
  void runMainLoop();

  virtual void display() {};
  virtual void advance() {};

  virtual void mouseMoved(int x, int y) {};
  virtual void mouseDragged(int x, int y) {};

  virtual void leftMouseDown(int x, int y) {};
  virtual void leftMouseUp(int x, int y) {};
  virtual void rightMouseDown(int x, int y) {};
  virtual void rightMouseUp(int x, int y) {};
  virtual void middleMouseDown(int x, int y) {};
  virtual void middleMouseUp(int x, int y) {};

  virtual void keyboard(unsigned char c, int x, int y) {};
  virtual void keyboardSpecial(int key, int x, int y) {};
  virtual void keyboardUp(unsigned char c, int x, int y) {};
  virtual void keyboardSpecialUp(int key, int x, int y) {};

  virtual void reshape(int width, int height);

  virtual void gluiControl(int controlID) {};

  int width() const;
  int height() const;
  int handle() { return m_glutWindowHandle; }
  GLUI* glui() { return m_glui; }

 protected:
  /** This function calls itself using glutTimerFunc once per frame.*/
  static void graphicsTimer(int v);

  /** GLUT and GLUI event callbacks are sent to the current window/app. Right now, there is only
      one window anyway (not counting the GLUI UI window.. in the future could be extended to 
      support more windows.  In any case, some structure like this is always needed when using 
      glut with C++, since the glut callbacks must be either global or static functions. */
  static BaseGfxApp* s_currentApp;  

  /** Has glutInit been called? (only allowed once per program) */
  static bool s_glutInitialized;

  // Glut callbacks:
  static void s_reshape(int width, int height);
  static void s_keyboard(unsigned char c, int x, int y);
  static void s_keyboardspecial(int key, int x, int y);
  static void s_keyboardup(unsigned char c, int x, int y);
  static void s_keyboardspecialup(int key, int x, int y);
  static void s_mousemotion(int x, int y);
  static void s_mousebtn(int b, int s, int x, int y);
  static void s_draw();    
  static void s_gluicallback(int controlID);


  /** Underlying glut window handle */
  int m_glutWindowHandle;

  // Pointer to GLUI master
  GLUI* m_glui;

  bool m_drag;
  int m_width;
  int m_height;
};
