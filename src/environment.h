#pragma once

/**
 * \author Lucas Kramer
 * \editor Himawan Go
 * \file   Environment.h
 * \brief  Main application class for the robot simulation
 *
 */

#include "Location.h"
#include "configuration.h"

#include <unordered_map>
#include <vector>
#include <mutex>
#include <stdexcept>

class PhysicalObject;

class ObjectIterator;

/**
 * \brief environment namespace, handles all the objects as a group.  Also manages
 * object ids and collision detection
 */  
namespace environment {
  /** An object iterator */
  typedef ObjectIterator objectIterator;

  /**
   * \brief Adds an object to the environment
   * \param object The object to add
   * \return The assigned id of the object
   */
  int addObject(PhysicalObject *object);

  /**
   * \brief Removes an object from the environment
   * \param id The id of the object to remove
   */
  void removeObject(int id);

  /**
   * \brief Removes all objects from the environment
   * and resets the id counter
   */
  void clear();

  /**
   * \brief Gets the number of objects in environment
   * \return The number of objects
   */
  unsigned getNumObjects();

  /**
   * \brief Finds an object from the environment
   * \param id The id of the object to find
   * \return The object that was looked iup
   */
  PhysicalObject* getObject(int id);

  /**
   * \brief Gets an iterator to the beginning of the objects
   * \return The iterator
   */
  objectIterator getObjectsBegin();

  /**
   * \brief Gets an iterator to the end of the objects
   * \return The iterator
   */
  objectIterator getObjectsEnd();

  /**
   * Checks if an object is in the designated window
   * \details It returns true whenever the following conditions are met:
   * <tr>
   * <td>x position - radius <= 0</td>
   * <td>y position - radius <= 0</td>
   * <td>x position + radius >= width </td>
   * <td>y position + radius >= height</td>
   * <td>
   * \param l The Location of the object
   * \param r The radius of the object
   * \return true when the object is touching a wall
   */
  bool isTouchingWall(Location l, int r);

  /**
   * Checks if an object is in the designated window
   * \details It reads in the width, height, x and y position
   * of the object, and then returns true whenever the following conditions are
   * met:
   * <tr>
   * <td>x position - radius <= 0</td>
   * <td>y position - radius <= 0</td>
   * <td>x position + radius >= width </td>
   * <td>y position + radius >= height</td>
   * <td>
   * \param id The id of the object
   * \return true when the object is touching a wall
   */
  bool isTouchingWall(int id);

  /**
   * Same as isTouchingObject(Location l, int r), but ignores the object with the
   * given id.
   * \param l The Location of the object
   * \param r The radius of the object
   * \param id The id of the object to ignore
   */
  bool isTouchingObject(Location l, int r, int id);

  /**
   * Same as isTouchingObject(Location l, int r), but ignores the object with the
   * given id and non-hitable objects.
   * \param l The Location of the object
   * \param r The radius of the object
   * \param id The id of the object to ignore
   */
  bool isTouchingHitableObject(Location l, int r, int id);

  /**
   * Checks if the object specified by the given Location and radius is touching
   * any other objects
   * \param l The Location of the object
   * \param r The radius of the object
   * \return true when the object is touching any other object
   */
  bool isTouchingObject(Location l, int r);

  /**
   * Checks if the object specified by the given Location and radius is touching
   * any other hitable objects
   * \param l The Location of the object
   * \param r The radius of the object
   * \return true when the object is touching any other hitable object
   */
  bool isTouchingHitableObject(Location l, int r);

  /**
   * Checks if the object specified by the given id is touching any other objects
   * \param id The id of the object
   * \return true when the object is touching any other object
   */
  bool isTouchingObject(int id);

  /**
   * Checks if the object specified by the given id is touching any other hitable objects
   * \param id The id of the object
   * \return true when the object is touching any other hitable object
   */
  bool isTouchingHitableObject(int id);

  /**
   * Checks if the object specified by the given Location and radius is touching
   * a wall or any other objects
   * \param l The Location of the object
   * \param r The radius of the object
   * \return true when the object is touching a wall or any other object
   * \see isTouchingWall, isTouchingObject
   */
  bool isColliding(Location l, int r);

