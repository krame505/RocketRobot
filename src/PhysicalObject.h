#pragma once

/**
 * \author Lucas Kramer
 * \editor Himawan Go, Carl Bahn
 * \file   PhysicalObject.h
 * \brief  The representation of any object within the simulation
 */

#include "Color.h"
#include "configuration.h"
#include "Location.h"
#include "Environment.h"

enum ObjectType {ROBOT, TARGET, OBSTACLE, LIGHT, LAST=LIGHT};

/** \brief This is a common superclass for all objects */  
class PhysicalObject {
public:
  PhysicalObject(ObjectType objectType, int radius = GET_INT("DEFAULT_RADIUS"),
                 Color color = Color('?'),
                 bool isHitable = true,
                 Environment *env = Environment::getEnv());
  PhysicalObject(ObjectType objectType, int maxRadius, int minRadius,
                 Color color = Color('?'),
                 bool isHitable = true,
                 Environment *env = Environment::getEnv());
  PhysicalObject(ObjectType objectType, int radius, Location loc,
                 Color color = Color('?'),
                 bool isHitable = true,
                 Environment *env = Environment::getEnv());

  /** This is the class destructor, it is implemented by all subclasses.  */
  virtual ~PhysicalObject();

  const ObjectType objectType;
  const bool isHitable;

  /**
   * Getter function for the object's id.  
   * \return The object's id
   */
  int getId() const;

  /**
   * Sets the position for the object
   * \param x x position in pixels
   * \param y y position in pixels
   */
  void setPosition(float x, float y); // xy position, in pixels

  /**
   * Sets the position for the object.  It does not throw an exception on invalid input.  
   * \param x position in pixels
   * \param y position in pixels
   */
  void forceSetPosition(float x, float y); // Always sets the position without throwing an exception, for testing. 

  /** 
   * Sets the position for the object
   * for the object
   * \param loc A struct of the x,y Location in pixels
   * \see setPosition
   */
  void setLocation(Location loc); // Same as setPosition, but takes a struct.  

  /** 
   * This function can be called by subclasses when handleing a collision.  
   * This causes the robot to rotate by the specified amount, and then translate
   * if that is enabled in the configuration.  Since translating could cause
   * another collision, this could call back to reorient, causing infinite recusion
   * in some cases.  To prevent this, the function ommits the translate if it is
   * more than MAX_REORIENT_RETRIES deep in recursive calls, set in the configuration
   * \param angle Angle to rotate in degrees
   * \param distance Distance to move in pixels
   */
  void reorient(int angle, float distance);

  /**
   * Translates in the current direction, and calls handleCollision where needed
   * \param distance Distance to move in pixels
   * \return true if objects were added or removed when any collisions were handled.  
   * This is to allow the position update function to break out of the loop when
   * objects are removed, to prevent trying to update objects that no longer exist
   */
  bool translate(float distance);

  /**
   * Translates without calling handleCollision.  This may cause objects to overlap
   * \param distance Distance to move in pixels
   */
  void forceTranslate(float distance);

  /**
   * Sets the absolute orientation for an object
   * \param degrees New angle for the object in degrees clockwise of North.
   */ 
  void setOrientation(int degrees); // orientation in degrees

  /**
   * Sets the speed of an object
   * \param pps The new speed in pixels per second
   */
  void setSpeed(int pps); // speed in pixels per second

  /**
   * Set the radius of the object
   * \param radius the new radius in pixels
   */
  void setRadius(int radius);

  /**
   * Set a new color for the object.
   * \param color 
   * \see Color
   */
  void setColor(Color color);

  /**
   * Returns the x position in pixels of the object
   * \return x position, horizontal from left
   */
  float getXPosition() const;

  /**
   * Returns the y position in pixels of the object
   * \return y position, vertical from bottom
   */
  float getYPosition() const;

  /**
   * Returns the x and y Location of the object in pixels in a struct
   * \see Location
   */
  Location getLocation() const;

  /**
   * Returns the orientation of the object in degrees
   * \return orientation in degrees
   */
  int getOrientation() const;

  /**
   * Returns the speed of the object
   * \return speed in pixels per second
   */
  int getSpeed() const;

  /**
   * Returns the radius of the object
   * \return radius
   */
  int getRadius() const;

  /**
   * Returns the color of the object
   * \return color
   */
  Color getColor() const;

  /** 
   * Calculates the direction of the object is facing towards the target object
   * based on position of the two objects using the math function atan2
   * \param other A PhysicalObject class pointer
   */
  void pointTo(const PhysicalObject &other);

  /** This function increments the object's orientation
   * \param degrees Angle to be incremented clockwise.
   * \exception Input must be within (-360,360)
   */
  void rotate(int degrees); // distance in degrees

  /** This member function checks when two objects are at the same Location,
   * it has one parameter
   * \param other The object to compare
   * \return A boolean value that is true when objects are overlapping
   */
  bool hasEqualPosition(const PhysicalObject &other) const;

  /**
   * This member function uses the speed and time to update the Location
   * \return true if objects were added or removed
   */
  bool updatePosition();

  /**
   * This is implemented by each class, and performs random updates,
   * e.g. point toward target
   */
  virtual void update();

  /**
   * This is implemented by each class, and perfoms updates such as setting the
   * position of sub-objects
   */
  virtual void updateMembers();

  /**
   * This is implemented by each class, normally just calls displayAsCircle.  
   * \see displayAsCircle()
   */
  virtual void display();

  /**
   * This is implemented by each class, called before display, normally does nothing.  
   * \see displayAsCircle()
   */
  virtual void displayBackground();

  /**
   * This is implemented by each class. It controls the objects behavior 
   * on collision during translate, normally just calls reorient.  
   * \param otherId The id of the object it hit
   * \param wasHit true if the object hit somthing, false if hit by somthing (see translate)
   * \return true if objects were added or removed
   * \see reorient()
   */
  virtual bool handleCollision(int otherId, bool wasHit) = 0;

  /**
   * This is implemented by each class. It controls the objects behavior 
   * when interacting with a non-hitable object during translate, normally 
   * just calls reorient.  
   * \param otherId The id of the object it hit
   * \return true if objects were added or removed
   * \see handleCollision(), reorient()
   */
  virtual bool handleNonCollision(int otherId);

protected:
  Environment *env;

  /**
   * This function finds an open Location to place the object
   * \param radius The radius of the object
   */
  static Location findOpenLocation(Environment *env, int radius);

private:
  int id;
  Location loc;
  int orientation;
  int speed;
  int radius;
  Color color;

  /**
   * This function finds an open Location to place the object, as well as
   * generating a random radius
   * \param radius A reference to an int that is set to the randomly generated radius
   * \param maxRadius The maximum radius that can generated
   * \param minRadius The minimum radius that can generated
   */
  static Location findOpenLocationRandomized(Environment *env,
                                             int &radius,
                                             int maxRadius,
                                             int minRadius);

};
