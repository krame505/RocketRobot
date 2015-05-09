#pragma once

/**
 * \author Himawan Go
 * \editor Lucas Kramer
 * \file  Target.h
 * \brief The representation of an obstacle in the simulation.
 *
 */

#include "PhysicalObject.h"

/** \brief Target for the robot to seek */
class Target: public PhysicalObject {
public:
  Target();

  Target(int radius, Color color, int targetNum=TARGET);

  Target(int maxRadius, int minRadius, Color color, int targetNum=TARGET);

  Target(int radius, Location loc, Color color, int targetNum=TARGET);

  /**
   * \author Lucas Kramer
   * This function gives Target an appropriate reaction when a collision occurs.
   * \param otherId id of the object that it collides with
   * \param wasHit tells if the object was hit previously
   */
  bool handleCollision(int otherId, bool wasHit);

  /** This is the class destructor.  */
  ~Target();
};
