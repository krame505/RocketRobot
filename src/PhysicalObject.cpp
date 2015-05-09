/**
 * \author Lucas Kramer
 * \editor Himawan Go, Carl Bahn
 * \file   PhysicalObject.cpp
 * \brief  The representation of any object within the simulation
 *
 */


#include <stdlib.h>
#include <iostream>

// Needed on some platforms to access the definition of pi, etc.  
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <stdexcept>
using namespace std;
/* for debugging
#include <chrono>
using namespace std::chrono;
*/
#include "artist.h"
#include "PhysicalObject.h"
#include "environment.h"
#include "configuration.h"
using namespace environment;

PhysicalObject::PhysicalObject(ObjectType objectType,
                               int radius,
                               Color color,
                               bool isHitable) :
  PhysicalObject(objectType,
                 radius,
                 findOpenLocation(radius),
                 color,
                 isHitable) {}

// Constructor with randomized radius
PhysicalObject::PhysicalObject(ObjectType objectType,
                               int maxRadius,
                               int minRadius,
                               Color color,
                               bool isHitable) :
  PhysicalObject(objectType,
                 radius,
                 findOpenLocationRandomized(radius, maxRadius, minRadius),
                 color,
                 isHitable) {}

PhysicalObject::PhysicalObject(ObjectType objectType,
                               int radius,
                               Location loc,
                               Color color,
                               bool isHitable) :
  objectType(objectType),
  isHitable(isHitable),
  loc(loc),
  orientation(rand() % 360),
  speed(0),
  radius(radius),
  color(color) {
  id = addObject(this);
}

PhysicalObject::~PhysicalObject() {
  removeObject(getId());
}

int PhysicalObject::getId() const {
  return id;
}

// Find a new, nonintersecting Location to place the object
Location PhysicalObject::findOpenLocation(int radius) {
  int width = GET_INT("DISPLAY_WIDTH");
  int height = GET_INT("DISPLAY_HEIGHT");
  Location result;
  int attempts = 0;
  
  // Guess Locations until one that touches nobody is found
  do {
    result.x = (rand() % (width - radius * 2)) + radius;
    result.y = (rand() % (height - radius * 2)) + radius;
    attempts++;
    
    // Throw exception after many attempts
    if (attempts > GET_INT("FIND_LOCATION_RETRIES")) {
      throw new NoOpenLocationException;
    }
  } while (isTouchingObject(result, radius));

  assert(result.x >= radius); 
  assert(result.x <= width - radius);
  assert(result.y >= radius);
  assert(result.y <= height - radius);
  return result;
}

// Find a new, nonintersecting Location to place the object, randomized radius
Location PhysicalObject::findOpenLocationRandomized(int &radius,
                                                    int maxRadius, int minRadius) {
  int width = GET_INT("DISPLAY_WIDTH");
  int height = GET_INT("DISPLAY_HEIGHT");
  Location result;
  int attempts = 0;
  
  // Guess Locations until one that touches nobody is found
  do {
    radius = (rand() % (maxRadius - minRadius)) + minRadius;

    result.x = (rand() % (width - radius * 2)) + radius;
    result.y = (rand() % (height - radius * 2)) + radius;

    attempts++;
    
    // Throw exception after many attempts
    if (attempts > GET_INT("FIND_LOCATION_RETRIES")) {
      throw new NoOpenLocationException;
    }
  } while (isTouchingObject(result, radius));

  assert(result.x >= radius);
  assert(result.x <= width - radius);
  assert(result.y >= radius);
  assert(result.y <= height - radius);
  return result;
}

void PhysicalObject::pointTo(const PhysicalObject &other) {
  float x_diff = other.getXPosition() - getXPosition();
  float y_diff = other.getYPosition() - getYPosition();
  int direction = (int)(atan2(x_diff, y_diff) * 180 / M_PI);
  direction = (direction + 360) % 360; //atan2 can return up to -180
  setOrientation(direction);
}

