#pragma once

/**
 * \author Himawan Go
 * \editor Lucas Kramer
 * \file  Robot.h
 * \brief The representation of robot within the simulation
 */

#include "PhysicalObject.h"
#include "Sensor.h"
#include "configuration.h"

enum RobotType {SIMPLE, COMPLEX, NEURAL_NETWORK};

/** \brief Robot that moves around the window and bumps into obstacles */
class Robot : public PhysicalObject {
public:

  /**
   * \param robotType the robot type
   * \param radius the radius in pixels
   * \param color color of the robot itself, a struct of float value 0 to 1 representing a hexedecimal color
   * \param lineColor color of the line, a struct of float value 0 to 1 representing a hexedecimal color
   * \par Initialization
   * Robot is initialized with the following values:
   * <tr><td>Orientation: 0</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  Robot(RobotType robotType, int radius, Color color, Color lineColor, int targetId = -1);

  /**
   * \param robotType the robot type
   * \param radius the radius in pixels
   * \param color a struct of float value 0 to 1 representing a hexedecimal color
   * \param linecolor color of the line, a struct of float value 0 to 1 representing a
   * hexedecimal color
   * \param loc start Location of the robot
   * \par Initialization
   * Robot is initialized with the following values:
   * <tr><td>Orientation: random</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  Robot(RobotType robotType, int radius, Location loc, Color color, Color lineColor, int targetId = -1);

  /** This is the class destructor.  */
  virtual ~Robot();

  const RobotType robotType;

  /**
   * \author Lucas Kramer
   * This function gets the id of the target for the robot to seek.
   * \param id the id of the target
   */
  int getTarget() const;

  /**
   * \author Lucas Kramer
   * This function sets the id of the target for the robot to seek.
   * \returns the target id
   */
  void setTarget(int id);

  /**
   * \author Lucas Kramer
   * This function gives Robot an appropriate reaction when a collision occurs
   * \param otherId id of the object that it collides with
   * \param wasHit tells if the object was hit previously
   */
  bool handleCollision(int otherId, bool wasHit);

  /** 
   * \author Lucas Kramer
   * This function updates the Robot's wheel speeds from the sensors
   */
  void update();

  /** 
   * \author Lucas Kramer
   * This function updates the position of the robot's sub-objects
   */
  void updateMembers();

  /** 
   * \author Lucas Kramer
   * This function displays the Robot in the graphics
   */
  void display();

  /**
   * \author Lucas Kramer
   * This function sets a new color for the robot's direction line.
   * \param lineColor color of the line, a struct of float value 0 to 1
   * representing a hexedecimal color
   */
  void setLineColor(Color lineColor);
  Color getLineColor();

protected:
  virtual float getLeftSpeed(float leftLightSensorVal, float rightLightSensorVal,
                             float leftRobotSensorVal, float rightRobotSensorVal,
                             float leftObstacleSensorVal, float rightObstacleSensorVal,
                             float leftTargetSensorVal, float rightTargetSensorVal) = 0;
  virtual float getRightSpeed(float leftLightSensorVal, float rightLightSensorVal,
                              float leftRobotSensorVal, float rightRobotSensorVal,
                              float leftObstacleSensorVal, float rightObstacleSensorVal,
                              float leftTargetSensorVal, float rightTargetSensorVal) = 0;

private:
  Sensor leftLightSensor, rightLightSensor;
  Sensor leftRobotSensor, rightRobotSensor;
  Sensor leftObstacleSensor, rightObstacleSensor;
  Sensor leftTargetSensor, rightTargetSensor;
  Color lineColor, defaultColor;
  int lastUpdateTime, pauseTime;
  int targetId;

  /**
   * This function computes the combined speed from the wheel speeds
   * \param leftSpeed the speed of the left wheel
   * \param rightSpeed the speed of the right wheel
   * \return The new speed
   */
  float getNewSpeed(float leftSpeed, float rightSpeed);

  /**
   * This function computes the amount to rotate in the next step from the wheel speeds
   * \param leftSpeed the speed of the left wheel
   * \param rightSpeed the speed of the right wheel
   * \return The amount to rotate
   */
  int getNewRotation(float leftSpeed, float rightSpeed);
};

