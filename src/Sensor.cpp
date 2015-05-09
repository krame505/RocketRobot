/**
 * \author Himawan Go
 * \editor Carl Bahn Lucas Kramer
 *
 * \file  Sensor.cpp
 * \brief A sensor that detects certain objects in the environment
 */

#include "artist.h"
#include "Sensor.h"
#include "environment.h"
using namespace environment;
#include "configuration.h"
#include "PhysicalObject.h" /* ObjectType definition */


#include <iostream>
using namespace std;

// Needed on some platforms to access the definition of pi, etc.  
#define _USE_MATH_DEFINES
#include <math.h>

#include <stdexcept> /* invalid_argument */
using std::invalid_argument;

Sensor::Sensor(Location loc, int orientation, ObjectType typeDetected) :
	Sensor(loc, orientation, GET_INT("SENSOR_VIEWANGLE"), typeDetected) {}

Sensor::Sensor(Location loc, int orientation, int viewAngle, ObjectType typeDetected) {
  setPosition(loc);
  setOrientation(orientation);
  setViewAngle(viewAngle);
  setTypeDetected(typeDetected);
}

void Sensor::setPosition(Location loc) {
  this->loc = loc;
}

void Sensor::setPosition(float x, float y) {
  loc.x = x;
  loc.y = y;
}

Location Sensor::getPosition() {
  return loc;
}

float Sensor::getXPosition() {
  return loc.x;
}

float Sensor::getYPosition() {
  return loc.y;
}

void Sensor::setOrientation(int orientation) {
  if (0 > orientation || orientation > 360) {
    throw new invalid_argument("setOrientation: Angle out of range.");
  }

  this->orientation = orientation;
}

int Sensor::getOrientation() {
  return orientation;
}

void Sensor::setViewAngle(int degrees) {
  if (0 > degrees || degrees > 360) {
    throw new invalid_argument("setViewAngle: Angle out of range.");
  }
  viewAngle = degrees;
}

int Sensor::getViewAngle() {
  return viewAngle;
}

void Sensor::setTypeDetected(ObjectType type) {
  typeDetected = type;
}

ObjectType Sensor::getTypeDetected() {
  return typeDetected;
}

void Sensor::display() {
  artist::drawSensor(absoluteLoc, absoluteOrientation, getViewAngle(), latestReading);
}

void Sensor::updatePosition(Location robotLoc, int robotAngle) {
  float cos_v = cos(robotAngle * M_PI / 180);
  float sin_v = sin(robotAngle * M_PI / 180);
  absoluteOrientation = (robotAngle + orientation) % 360;
  absoluteLoc.x = robotLoc.x + cos_v * loc.x + sin_v * loc.y;
  absoluteLoc.y = robotLoc.y - sin_v * loc.x + cos_v * loc.y; 
}

float Sensor::sense() {
  float distanceSquared, delta_x, delta_y;
  int absoluteAngleToLight, angle;

  float strength = 0.0;
  for (objectIterator it = getObjectsBegin(); it != getObjectsEnd(); it++) {
    if ((*it) != NULL && (*it)->objectType == typeDetected) {
      // The angle between two objects
      // Angle = atan2 (delta x, delta y)
      delta_x = (*it)->getXPosition() - absoluteLoc.x;
      delta_y = (*it)->getYPosition() - absoluteLoc.y;
      absoluteAngleToLight = (int)(atan2(delta_x, delta_y) * 180 / M_PI);
      angle = (absoluteAngleToLight + 720 - absoluteOrientation) % 360;
      if (angle > 180)
        angle -= 360;
      /* At this point, normalizedAngle is between -180 and 180 and represents the
       * angle between the directon of the sensor and the light */

      /* Scaling Factor to reduce brightness gradually as light is further from
       * being directly in front of sensor. Always within [0,1] */
      float angleBrightnessScale = 1.0 / exp(3 * pow(2 * angle / getViewAngle(), 2));
      
      // Instead of squareroot-ing then square-ing to find distance, we do neither
      distanceSquared  = pow(((*it)->getXPosition() - absoluteLoc.x), 2.0);
      distanceSquared += pow(((*it)->getYPosition() - absoluteLoc.y), 2.0);
      strength += angleBrightnessScale / distanceSquared;
    }
  }//End for

  // Treat walls as obstacles
  if (typeDetected == OBSTACLE) {
    float distance;
    float delta_x_left = absoluteLoc.y;
    float delta_x_right = GET_INT("DISPLAY_WIDTH") - absoluteLoc.x;
    float delta_y_top = GET_INT("DISPLAY_HEIGHT") - absoluteLoc.y;
    float delta_y_bottom = absoluteLoc.x;

    if (absoluteOrientation == 0) {
      distance = delta_y_top;
    }
    else if (absoluteOrientation > 0 && absoluteOrientation < 90) {
      float cos_v = cos(absoluteOrientation * M_PI / 180);
      float sin_v = sin(absoluteOrientation * M_PI / 180);
      distance = fmin(delta_x_right / cos_v, delta_y_top / sin_v);
    }
    else if (absoluteOrientation == 90) {
      distance = delta_x_right;
    }
    else if (absoluteOrientation > 90 && absoluteOrientation < 180) {
      float cos_v = cos((absoluteOrientation - 90) * M_PI / 180);
      float sin_v = sin((absoluteOrientation - 90) * M_PI / 180);
      distance = fmin(delta_x_right / cos_v, delta_y_bottom / sin_v);
    }
    else if (absoluteOrientation == 180) {
      distance = delta_y_bottom;
    }
    else if (absoluteOrientation > 180 && absoluteOrientation < 270) {
      float cos_v = cos((absoluteOrientation - 180) * M_PI / 180);
      float sin_v = sin((absoluteOrientation - 180) * M_PI / 180);
      distance = fmin(delta_x_left / sin_v, delta_y_bottom / cos_v);
    }
    else if (absoluteOrientation == 270) {
      distance = delta_x_right;
    }
    else { // if (absoluteOrientation > 270 && absoluteOrientation < 360)
      float cos_v = cos((absoluteOrientation - 270) * M_PI / 180);
      float sin_v = sin((absoluteOrientation - 270) * M_PI / 180);
      distance = fmin(delta_x_left / sin_v, delta_y_top / cos_v);
    }

    strength += GET_FLOAT("WALL_OBSTACLE_SCALE") / pow(distance, 2.0);
  }
  
  strength *= GET_FLOAT("SENSOR_SCALE");
  if (strength > 1)
    strength = 1;

  //artist::drawSensor(absoluteLoc, absoluteOrientation, getViewAngle(), strength);
  latestReading = strength;
  return strength;
}





















