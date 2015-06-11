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
  Obstacle(int radius,
           Color color,
           Environment *env = Environment::getEnv());
  
  Obstacle(int maxRadius, int minRadius,
           Color color,
           Environment *env = Environment::getEnv());

  Obstacle(int radius, Location loc,
           Color color,
           Environment *env = Environment::getEnv());

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
