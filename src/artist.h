#pragma once

/**
 * \author Carl Bahn
 * \editor Lucas Kramer
 * \file   artist.h
 * \brief  A namespace for OpenGL graphical functions
 */

#include "Color.h"
#include "Location.h"
#include "configuration.h"

/**
 * \brief graphical commands 
 * \author Carl
 * artist functions provide a contained way to display graphics without
 * putting OpenGL graphics in each file
 */
namespace artist {
  /**
   * \author Lucas Kramer
   * Draws a rectangle the size of the drawing area
   * \param color the color of the background
   */
  void drawBackground(Color color = GET_COLOR("BACKGROUND_COLOR"));

  void debugArrow(Location loc, int orientation);

  /**
   * \author Lucas Kramer
   * Draws the default Object, which is a circle 
   * \param loc absolute Location to draw center of circle
   * \param radius radius of circle
   * \param color the color of the circle
   */
  void drawObject(Location loc, int radius, Color color);
  
  /** Draws a LightSource
   * \param loc absolute Location to draw center of light
   * \param radius radius of 'bulb' of light
   */  
  void drawLight(Location loc, int radius, Color color = GET_COLOR("LIGHT_COLOR"));
  
/**
 * Draws an obstacle 
 * \param loc absolute Location to draw center of Obstacle
 * \param radius radius of Obstacle
 */
  void drawObstacle(Location loc, int radius);

/**
 * Draws a sensor
 * \param loc absolute Location to draw center of Sensor
 * \param orientation absolute direction of sensor
 * \param angle number of degrees that sensor senses in, with orientation in center
 * \param intensity redness of sensor as value between 0 and 1, meant to indicate amount of light detected
 */
  void drawSensor(Location loc, int orientation, int angle, float intensity);
  
/**
 * Draws a Robot
 * \param loc absolute Location of center of robot
 * \param orientation absolute direction for robot to face
 * \param color the color of the robot's body
 * \param lineColor the color of the robot's direction line
 */
  void drawRobot(Location loc, int radius, int orientation, Color color = GET_COLOR("ROBOT_COLOR"), Color lineColor = Color('?'));
}