void PhysicalObject::setPosition(float x, float y) {
  int width = GET_INT("DISPLAY_WIDTH");
  int height = GET_INT("DISPLAY_HEIGHT");
  if (x - radius < 0 || y - radius < 0 || x + radius > width || y + radius > height) {
    throw new invalid_argument("setPosition: Invalid position.");
  }

  loc.x = x;
  loc.y = y;

  updateMembers(); // Update positions of sub-objects
}

void PhysicalObject::forceSetPosition(float x, float y) {
  loc.x = x;
  loc.y = y;

  updateMembers();
}

void PhysicalObject::setLocation(Location loc) {
  setPosition(loc.x, loc.y);
}

void PhysicalObject::setOrientation(int orientation) {
  if (0 > orientation || orientation > 360) {
    throw new invalid_argument("setOrientation: Angle out of range.");
  } else {
    this->orientation = orientation;
    updateMembers();
  }
}

void PhysicalObject::reorient(int angle, float distance) {
  // Hack so that it doesn't bother translating after a bunch of tries - prevents infinite recursion
  static int callDepth = 0;
  callDepth++;
  if (callDepth == 1)
    rotate(-angle);
  else
    rotate(angle);
  if (GET_BOOL("TRANSLATE_REORIENT") && callDepth < GET_INT("MAX_REORIENT_RETRIES"))
    translate(distance);
  callDepth--;
}

bool PhysicalObject::translate(float distance) {
  Location originalPosition = loc;

  //sin takes radians, therefore we must convert
  loc.x += distance * sin(orientation * M_PI / 180);
  loc.y += distance * cos(orientation * M_PI / 180);
 
  if (getObject(id) != NULL && isCollidingWithHitable(id)) {
    int collisionId = getHitableCollisionId(id);
    loc = originalPosition;

    updateMembers();
 
    if ((collisionId != -1? getObject(collisionId)->isHitable : true) &&
        handleCollision(collisionId, false)) {
      return true;
    }
    if (collisionId != -1 && isHitable && getObject(collisionId) != NULL) {
      if (getObject(collisionId)->handleCollision(id, true)) {
        return true;
      }
    }
  }
  return false;
}

void PhysicalObject::forceTranslate(float distance) {
  //sin takes radians, therefore we must convert
  loc.x -= distance * sin(orientation * M_PI / 180);
  loc.y += distance * cos(orientation * M_PI / 180);
}

bool PhysicalObject::updatePosition() {
  update();
  updateMembers();

  /*  long current_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
  long time_since_last_update = current_time - prev_update_time;
  prev_update_time = current_time; */

  if (speed > 0)
    return translate(speed / (float)GET_INT("FRAMES_PER_SECOND"));
  else
    return false;
}

void PhysicalObject::rotate(int degrees) {
  if (-360 > degrees || degrees > 360) {
    throw new invalid_argument("rotate: Angle out of range.");
  } else {
    orientation += degrees + 360;
    orientation %= 360;
    setOrientation(orientation);
  }
}

void PhysicalObject::setSpeed(int speed) {
  if (speed < 0) {
    throw new invalid_argument("setSpeed: Invalid speed.");
  }

  this->speed = speed;
}

void PhysicalObject::setRadius(int radius) {
  if (radius < 0) {
    throw new invalid_argument("setRadius: Invalid radius.");
  }

  this->radius = radius;
}

void PhysicalObject::setColor(Color color) {
  this->color = color;
}

float PhysicalObject::getXPosition() const {
  return loc.x;
}

float PhysicalObject::getYPosition() const {
  return loc.y;
}

Location PhysicalObject::getLocation() const {
  return loc;
}

int PhysicalObject::getOrientation() const {
  return orientation;
}

int PhysicalObject::getSpeed() const {
  return speed;
}

int PhysicalObject::getRadius() const {
  return radius;
}

Color PhysicalObject::getColor() const {
  return color;
}

bool PhysicalObject::hasEqualPosition(const PhysicalObject &other) const {
  return ((other.getXPosition() == getXPosition())
        && (other.getYPosition() == getYPosition()));
}

void PhysicalObject::update() {}
void PhysicalObject::updateMembers() {}
void PhysicalObject::display() {
  artist::drawObject(loc, radius, color);
}
