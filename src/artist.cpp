/**
 * \author Carl Bahn
 * \editor Lucas Kramer
 * \file  artist.h
 * \brief A namespace for OpenGL graphical functions
 */

#include "Color.h"
#include "artist.h"
#include "Environment.h"
#include "configuration.h"

#include <GL/glut.h>
#include <GL/glu.h>
#define _USE_MATH_DEFINES
#include <math.h>

/* Anonymous namespace. Inaccessable outside this file */
namespace {

/* this is a useful function to quickly throw
together a color from rgb256 values */
  Color colorPicker(int red, int green, int blue) {
    Color newColor(((float)red)/256.0, ((float)green)/256.0, ((float)blue)/256.0);
    return newColor;
  }

  void directionLine(Color lineColor, int robotRadius) {
    int radius = robotRadius + 10;
    glBegin(GL_POLYGON);
    glColor3f(lineColor.red, lineColor.green, lineColor.blue);
    glVertex2f(-3,5);
    glVertex2f(3,5);
    glVertex2f(3, radius);
    glVertex2f(-3, radius);
    glEnd();
  }

  void circle(int radius, Color color) {
    int slices_per_circle = GET_INT("SLICES_PER_CIRCLE");
    
    glBegin(GL_POLYGON);
    glColor3f(color.red, color.green, color.blue);
    for (int i = 0; i < slices_per_circle; i++)
      glVertex2f((radius * cos(i * 2.0 * M_PI / slices_per_circle)),
                 (radius * sin(i * 2.0 * M_PI / slices_per_circle)));
    glEnd();
  }
}

namespace artist {
  void drawBackground(Color color) {
    glBegin(GL_POLYGON);
    glColor3f(color.red, color.green, color.blue);
    glVertex2f(0, 0);
    glVertex2f(GET_INT("DISPLAY_WIDTH"), 0);
    glVertex2f(GET_INT("DISPLAY_WIDTH"), 1);
    glVertex2f(0, 1);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(color.red, color.green, color.blue);
    glVertex2f(0, Environment::getEnv()->getHeight());
    glVertex2f(Environment::getEnv()->getWidth(), Environment::getEnv()->getHeight());
    glVertex2f(Environment::getEnv()->getWidth(), Environment::getEnv()->getHeight() - 1);
    glVertex2f(0, Environment::getEnv()->getHeight() - 1);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(color.red, color.green, color.blue);
    glVertex2f(0, 0);
    glVertex2f(0, Environment::getEnv()->getHeight());
    glVertex2f(1, Environment::getEnv()->getHeight());
    glVertex2f(1, 0);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3f(color.red, color.green, color.blue);
    glVertex2f(Environment::getEnv()->getWidth(), 0);
    glVertex2f(Environment::getEnv()->getWidth(), Environment::getEnv()->getHeight());
    glVertex2f(Environment::getEnv()->getWidth() - 1, Environment::getEnv()->getHeight());
    glVertex2f(Environment::getEnv()->getWidth() - 1, 0);
    glEnd();
  }

  void drawObject(Location loc, int radius, Color color) {
    glPushMatrix();
    glTranslatef(loc.x, loc.y, 0.0f);
    circle(radius, color);
    glPopMatrix();
  }

  void debugArrow(Location loc, int orientation) {
    glPushMatrix();
    glTranslatef(loc.x, loc.y, 0.0f);
    glRotatef(orientation, 0,0,-1); //Rotate about z-axis
    Color c = colorPicker(242,190,121);
    
    glBegin(GL_POLYGON);
    glColor3f(c.red, c.green, c.blue);
    glVertex2f(-3,5);
    glVertex2f(3,5);
    glVertex2f( 0, 37);
    glEnd();
    
    glPopMatrix();
  }

  void drawLight(Location loc, int radius, Color color) {
    int slices_per_circle = GET_INT("SLICES_PER_CIRCLE");
    int aura_radius = GET_INT("LIGHT_AURA_RADIUS");
    glPushMatrix();
    glTranslatef(loc.x, loc.y, 0.0f);
    
    glEnable(GL_BLEND);
    //This function contributes to how objects look while intersecting the light
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Light halo
    Color auraColor = GET_COLOR("LIGHT_AURA_COLOR");
    glBegin(GL_POLYGON);
    glColor4f(auraColor.red, auraColor.green, auraColor.blue, 1);
    glVertex2f(0,0);
    glColor4f(auraColor.red, auraColor.green, auraColor.blue, 0);
    for (int i = 0; i <= slices_per_circle; i++) {
      glVertex2f((aura_radius * cos(i * 2.0 * M_PI / slices_per_circle)),
                 (aura_radius * sin(i * 2.0 * M_PI / slices_per_circle)));
    }
    glEnd();
    glDisable(GL_BLEND);
    
    //Light center
    circle(radius, color);

    glPopMatrix();
  }
  
  void drawObstacle(Location loc, int radius) {
    glPushMatrix();
    glTranslatef(loc.x, loc.y, 0.0f);
    circle(radius, GET_COLOR("OBSTACLE_COLOR"));
    glPopMatrix();
  }
  
  void drawSensor(Location loc, int orientation, int angle, float intensity) {
    glPushMatrix();
    glTranslatef(loc.x, loc.y, 0.0f);
    glRotatef(orientation, 0,0,-1); //Rotate about z-axis
    
    int radius = 17;
    int slices_per_arc = angle/19; //Aproximately every 10 degrees
    float angle_per_slice = ((float)(angle)) / ((float)(slices_per_arc));
    glBegin(GL_POLYGON);
    glColor3f(179.0/256.0, 255.0/256.0, 250.0/256.0);
    glVertex2f(0,0);
    float currentAngle = - (float)angle / 2.0;
    for (int i = 0; i <= slices_per_arc; i++) {
      glVertex2f((radius * sin(currentAngle * M_PI/180.0)),
                 (radius * cos(currentAngle * M_PI/180.0)));
      currentAngle += angle_per_slice;
    }
    glEnd();
    int redvalue = intensity*256.0;
    circle(8, colorPicker(redvalue, 86,97));
    
    glPopMatrix();
  }
  
  void drawRobot(Location loc, int radius, int orientation, Color color, Color lineColor) {
    glPushMatrix();
    glTranslatef(loc.x, loc.y, 0.0f);
    glRotatef(orientation, 0,0,-1);
    
    circle(radius, color);
    directionLine(lineColor, radius);

    glPopMatrix();
  }
}
