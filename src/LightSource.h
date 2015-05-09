#pragma once

/**
 * \author Himawan Go
 * \editor Lucas Kramer
 *
 * \file  LightSource.h
 * \brief The representation of an obstacle in the simulation.
 *
 */

#include "PhysicalObject.h"
#include "configuration.h"

/** \brief LightSource for the robot to seek */
class LightSource: public PhysicalObject {
public:

  /**
   * This constructor requires two parameters: radius and color)
   * \param radius the radius in pixels
   * \param color a struct of float value 0 to 1 representing a hexedecimal color
   * \par Initialization
   * LightSource is initialized with the following values:
   * <tr><td>Orientation: random</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  LightSource(int radius, Color color);

  /**
   * This constructor creates a light with a random radius
   * \param minRadius the minimum possible radius in pixels
   * \param maxRadius the maximum possible radius in pixels
   * \param color a struct representing color
   * \par Initialization
   * LightSource is initialized with the following values:
   * <tr><td>Orientation: random</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  LightSource(int maxRadius, int minRadius, Color color);

  /**
   * This constructor requires four parameters: radius, color, start Location and ConfigTable value)
   * \param radius the radius in pixels
   * \param color a struct of float value 0 to 1 representing a hexedecimal color
   * \param loc a struct of the x,y start Location in pixels
   * \param config a configuration table 
   * \par Initialization
   * LightSource is initialized with the following values:
   * <tr><td>Orientation: 0</td>
   * <td>Speed: 0</td>
   * <td>Position: (loc.x, loc.y) </td>
   * </tr>
   */
  LightSource(int radius, Location loc, Color color);

  /**
   * \author Lucas Kramer
   * This function gives LightSource an appropriate reaction when a collision occurs.
   * \param otherId id of the object that it collides with
   * \param wasHit tells if the object was hit previously
   */
  bool handleCollision(int otherId, bool wasHit);

  /** This is the class destructor.  */
  ~LightSource();

  /** 
   * This function displays the LightSource in the graphics.
   */
  void display();
};