  /**
   * Checks if the object specified by the given id is touching
   * a wall or any other objects
   * \param id The id of the object
   * \return true when the object is touching a wall or any other object
   * \see isTouchingWall, isTouchingObject
   */
  bool isColliding(int id);

  /**
   * Checks if the object specified by the given Location and radius is touching
   * a wall or any other hitable objects
   * \param l The Location of the object
   * \param r The radius of the object
   * \return true when the object is touching a wall or any other hitable object
   * \see isTouchingWall, isTouchingObject
   */
  bool isCollidingWithHitable(Location l, int r);

  /**
   * Checks if the object specified by the given id is touching
   * a wall or any other hitable objects
   * \param id The id of the object
   * \return true when the object is touching a wall or any other hitable object
   * \see isTouchingWall, isTouchingObject
   */
  bool isCollidingWithHitable(int id);

  /**
   * Determines what object is being hit by another object
   * \param l The Location of the object
   * \param r The radius of the object
   * \param id The id of the object to exclude
   * \return The id of one object that is being hit, or -1 if it is hiting
   * nothing or a wall
   * \see isColliding
   */
  int getCollisionId(Location l, int r, int id);

  /**
   * Determines what hitable object is being hit by another object
   * \param l The Location of the object
   * \param r The radius of the object
   * \param id The id of the object to exclude
   * \return The id of one hitable object that is being hit, or -1 if it is hiting
   * nothing or a wall
   * \see isColliding
   */
  int getHitableCollisionId(Location l, int r, int id);

  /**
   * Determines what object is being hit by another object
   * \param l The Location of the object
   * \param r The radius of the object
   * \return The id of one object that is being hit, or -1 if it is hiting
   * nothing or a wall
   * \see isColliding
   */
  int getCollisionId(Location l, int r);

  /**
   * Determines what hitable object is being hit by another object
   * \param l The Location of the object
   * \param r The radius of the object
   * \return The id of one hitable object that is being hit, or -1 if it is hiting
   * nothing or a wall
   * \see isColliding
   */
  int getHitableCollisionId(Location l, int r);

  /**
   * Determines what object is being hit by another object
   * \param id The id of the object
   * \return The id of one object that is being hit, or -1 if it is hiting
   * nothing or a wall
   * \see isColliding
   */
  int getCollisionId(int id);

  /**
   * Determines what hitable object is being hit by another object
   * \param id The id of the object
   * \return The id of one hitable object that is being hit, or -1 if it is hiting
   * nothing or a wall
   * \see isColliding
   */
  int getHitableCollisionId(int id);
};

/**
 * \brief ObjectIterator class, a simple iterator for the object in environment
 */
class ObjectIterator {
public:
  /**
   * \brief The default constructor, the iterated objects are allways the iterObjects
   * vector in environment
   */
  ObjectIterator();

  /**
   * \brief The copy constructor, the iterated objects are allways the iterObjects
   * vector in environment
   * \param other The ObjectIterator to copy
   */
  ObjectIterator(const ObjectIterator& other);

  /**
   * \brief The destructor, does nothing
   */
  ~ObjectIterator();

  // Operators
  void operator++();
  void operator++(int);
  PhysicalObject* operator*();
  bool operator>(const ObjectIterator& other);
  bool operator<(const ObjectIterator& other);
  bool operator==(const ObjectIterator& other);
  bool operator!=(const ObjectIterator& other);

  friend environment::objectIterator environment::getObjectsBegin();
  friend environment::objectIterator environment::getObjectsEnd();

private:
  /**
   * \brief A private constructor that allows iteration to start at a given index.
   * Used for getObjectsBegin() and getObjectsEnd()
   */
  ObjectIterator(int index);

  unsigned index;
};

/**
 * \brief An exception to be thrown when an open Location cannot be found
 */
class NoOpenLocationException : public std::runtime_error {
public:
   NoOpenLocationException():
     std::runtime_error("Unable to find open Location.") {}
};
