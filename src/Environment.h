#pragma once

/**
 * \author Lucas Kramer
 * \editor Himawan Go
 * \file   Environment.h
 * \brief  Provides environment information to objects.  
 *
 */

#include "Location.h"
#include "configuration.h"

#include <unordered_map>
#include <vector>
#include <mutex>
#include <stdexcept>

class PhysicalObject;

/**
 * \brief environment namespace, handles all the objects as a group.  Also manages
 * object ids and collision detection
 */  
class Environment {
public:
  class iterator;

  /**
   * \brief Environment constructor
   * \param width The width of the environment
   * \param height The height of the environment
   */
  Environment(int width = GET_INT("DISPLAY_WIDTH"),
              int height = GET_INT("DISPLAY_HEIGHT"));

  /**
   * \brief Environment destructor
   * \param width The width of the environment
   * \param height The height of the environment
   */
  ~Environment();

  /**
   * Creates a new environment with the given dimensions and sets it as the current
   * environment.  The user is responsible for saving or deleting the old environment.  
   * \param width The width of the environment
   * \param height The height of the environment
   * \returns a pointer to the environment
   */
  static Environment *newEnv(int width, int height);

  /**
   * Gets the current environment referenced by all functions.  If uninitialized, it is
   * created with the default values.  
   * \returns a pointer to the environment
   */
  static Environment *getEnv();

  /**
   * Sets the current environment referenced by all functions.  The user is responsible for
   * saving or deleting the old environment.  
   * \param newEnv The new environment
   */
  static void setEnv(Environment *newEnv);

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
  Environment::iterator begin();

  /**
   * \brief Gets an iterator to the end of the objects
   * \return The iterator
   */
  Environment::iterator end();

  /**
   * \brief Gets the width of the environment
   * \return The width
   */
  int getWidth();

  /**
   * \brief Gets the height of the environment
   * \return The height
   */
  int getHeight();

  /**
   * \brief Sets the width of the environment
   * \param width The new width
   */
  void setWidth(int width);

  /**
   * \brief Sets the height of the environment
   * \param height The new height
   */
  void setHeight(int height);

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

  /**
   * \brief a simple iterator for the objects in the environment
   */
  class iterator {
  public:
    /**
     * \brief The default constructor, the iterated objects are always the objects
     * vector in the environment
     */
    iterator(Environment *env);

    /**
     * \brief The copy constructor, the iterated objects are always the objects
     * vector in environment
     * \param other The iterator to copy
     */
    iterator(const iterator& other);

    /**
     * \brief The destructor, does nothing
     */
    ~iterator();

    // Operators
    void operator++();
    void operator++(int);
    PhysicalObject* operator*();
    bool operator>(const iterator& other);
    bool operator<(const iterator& other);
    bool operator==(const iterator& other);
    bool operator!=(const iterator& other);

    friend Environment::iterator Environment::begin();
    friend Environment::iterator Environment::end();

  private:
    /**
     * \brief A private constructor that allows iteration to start at a given index.
     * Used for getObjectsBegin() and getObjectsEnd()
     */
    iterator(Environment *env, int index);

    Environment *env;

    unsigned index;
  };

private:
  int id;
  int numObjects;
  std::vector<PhysicalObject*> objects;

  std::mutex objectsMutex;

  int width, height;

  static Environment *currentEnv;
};

/**
 * \brief An exception to be thrown when an open Location cannot be found
 */
class NoOpenLocationException : public std::runtime_error {
public:
   NoOpenLocationException():
     std::runtime_error("Unable to find open Location.") {}
};
