#pragma once
/**
 * \author Lucas Kramer
 * \file   SimpleRobot.h
 * \brief  The representation of a simple robot
 */

#include "PhysicalObject.h"
#include "Robot.h"
#include "Sensor.h"
#include "configuration.h"

/** \brief A simple robot with uncrossed feedback. */
class SimpleRobot : public Robot {
public:
  SimpleRobot(int radius,
              Color color,
              Color lineColor,
              int targetId = -1,
              Environment *env = Environment::getEnv());

  SimpleRobot(int radius,
              Location loc,
              Color color,
              Color lineColor,
              int targetId = -1,
              Environment *env = Environment::getEnv());

  ~SimpleRobot();

  float getLeftSpeed(float leftLightSensorVal, float rightLightSensorVal,
                     float leftRobotSensorVal, float rightRobotSensorVal,
                     float leftObstacleSensorVal, float rightObstacleSensorVal,
                     float leftTargetSensorVal, float rightTargetSensorVal);
  float getRightSpeed(float leftLightSensorVal, float rightLightSensorVal,
                      float leftRobotSensorVal, float rightRobotSensorVal,
                      float leftObstacleSensorVal, float rightObstacleSensorVal,
                      float leftTargetSensorVal, float rightTargetSensorVal);
};

