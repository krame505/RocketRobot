#pragma once
/**
 * \author Lucas Kramer
 * \file   ComplexRobot.h
 * \brief  The representation of a robot within the simulation
 */

#include "PhysicalObject.h"
#include "Robot.h"
#include "Sensor.h"
#include "configuration.h"

/** \brief A complex robot with configurable feedback from all sensors */
class ComplexRobot : public Robot {
public:
  ComplexRobot();
  ComplexRobot(int radius, Color color, Color lineColor,
               bool enableLightSensors,
               bool enableRobotSensors,
               bool enableObstacleSensors,
               bool enableTargetSensors,
               bool lightSensorsCrossed,
               bool robotSensorsCrossed,
               bool obstacleSensorsCrossed,
               bool targetSensorsCrossed,
               float lightSensorScale,
               float robotSensorScale,
               float obstacleSensorScale,
               float targetSensorScale,
               int defaultSpeed,
               int targetId = -1);
  ComplexRobot(int radius, Location loc, Color color, Color lineColor,
               bool enableLightSensors,
               bool enableRobotSensors,
               bool enableObstacleSensors,
               bool enableTargetSensors,
               bool lightSensorsCrossed,
               bool robotSensorsCrossed,
               bool obstacleSensorsCrossed,
               bool targetSensorsCrossed,
               float lightSensorScale,
               float robotSensorScale,
               float obstacleSensorScale,
               float targetSensorScale,
               int defaultSpeed,
               int targetId = -1);

  ~ComplexRobot();

  float getLeftSpeed(float leftLightSensorVal, float rightLightSensorVal,
                     float leftRobotSensorVal, float rightRobotSensorVal,
                     float leftObstacleSensorVal, float rightObstacleSensorVal,
                     float leftTargetSensorVal, float rightTargetSensorVal);
  float getRightSpeed(float leftLightSensorVal, float rightLightSensorVal,
                      float leftRobotSensorVal, float rightRobotSensorVal,
                      float leftObstacleSensorVal, float rightObstacleSensorVal,
                      float leftTargetSensorVal, float rightTargetSensorVal);

  const bool enableLightSensors, enableRobotSensors, enableObstacleSensors, enableTargetSensors;
  const bool lightSensorsCrossed, robotSensorsCrossed, obstacleSensorsCrossed, targetSensorsCrossed;
  const float lightSensorScale, robotSensorScale, obstacleSensorScale, targetSensorScale;
  const int defaultSpeed;
};
