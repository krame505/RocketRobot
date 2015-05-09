/**
 * \author Lucas Kramer
 * \editor Himawan Go, Carl Bahn
 * \file  Robot.cpp
 * \brief The representation of robot within the simulation
 */

/* //Can be uncommented for debugging
   #include <iostream>
   using std::cout;
   using std::endl;
   using std::string;
*/

#include <stdexcept>
#include <math.h>

#include "artist.h"
#include "environment.h"
#include "Robot.h"
#include "configuration.h"
using namespace std;
using namespace environment;

Robot::Robot(RobotType robotType,
             int radius,
             Color color,
             Color lineColor,
             int targetId) :
  Robot(robotType, radius, findOpenLocation(radius), color, lineColor, targetId) {}

Robot::Robot(RobotType robotType,
             int radius,
             Location loc,
             Color color,
             Color lineColor,
             int targetId) :
  PhysicalObject(ROBOT, radius, loc, color, GET_BOOL("ROBOTS_HITABLE")),
  robotType(robotType),
  leftLightSensor (   Location(-GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_LEFT"),  LIGHT),
  rightLightSensor(   Location( GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_RIGHT"), LIGHT),
  leftRobotSensor (   Location(-GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_LEFT"),  ROBOT),
  rightRobotSensor(   Location( GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_RIGHT"), ROBOT),
  leftObstacleSensor (Location(-GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_LEFT"),  OBSTACLE),
  rightObstacleSensor(Location( GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_RIGHT"), OBSTACLE),
  leftTargetSensor (  Location(-GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_LEFT"),  targetId != -1? getObject(targetId)->objectType : TARGET),
  rightTargetSensor(  Location( GET_FLOAT("SENSOR_POSITION_X"), GET_FLOAT("SENSOR_POSITION_Y")), GET_INT("SENSOR_ANGLE_RIGHT"), targetId != -1? getObject(targetId)->objectType : TARGET),
  lineColor(lineColor),
  defaultColor(color),
  lastUpdateTime(0),
  pauseTime(0),
  targetId(targetId) {
  updateMembers();
  }

Robot::~Robot() {}

int Robot::getTarget() const {
  return targetId;
}

void Robot::setTarget(int id) {
  if (id < 0){
    throw new std::invalid_argument("setTarget: Invalid ID.");
  }

  targetId = id;
}

bool Robot::handleCollision(int otherId, bool wasHit) {
  if (getColor() != GET_COLOR("COLLISION_COLOR") &&
      getColor() != GET_COLOR("UPDATE_COLOR"))
  defaultColor = getColor();
  setColor(GET_COLOR("COLLISION_COLOR"));

  pauseTime = GET_INT("POST_COLLISION_PAUSE");

  if (otherId == targetId && targetId != -1) {
    removeObject(targetId);
    removeObject(getId());
    return true;
  }
  GET_BOOL("TRANSLATE_REORIENT");
  reorient(GET_BOOL("OPPOSITE_ANGLES") && wasHit?
           GET_INT("REORIENT_ANGLE") :
           -GET_INT("REORIENT_ANGLE"), GET_INT("REORIENT_DISTANCE"));

  lastUpdateTime = GET_INT("REORIENT_TIME") - GET_INT("POST_COLLISION_DELAY");
  return false;
}

void Robot::update() {
  lastUpdateTime++;
  if (lastUpdateTime >= GET_INT("REORIENT_TIME")) {
    lastUpdateTime = 0;
    if (getColor() != GET_COLOR("COLLISION_COLOR") &&
        getColor() != GET_COLOR("UPDATE_COLOR"))
      defaultColor = getColor();
    if (GET_BOOL("UPDATE_CHANGE_COLOR"))
      setColor(GET_COLOR("UPDATE_COLOR"));
  }
  if (GET_BOOL("ENABLE_SENSORS")) {
    float leftLightSensorVal     = leftLightSensor.sense();
    float rightLightSensorVal    = rightLightSensor.sense();
    float leftRobotSensorVal     = leftRobotSensor.sense();
    float rightRobotSensorVal    = rightRobotSensor.sense();
    float leftObstacleSensorVal  = leftObstacleSensor.sense();
    float rightObstacleSensorVal = rightObstacleSensor.sense();
    float leftTargetSensorVal    = targetId != -1? leftTargetSensor.sense() : 0;
    float rightTargetSensorVal   = targetId != -1? rightTargetSensor.sense() : 0;
    float leftSpeed  = getLeftSpeed(leftLightSensorVal, rightLightSensorVal,
                                    leftRobotSensorVal, rightRobotSensorVal,
                                    leftObstacleSensorVal, rightObstacleSensorVal,
                                    leftTargetSensorVal, rightTargetSensorVal);
    float rightSpeed = getRightSpeed(leftLightSensorVal, rightLightSensorVal,
                                     leftRobotSensorVal, rightRobotSensorVal,
                                     leftObstacleSensorVal, rightObstacleSensorVal,
                                     leftTargetSensorVal, rightTargetSensorVal);
    if (pauseTime > 0) {
      setSpeed(0);
      pauseTime--;
    }
    else
      setSpeed(getNewSpeed(leftSpeed, rightSpeed));

    int rotateAmount = getNewRotation(leftSpeed, rightSpeed);
    if (rotateAmount < -GET_INT("MAX_ROTATION"))
      rotateAmount = -GET_INT("MAX_ROTATION");
    if (rotateAmount > GET_INT("MAX_ROTATION"))
      rotateAmount = GET_INT("MAX_ROTATION");
    rotate(rotateAmount);
  }
}

void Robot::updateMembers() {
  rightLightSensor.updatePosition(getLocation(), getOrientation());
  leftLightSensor.updatePosition(getLocation(), getOrientation());
  rightRobotSensor.updatePosition(getLocation(), getOrientation());
  leftRobotSensor.updatePosition(getLocation(), getOrientation());
  rightObstacleSensor.updatePosition(getLocation(), getOrientation());
  leftObstacleSensor.updatePosition(getLocation(), getOrientation());
  rightTargetSensor.updatePosition(getLocation(), getOrientation());
  leftTargetSensor.updatePosition(getLocation(), getOrientation());
}

float Robot::getNewSpeed(float leftSpeed, float rightSpeed) {
  float correctedLeftSpeed =
    min((float)GET_INT("ROBOT_MAX_SPEED"),
        max((float)GET_INT("ROBOT_MIN_SPEED"),
            leftSpeed));
  float correctedRightSpeed =
    min((float)GET_INT("ROBOT_MAX_SPEED"),
        max((float)GET_INT("ROBOT_MIN_SPEED"),
            rightSpeed));
  return (correctedLeftSpeed + correctedRightSpeed) / 2.0;
}

int Robot::getNewRotation(float leftSpeed, float rightSpeed) {
  float correctedLeftSpeed =
    min((float)GET_INT("ROBOT_MAX_SPEED"),
        max((float)GET_INT("ROBOT_MIN_SPEED"),
            leftSpeed));
  float correctedRightSpeed =
    min((float)GET_INT("ROBOT_MAX_SPEED"),
        max((float)GET_INT("ROBOT_MIN_SPEED"),
            rightSpeed));
  return (correctedLeftSpeed - correctedRightSpeed) * GET_FLOAT("ROTATION_SCALE_FACTOR");
}

// Graphics
void Robot::setLineColor(Color newColor) {
  lineColor = newColor;
}

Color Robot::getLineColor() {
  return lineColor;
}

void Robot::display() {
  artist::drawRobot(getLocation(), getRadius(), getOrientation(), getColor(), getLineColor());
  switch (GET_INT("DISPLAY_SENSOR")) {
  case 1:
    leftLightSensor.display();
    rightLightSensor.display();
    break;
  case 2:
    leftRobotSensor.display();
    rightRobotSensor.display();
    break;
  case 3:
    leftObstacleSensor.display();
    rightObstacleSensor.display();
    break;
  case 4:
    leftTargetSensor.display();
    rightTargetSensor.display();
    break;
  }

  // Have to set the color after calling display, or it gets reset immediatly.
  if (isCollidingWithHitable(getId())) {
    if (getColor() != GET_COLOR("COLLISION_COLOR") &&
        getColor() != GET_COLOR("UPDATE_COLOR"))
      defaultColor = getColor();
    setColor(GET_COLOR("COLLISION_COLOR"));
  }
  else if (getColor() == GET_COLOR("COLLISION_COLOR") ||
           getColor() == GET_COLOR("UPDATE_COLOR"))
    setColor(defaultColor);
}
