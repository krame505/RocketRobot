#pragma once

/**
 * \author Himawan Go
 * \author Carl Bahn
 *
 * \file  Sensor.h
 * \brief A sensor that detects certain objects in the environment
 */

#include "Color.h"
#include "Location.h"
#include "configuration.h"
#include "PhysicalObject.h"

class Sensor {
public:
  /**
   * Sensor constructor with default angle
   * \param loc Location of the sensor in x and y
   * \param orientation orientation of the sensor
   * \param typeDetected type that sensor will detect
   */
  Sensor(Location loc, int orientation,
         ObjectType typeDetected,
         Environment *env = Environment::getEnv());
  
  /**
   * Sensor constructor
   * \param loc Location of the sensor in x and y
   * \param orientation orientation of the sensor
   * \param viewAngle view angle of the sensor in degrees
   * \param typeDetected type that sensor will detect
   */
  Sensor(Location loc, int orientation, int viewAngle,
         ObjectType typeDetected,
         Environment *env = Environment::getEnv());

  //Location of sensor is relitive to robot sensor.
    
  /**
   * Sets the position for the sensor
   * \param x x position in pixels
   * \param y y position in pixels
   */
  void setPosition(float x, float y);
    
  /**
   * Sets the position for the sensor
   * \param loc A struct of the x and y Location in pixels
   */
  void setPosition(Location loc);
    
  /**
   * Returns a Location struct of x and y position in pixels of the sensor
   * \return A Location struct of x and y position
   */
  Location getPosition();
  
  /**
   * Returns the x position in pixels of the sensor
   * \return x position
   */
  float getXPosition();
  
  /**
   * Returns the y position in pixels of the sensor
   * \return y position
   */
  float getYPosition();
  
  /**
   * Sets the orientation for the sensor
   * \param orientation The orientation of the object in degrees
   */
  void setOrientation(int orientation);
  
  /**
   * Returns the orientation of the sensor
   * \return orientation in degrees
   */
  int getOrientation();
  
  /**
   * Sets the view angle of the sensor
   * \param degrees The view angle of the sensor in degrees
   */
  void setViewAngle(int degrees);
  
  /**
   * Returns the view angle of the sensor
   * \return view angle in degrees
   */
  int getViewAngle();

  /**
   * Sets the object type that the sensor detects
   * \author Himawan
   * \param type object type to detect
   */
  void setTypeDetected(ObjectType type);
  
  /**
   * Returns the type that sensor detects
   * \author Himawan
   * \return type the sensor detects
   */
  ObjectType getTypeDetected();

  /**
   * Called to update the position of the sensor 
   * match the updated position of the robot 
   * \author Carl
   */
  void updatePosition(Location robotLoc, int robotAngle);

  /**
   * Displays the sensor 
   * \author Carl
   */
  void display();
  
  /**
   * Returns some strength by sensing the environment
   * \author Carl & Himawan
   * \return the strength of sensor reading of [0..1]
   */
  float sense();

private:
  Environment *env;

  Location loc, absoluteLoc;
  int orientation, absoluteOrientation;
  int viewAngle;
  ObjectType typeDetected;
  float latestReading;
};
