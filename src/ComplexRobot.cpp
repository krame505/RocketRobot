/**
 * \author Lucas Kramer
 * \file  ComplexRobot.cpp
 * \brief The representation of a robot within the simulation
 */

 //Can be uncommented for debugging
/*
   #include <iostream>
   using std::cout;
   using std::endl;
   using std::string;
*/

#include <GL/glu.h>
#include <GL/glut.h>
#include <stdexcept>

#include "Environment.h"
#include "Robot.h"
#include "ComplexRobot.h"
#include "configuration.h"
using namespace std;

ComplexRobot::ComplexRobot(int radius, Color color, Color lineColor,
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
                           int targetId,
                           Environment *env) :
Robot(COMPLEX, radius, color, lineColor, targetId, env),
  enableLightSensors(enableLightSensors),
  enableRobotSensors(enableRobotSensors),
  enableObstacleSensors(enableObstacleSensors),
  enableTargetSensors(enableTargetSensors),
  lightSensorsCrossed(lightSensorsCrossed),
  robotSensorsCrossed(robotSensorsCrossed),
  obstacleSensorsCrossed(obstacleSensorsCrossed),
  targetSensorsCrossed(targetSensorsCrossed),
  lightSensorScale(lightSensorScale),
  robotSensorScale(robotSensorScale),
  obstacleSensorScale(obstacleSensorScale),
  targetSensorScale(targetSensorScale),
  defaultSpeed(defaultSpeed) {}

ComplexRobot::ComplexRobot(int radius, Location loc, Color color, Color lineColor,
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
                           int targetId,
                           Environment *env) :
Robot(COMPLEX, radius, loc, color, lineColor, targetId, env),
  enableLightSensors(enableLightSensors),
  enableRobotSensors(enableRobotSensors),
  enableObstacleSensors(enableObstacleSensors),
  enableTargetSensors(enableTargetSensors),
  lightSensorsCrossed(lightSensorsCrossed),
  robotSensorsCrossed(robotSensorsCrossed),
  obstacleSensorsCrossed(obstacleSensorsCrossed),
  targetSensorsCrossed(targetSensorsCrossed),
  lightSensorScale(lightSensorScale),
  robotSensorScale(robotSensorScale),
  obstacleSensorScale(obstacleSensorScale),
  targetSensorScale(targetSensorScale),
  defaultSpeed(defaultSpeed) {}

ComplexRobot::~ComplexRobot() {}

float ComplexRobot::getLeftSpeed(float leftLightSensorVal, float rightLightSensorVal,
                                 float leftRobotSensorVal, float rightRobotSensorVal,
                                 float leftObstacleSensorVal, float rightObstacleSensorVal,
                                 float leftTargetSensorVal, float rightTargetSensorVal) {
  return 
    (enableLightSensors?
     lightSensorsCrossed? rightLightSensorVal : leftLightSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * lightSensorScale +
    (enableRobotSensors?
     robotSensorsCrossed? rightRobotSensorVal : leftRobotSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * robotSensorScale +
    (enableObstacleSensors?
     obstacleSensorsCrossed? rightObstacleSensorVal : leftObstacleSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * obstacleSensorScale +
    (enableTargetSensors?
     targetSensorsCrossed? rightTargetSensorVal : leftTargetSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * targetSensorScale +
    defaultSpeed;
}

float ComplexRobot::getRightSpeed(float leftLightSensorVal, float rightLightSensorVal,
                                  float leftRobotSensorVal, float rightRobotSensorVal,
                                  float leftObstacleSensorVal, float rightObstacleSensorVal,
                                  float leftTargetSensorVal, float rightTargetSensorVal) {
  return
    (enableLightSensors?
     lightSensorsCrossed? leftLightSensorVal : rightLightSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * lightSensorScale +
    (enableRobotSensors?
     robotSensorsCrossed? leftRobotSensorVal : rightRobotSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * robotSensorScale +
    (enableObstacleSensors?
     obstacleSensorsCrossed? leftObstacleSensorVal : rightObstacleSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * obstacleSensorScale +
    (enableTargetSensors?
     targetSensorsCrossed? leftTargetSensorVal : rightTargetSensorVal :
     0) * GET_FLOAT("SPEED_SCALE_FACTOR") * targetSensorScale +
    defaultSpeed;
}
