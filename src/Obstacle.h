#pragma once

/**
 * \author Himawan Go
 * \editor Lucas Kramer
 * \file  Obstacle.h
 * \brief The representation of an obstacle in the simulation.
 */

#include "PhysicalObject.h"
#include "configuration.h"

/** \brief Obstacle for the robot to hit/avoid */
class Obstacle : public PhysicalObject {
public:

  /**
   * \param radius the radius in pixels
   * \param color a struct of float value 0 to 1 representing a hexedecimal color
   * \param config a configuration table 
   * \par Initialization
   * Obstacle is initialized with the following values:
   * <tr>
   * <td>Radius: DEFAULT_RADIUS as specified in ../config/default </td>
   * <td>Orientation: random</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  Obstacle(int radius, Color color);
  
  /**
   * \param minRadius the minimum radius in pixels
   * \param maxRadius the maximum radius in pixels
   * \param color a struct of float value 0 to 1 representing a hexedecimal color
   * \par Initialization
   * Obstacle is initialized with the following values:
   * <tr><td>Orientation: random</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  Obstacle(int maxRadius, int minRadius, Color color);

  /**
   * \param radius the radius in pixels
   * \param color a struct of float value 0 to 1 representing a hexedecimal color
   * \param loc a struct of the x,y start Location in pixels
   * \par Initialization
   * Obstacle is initialized with the following values:
   * <tr><td>Orientation: 0</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  Obstacle(int radius, Location loc, Color color);

  /**
   * \author Lucas Kramer
   * This function gives Obstacle an appropriate reaction when a collision occurs.  Does nothing for now
   * \param otherId id of the object that it collides with
   * \param wasHit tells if the object was hit previously
   */
  bool handleCollision(int otherId, bool wasHit);

  /** This is the class destructor.  */
  ~Obstacle();
};
