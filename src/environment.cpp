/**
 * \author Lucas Kramer
 * \editor Himawan Go
 * \file   Environment.h
 * \brief  Main application class for the robot simulation
 *
 */

#include "PhysicalObject.h"
#include "environment.h"
#include "configuration.h"
using namespace environment;

#include <iostream>
#include <math.h>
using namespace std;

static int id = 0;
static int numObjects = 0;
static vector<PhysicalObject*> objects;

static mutex objectsMutex;

int environment::addObject(PhysicalObject *object) {
  objectsMutex.lock();
  if (id < (int)objects.size())
    objects[id] = object;
  else
    objects.push_back(object);
  id++;
  numObjects++;
  objectsMutex.unlock();
  return id - 1;
}

void environment::removeObject(int id) {
  objectsMutex.lock();
  if (id < 0) {
    objectsMutex.unlock();
    throw new invalid_argument("Invalid id");
  }
  if (objects[id] != NULL && id < (int)objects.size()) {
    objects[id] = NULL;
    numObjects--;
  }
  objectsMutex.unlock();
}

void environment::clear() {
  objectsMutex.lock();
  for (unsigned i = 0; i < (unsigned)id; i++) {
    if (objects[i] != NULL) {
      objects[i] = NULL;
    }
  }
  numObjects = 0;
  id = 0;
  objectsMutex.unlock();
}

unsigned environment::getNumObjects() {
  return numObjects;
}

PhysicalObject* environment::getObject(int id) {
  if (id >= 0 && id < (int)objects.size())
    return objects[id];
  else
    return NULL;
}

objectIterator environment::getObjectsBegin() {
  // Need to do this to initialize the iterator to the first object
  // Otherwise it can think it isn't done starting out, and run into
  // an error trying to dereference
  ObjectIterator result(-1);
  result++;
  return result;
}

objectIterator environment::getObjectsEnd() {
  return ObjectIterator(objects.size());
}

// Collision stuff

/**
 * Checks if two objects specified by their Locations and radii
 * \return a boolean value that is true when objects are overlapping
 */
bool isTouching(Location l1, int r1, Location l2, int r2) {
  float x_diff = l1.x - l2.x;
  float y_diff = l1.y - l2.y;
  float min_distance = r1 + r2;
  float distance = sqrt(x_diff * x_diff + y_diff * y_diff);
  
  // min_distance should be <= distance
  return ((distance - min_distance) < GET_FLOAT("TOUCHING_DELTA")); 
}

bool environment::isTouchingWall(Location l, int r) {
  int width = GET_INT("DISPLAY_WIDTH");
  int height = GET_INT("DISPLAY_HEIGHT");
  return
    (l.x - r <= 0) ||
    (l.y - r <= 0) ||
    (l.x + r >= width) ||
    (l.y + r >= height);
}

bool environment::isTouchingWall(int id) {
  return isTouchingWall(getObject(id)->getLocation(), getObject(id)->getRadius());
}

bool environment::isTouchingObject(Location l, int r, int id) {
  //  objectsMutex.lock();
  bool result = false;
  for (objectIterator it = getObjectsBegin(); it < getObjectsEnd(); it++) {
    if ((*it)->getId() != id)
      result |= isTouching(l, r, (*it)->getLocation(), (*it)->getRadius());
  }
  //  objectsMutex.unlock();
  return result;
}

bool environment::isTouchingHitableObject(Location l, int r, int id) {
  //  objectsMutex.lock();
  bool result = false;
  for (objectIterator it = getObjectsBegin(); it < getObjectsEnd(); it++) {
    if ((*it)->getId() != id && (*it)->isHitable)
      result |= isTouching(l, r, (*it)->getLocation(), (*it)->getRadius());
  }
  //  objectsMutex.unlock();
  return result;
}

bool environment::isTouchingObject(Location l, int r) {
  return isTouchingObject(l, r, -1);
}

bool environment::isTouchingObject(int id) {
  return isTouchingObject(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

bool environment::isTouchingHitableObject(Location l, int r) {
  return isTouchingHitableObject(l, r, -1);
}

bool environment::isTouchingHitableObject(int id) {
  return isTouchingHitableObject(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

bool environment::isColliding(Location l, int r) {
  return isTouchingWall(l, r) || isTouchingObject(l, r);
}

bool environment::isColliding(int id) {
  return isTouchingWall(id) || isTouchingObject(id);
}

bool environment::isCollidingWithHitable(Location l, int r) {
  return isTouchingWall(l, r) || isTouchingHitableObject(l, r);
}

bool environment::isCollidingWithHitable(int id) {
  return isTouchingWall(id) || isTouchingHitableObject(id);
}

int environment::getCollisionId(Location l, int r, int id) {
  //  objectsMutex.lock();
  for (objectIterator it = getObjectsBegin(); it != getObjectsEnd(); it++) {
    if ((*it)->getId() != id &&
        isTouching(l, r, (*it)->getLocation(), (*it)->getRadius())) {
      objectsMutex.unlock();
      return (*it)->getId();
    }
  }
  //  objectsMutex.unlock();
  return -1;
}

int environment::getHitableCollisionId(Location l, int r, int id) {
  //  objectsMutex.lock();
  for (objectIterator it = getObjectsBegin(); it != getObjectsEnd(); it++) {
    if ((*it)->getId() != id &&
        isTouching(l, r, (*it)->getLocation(), (*it)->getRadius()) &&
        (*it)->isHitable) {
      objectsMutex.unlock();
      return (*it)->getId();
    }
  }
  //  objectsMutex.unlock();
  return -1;
}

int environment::getCollisionId(Location l, int r) {
  return getCollisionId(l, r, -1);
}

int environment::getHitableCollisionId(Location l, int r) {
  return getHitableCollisionId(l, r, -1);
}

int environment::getCollisionId(int id) {
  return getCollisionId(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

int environment::getHitableCollisionId(int id) {
  return getHitableCollisionId(getObject(id)->getLocation(), getObject(id)->getRadius(), id);
}

// ObjectIterator
ObjectIterator::ObjectIterator() {
  this->index = 0;
}

ObjectIterator::ObjectIterator(int index) {
   this->index = index;
}

ObjectIterator::ObjectIterator(const ObjectIterator& other) {
   this->index = other.index;
}

ObjectIterator::~ObjectIterator() {}

void ObjectIterator::operator++() {
  objectsMutex.lock();
  index++;
  while (index < objects.size() && objects[index] == NULL)
    index++;
  objectsMutex.unlock();
}

void ObjectIterator::operator++(int) {
  objectsMutex.lock();
  index++;
  while (index < objects.size() && objects[index] == NULL)
    index++;
  objectsMutex.unlock();
}

PhysicalObject* ObjectIterator::operator*() {
  objectsMutex.lock();
  if (index >= objects.size()) {
    throw new runtime_error("Cannot dereference iterator: No more elements");
  }
  objectsMutex.unlock();
  return objects[index];
}

bool ObjectIterator::operator>(const ObjectIterator& other) {
  return index > other.index;
}

bool ObjectIterator::operator<(const ObjectIterator& other) {
  return index < other.index;
}

bool ObjectIterator::operator==(const ObjectIterator& other) {
  return index == other.index;
}

bool ObjectIterator::operator!=(const ObjectIterator& other) {
  return index != other.index;
}
