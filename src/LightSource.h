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
  LightSource(int radius,
              Color color,
              Environment *env = Environment::getEnv());

  LightSource(int maxRadius, int minRadius,
              Color color,
              Environment *env = Environment::getEnv());

  LightSource(int radius, Location loc,
              Color color,
              Environment *env = Environment::getEnv());

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
